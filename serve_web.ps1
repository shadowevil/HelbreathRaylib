# Simple HTTP server with CORS headers for SharedArrayBuffer support
# Serves files from Build/Web/<Configuration> directory

param(
    [Parameter(Mandatory=$true)]
    [ValidateSet("Debug", "Release", IgnoreCase=$true)]
    [string]$Configuration,

    [int]$Port = 8000
)

# Normalize Configuration to proper case
$Configuration = (Get-Culture).TextInfo.ToTitleCase($Configuration.ToLower())

$webBuildPath = Join-Path $PSScriptRoot "Build\Web\$Configuration"

# Check if build directory exists
if (-not (Test-Path $webBuildPath)) {
    Write-Host "Error: Build directory not found: $webBuildPath" -ForegroundColor Red
    Write-Host "Please run: .\build_web.ps1 -Configuration $Configuration" -ForegroundColor Yellow
    exit 1
}

# Check if helbreath_web.html exists
if (-not (Test-Path (Join-Path $webBuildPath "helbreath_web.html"))) {
    Write-Host "Error: helbreath_web.html not found in $webBuildPath" -ForegroundColor Red
    Write-Host "Please run: .\build_web.ps1 -Configuration $Configuration" -ForegroundColor Yellow
    exit 1
}

# Kill any process already using the port
Write-Host "Checking if port $Port is already in use..." -ForegroundColor Yellow
$connections = Get-NetTCPConnection -LocalPort $Port -ErrorAction SilentlyContinue
if ($connections) {
    Write-Host "Port $Port is in use. Stopping existing process(es)..." -ForegroundColor Yellow
    foreach ($conn in $connections) {
        try {
            Stop-Process -Id $conn.OwningProcess -Force -ErrorAction SilentlyContinue
            Write-Host "  Killed process $($conn.OwningProcess)" -ForegroundColor Green
        } catch {
            Write-Host "  Warning: Could not kill process $($conn.OwningProcess)" -ForegroundColor Red
        }
    }
    # Wait a moment for the port to be released
    Start-Sleep -Milliseconds 500
}

$url = "http://localhost:$Port/"

$listener = New-Object System.Net.HttpListener
$listener.Prefixes.Add($url)

try {
    $listener.Start()
} catch {
    Write-Host ""
    Write-Host "Error: Failed to start server on port $Port" -ForegroundColor Red
    Write-Host "The port may still be in use. Try running the script again." -ForegroundColor Yellow
    exit 1
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Server Started Successfully!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "  Configuration: $Configuration" -ForegroundColor White
Write-Host "  Open in browser: " -NoNewline
Write-Host "$($url)helbreath_web.html" -ForegroundColor Yellow
Write-Host ""
Write-Host "  Serving from: $webBuildPath" -ForegroundColor DarkGray
Write-Host ""
Write-Host "  Press Ctrl+C to stop the server" -ForegroundColor White
Write-Host ""

# Handle Ctrl+C gracefully
[Console]::TreatControlCAsInput = $false
$null = Register-EngineEvent -SourceIdentifier PowerShell.Exiting -Action {
    if ($listener -and $listener.IsListening) {
        $listener.Stop()
        $listener.Close()
    }
}

try {
    while ($listener.IsListening) {
        # Use async Begin/End pattern with timeout to allow Ctrl+C to work
        $contextTask = $listener.GetContextAsync()

        while (-not $contextTask.AsyncWaitHandle.WaitOne(200)) {
            # Check if Ctrl+C was pressed
            if ([Console]::KeyAvailable) {
                $key = [Console]::ReadKey($true)
                if ($key.Key -eq 'C' -and $key.Modifiers -eq 'Control') {
                    throw [System.OperationCanceledException]::new("Server stopped by user")
                }
            }
        }

        $context = $contextTask.GetAwaiter().GetResult()
        $request = $context.Request
        $response = $context.Response

        # Get the requested file path
        $localPath = $request.Url.LocalPath
        if ($localPath -eq "/") { $localPath = "/helbreath_web.html" }
        $filePath = Join-Path $webBuildPath $localPath.TrimStart('/')

        # Set CORS headers for SharedArrayBuffer and AudioWorklet
        $response.Headers.Add("Cross-Origin-Opener-Policy", "same-origin")
        $response.Headers.Add("Cross-Origin-Embedder-Policy", "require-corp")
        $response.Headers.Add("Cross-Origin-Resource-Policy", "cross-origin")
        $response.Headers.Add("Cache-Control", "no-store, no-cache, must-revalidate")

        if (Test-Path $filePath) {
            # Determine content type
            $extension = [System.IO.Path]::GetExtension($filePath)
            $contentType = switch ($extension) {
                ".html" { "text/html" }
                ".js"   { "application/javascript" }
                ".wasm" { "application/wasm" }
                ".data" { "application/octet-stream" }
                default { "application/octet-stream" }
            }

            $response.ContentType = $contentType
            $content = [System.IO.File]::ReadAllBytes($filePath)
            $response.ContentLength64 = $content.Length
            $response.OutputStream.Write($content, 0, $content.Length)
            $timestamp = Get-Date -Format "HH:mm:ss"
            Write-Host "[$timestamp] " -NoNewline -ForegroundColor DarkGray
            Write-Host "$($request.HttpMethod) " -NoNewline -ForegroundColor Cyan
            Write-Host "$($request.Url.LocalPath) " -NoNewline
            Write-Host "200 OK" -ForegroundColor Green
        } else {
            $response.StatusCode = 404
            $buffer = [System.Text.Encoding]::UTF8.GetBytes("404 - File Not Found: $localPath")
            $response.ContentLength64 = $buffer.Length
            $response.OutputStream.Write($buffer, 0, $buffer.Length)
            $timestamp = Get-Date -Format "HH:mm:ss"
            Write-Host "[$timestamp] " -NoNewline -ForegroundColor DarkGray
            Write-Host "$($request.HttpMethod) " -NoNewline -ForegroundColor Cyan
            Write-Host "$($request.Url.LocalPath) " -NoNewline
            Write-Host "404 Not Found" -ForegroundColor Red
        }

        $response.Close()
    }
} catch [System.OperationCanceledException] {
    Write-Host ""
    Write-Host "Server stopped by user" -ForegroundColor Yellow
} catch {
    Write-Host ""
    Write-Host "Error: $_" -ForegroundColor Red
} finally {
    if ($listener -and $listener.IsListening) {
        $listener.Stop()
    }
    if ($listener) {
        $listener.Close()
    }
    Write-Host "Server stopped" -ForegroundColor Yellow
}
