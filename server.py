from http.server import HTTPServer, BaseHTTPRequestHandler
import subprocess
import shlex
import os

PORT = 8080

class ShellCommandHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        path = self.path[1:]

        try:
            file = os.path.join(os.getcwd(), "notes", f"{path}.md")

            if not os.path.exists(file):
                self.send_response(404)
                self.end_headers()
                return

            command = ['./result/bin/uni-compiler']

            with open(file, "r") as f:
                process = subprocess.Popen(
                    command,
                    stdin=f,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True,
                )
                stdout, stderr = process.communicate()

                self.send_response(200)
                self.end_headers()
                self.wfile.write(stdout.encode())
        except Exception as e:
            print(e)


address = ('', PORT)
httpd = HTTPServer(address, ShellCommandHandler)
print(f"Server running on port {PORT}")
httpd.serve_forever()
