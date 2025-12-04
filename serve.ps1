# Simple HTTP server with CORS headers for SharedArrayBuffer support
# Serves files from the web_build directory

param(
    [int]$port = 8000
)

$webBuildPath = Join-Path $PSScriptRoot "web_build"

# Check if web_build directory exists
if (-not (Test-Path $webBuildPath)) {
    Write-Host "Error: web_build directory not found!" -ForegroundColor Red
    Write-Host "Please run build_web.ps1 first to generate the web build."
    exit 1
}

$url = "http://localhost:$port/"

$listener = New-Object System.Net.HttpListener
$listener.Prefixes.Add($url)
$listener.Start()

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Server Started Successfully!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
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

        # Set CORS headers for SharedArrayBuffer (if needed in future)
        $response.Headers.Add("Cross-Origin-Opener-Policy", "same-origin")
        $response.Headers.Add("Cross-Origin-Embedder-Policy", "require-corp")
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
