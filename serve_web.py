#!/usr/bin/env python3
"""
Simple HTTP server with CORS headers required for SharedArrayBuffer/pthreads
Serves files from Build/Web/<Configuration> directory
"""
from http.server import HTTPServer, SimpleHTTPRequestHandler
import sys
import os
import argparse

class CORSRequestHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, directory=None, **kwargs):
        self.custom_directory = directory
        super().__init__(*args, directory=directory, **kwargs)

    def end_headers(self):
        # Required headers for SharedArrayBuffer (if needed in future)
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate')
        super().end_headers()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Serve web build files')
    parser.add_argument('-c', '--configuration', type=str,
                       required=True, help='Build configuration (required: Debug or Release)')
    parser.add_argument('-p', '--port', type=int, default=8000,
                       help='Port number (default: 8000)')

    args = parser.parse_args()

    # Normalize configuration to proper case (case-insensitive input)
    config_lower = args.configuration.lower()
    if config_lower == 'debug':
        config = 'Debug'
    elif config_lower == 'release':
        config = 'Release'
    else:
        print(f'Error: Invalid configuration "{args.configuration}"')
        print('Valid options: Debug, Release (case-insensitive)')
        sys.exit(1)

    build_dir = os.path.join('Build', 'Web', config)

    # Check if build directory exists
    if not os.path.exists(build_dir):
        print(f'Error: Build directory not found: {build_dir}')
        print(f'Please run: build_web.ps1 -Configuration {config}')
        sys.exit(1)

    # Check if helbreath_web.html exists
    html_file = os.path.join(build_dir, 'helbreath_web.html')
    if not os.path.exists(html_file):
        print(f'Error: helbreath_web.html not found in {build_dir}')
        print(f'Please run: build_web.ps1 -Configuration {config}')
        sys.exit(1)

    # Create handler with custom directory
    handler = lambda *args, **kwargs: CORSRequestHandler(*args, directory=build_dir, **kwargs)

    server = HTTPServer(('localhost', args.port), handler)
    print('')
    print('=' * 40)
    print('  Server Started Successfully!')
    print('=' * 40)
    print('')
    print(f'  Configuration: {config}')
    print(f'  Open in browser: http://localhost:{args.port}/helbreath_web.html')
    print('')
    print(f'  Serving from: {os.path.abspath(build_dir)}')
    print('')
    print('  Press Ctrl+C to stop the server')
    print('')

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print('')
        print('Server stopped by user')
        server.shutdown()
