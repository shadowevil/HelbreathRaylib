#!/usr/bin/env python3
"""
Simple HTTP server with CORS headers required for SharedArrayBuffer/pthreads
Serves files from the web_build directory
"""
from http.server import HTTPServer, SimpleHTTPRequestHandler
import sys
import os

class CORSRequestHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        # Change to web_build directory
        super().__init__(*args, directory='web_build', **kwargs)

    def end_headers(self):
        # Required headers for SharedArrayBuffer (if needed in future)
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate')
        super().end_headers()

if __name__ == '__main__':
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 8000

    # Check if web_build directory exists
    if not os.path.exists('web_build'):
        print('Error: web_build directory not found!')
        print('Please run build_web.ps1 first to generate the web build.')
        sys.exit(1)

    server = HTTPServer(('localhost', port), CORSRequestHandler)
    print(f'Starting server on http://localhost:{port}')
    print(f'Serving files from: {os.path.abspath("web_build")}')
    print('Press Ctrl+C to stop')
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print('\nShutting down server')
        server.shutdown()
