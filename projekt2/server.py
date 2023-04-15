import time
import argparse
import socket
import hashlib
hostName = "localhost"
serverPort = 8080


class MyClient:
    def __init__(self, port=8080, ip_addr='localhost'):
        self.port = port
        self.ip_addr = ip_addr

        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.public_key = None
        self.private_key = None
        self.public_key_receiver = None
        self.run()

    def run(self):
        print(f'Client is starting on {self.port}')
        self.client_socket.connect((self.ip_addr, self.port))

        # Send a message
        message = input("Enter your value: ")
        # message = 'Hello from the client!'
        self.client_socket.send(message.encode())
        result = hashlib.md5(bytes(message))

        # response
        response = self.client_socket.recv(1024).decode()
        print(f'Received response from server: {response}')

        self.client_socket.close()


class MyServer():
    def __init__(self, port=8080, ip_addr="localhost"):
        self.port = port
        self.ip_addr = ip_addr
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.bind((ip_addr, port))
        self.public_key = None
        self.private_key = None

        self.run()

    def run(self):
        self.server_socket.listen()

        try:
            while True:
                client_socket, client_address = self.server_socket.accept()

                # Receive the message from the client
                message = client_socket.recv(1024).decode()
                print(f'Received message from {client_address}: {message}')

                # Send a response back to the client
                response = 'Hello from the server!'
                client_socket.send(response.encode())

                # Close the connection
                client_socket.close()
        except KeyboardInterrupt:
            print("Server stopped.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog='kry2.py',
        description='Hybrid crypto project',
        epilog='Good luck warrior')

    parser.add_argument(
        '-t', '--type', help='client (c) / server (s)', required=True)
    parser.add_argument('-p', '--port', help='port', required=True)

    args = parser.parse_args()

    if (args.type == "c" or args.type == "client"):
        MyClient()

    if (args.type == "s" or args.type == "server"):
        MyServer()
