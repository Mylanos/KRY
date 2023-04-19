import os
import time
import argparse
import socket
from Crypto.PublicKey import RSA
import rsa
import hashlib
host_name = "localhost"
server_port = 8080
key_folder = "./cert/"
RSA_key_size = 2048


class CryptoUtils:

    def generate_octet_string_except_zero(self, length):
        i = 1
        result = os.urandom(1)
        i += 1
        while i <= length:
            random_bytes = os.urandom(1)
            random_int = int.from_bytes(random_bytes, byteorder="big")
            if random_int != 0:
                result += random_bytes
            i += 1
        return result

    def generate_octet_string(self, length):
        return os.urandom(length)

    def sign_md5_with_padding(self, message, private_key):
        md5_hash = hashlib.md5(message.encode()).digest()
        hash_len = len(md5_hash)

        # 3  bytes for reserved hexa numbers
        padding_len = private_key.size_in_bytes() - hash_len - 3

        # following the PKCS #1 v1.5 Padding Scheme
        padding_bytes = b"\x00\x02" + \
            (self.generate_octet_string_except_zero(padding_len)) + b"\x00"

        padding_hash = padding_bytes + md5_hash

        # sign padded hash with private key
        padded_hash_int = int.from_bytes(padding_hash, byteorder='big')
        signature_int = pow(padded_hash_int, private_key.d, private_key.n)
        signature_bytes = signature_int.to_bytes(
            (signature_int.bit_length() + 7) // 8, byteorder='big')
        # print("geg : " + str(len(signature_bytes)))
        # print("int : " + str(signature_int))
        # print("bytes : " + str(int.from_bytes(signature_bytes, byteorder='big')))
        test = pow(signature_int, private_key.e, private_key.n)
        # print("decrpyted : " + str(test))
        # print("padded hash int : " + str(padded_hash_int))
        print("padded hash : " + str(padding_hash))
        # print("padded hash calc : " + str(padded_hash_int.to_bytes(
        #    (padded_hash_int.bit_length() + 7) // 8, byteorder='big')))

        # print(f"PRIVATE - E: {private_key.e} N: {private_key.n}")
        return signature_bytes

    # credits to https://www.youtube.com/watch?v=bcBBEdpLhlg&t=240s&ab_channel=SoftwareSecurityandCryptography
    def verify_signature(self, message, signature, public_key):
        # Hash the message using MD5
        message_hash = hashlib.md5(message.encode()).digest()
        signature_int = int.from_bytes(signature, byteorder='big')

        decrypted_signature_int = pow(
            signature_int, public_key.e, public_key.n)

        decrypted_signature_bytes = decrypted_signature_int.to_bytes(
            ((decrypted_signature_int.bit_length() + 7) // 8) + 1, byteorder='big')

        # Calculate the length of the hash
        hash_len = len(message_hash)
        if (len(message) > public_key.size_in_bytes()):
            return False
        if (decrypted_signature_bytes[0] != 0 or decrypted_signature_bytes[1] != 2):
            print("b")
            print(str(decrypted_signature_bytes[0]))
            print(str(decrypted_signature_bytes[1]))
            return False

        i = 2
        while i < public_key.size_in_bytes():
            if decrypted_signature_bytes[i] == 0:
                i += 1
                break
            i += 1
        if i == public_key.size_in_bytes():
            print("c")
            return False

        signature_hash = decrypted_signature_bytes[i:]
        return signature_hash == message_hash

    def generate_or_load_RSA_keypair(self, owner):
        # Create the key directory if it does not already exist
        os.makedirs(key_folder, exist_ok=True)
        publicKey = None
        privateKey = None
        if os.path.exists(key_folder + owner + 'publicKey.pem') and os.path.exists(key_folder + owner + 'privateKey.pem'):
            with open(key_folder + owner + 'publicKey.pem', 'rb') as f:
                publicKey = RSA.import_key(f.read())
            with open(key_folder + owner + 'privateKey.pem', 'rb') as f:
                privateKey = RSA.import_key(f.read())
        else:
            privateKey = RSA.generate(RSA_key_size)
            publicKey = privateKey.publickey()
            with open(key_folder + owner + 'publicKey.pem', 'wb') as p:
                p.write(publicKey.export_key('PEM'))
            with open(key_folder + owner + 'privateKey.pem', 'wb') as p:
                p.write(privateKey.export_key('PEM'))

        return (publicKey, privateKey)

    def load_RSA_public_key(self, owner):
        if os.path.exists(key_folder + owner + 'publicKey.pem'):
            with open(key_folder + owner + 'publicKey.pem', 'rb') as f:
                publicKey = RSA.import_key(f.read())
        else:
            print("Error: opening the reciever public key!")
            exit(1)

        return publicKey


class MyClient:
    def __init__(self, port=8080, ip_addr='localhost'):
        self.c_util = CryptoUtils()
        self.RSA_pubkey, self.RSA_privkey = self.c_util.generate_or_load_RSA_keypair(
            "RSA_client")
        self.RSA_pubkey_reciever = self.c_util.load_RSA_public_key(
            "RSA_server")
        self.port = port
        self.ip_addr = ip_addr

        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.public_key = None
        self.private_key = None
        self.public_key_receiver = None
        self.run()

    def run(self):

        try:
            while True:
                try:
                    self.socket.connect((self.ip_addr, self.port))
                    print(f"Successfully connected server")
                    print(
                        f"RSA_public_key_receiver= {self.RSA_pubkey.export_key('DER')}\n")
                    print(
                        f"RSA_private_key_receiver= {self.RSA_privkey.export_key('DER')}\n")
                    print(
                        f"RSA_private_key_receiver= {self.RSA_privkey.export_key('DER')}\n")
                    break
                except:
                    print(
                        "ERROR: Failed to establish conection. Gonna sleep briefly & try again")
                    time.sleep(10)
                    continue
            while True:

                # get a message
                message = input("Enter your value: ")

                # hash it and create signature
                signature = self.c_util.sign_md5_with_padding(
                    message, self.RSA_privkey)

                package = signature + message.encode('utf8')
                # generate symmetric key and encrypt it

                self.socket.send(package)

        except KeyboardInterrupt:
            print("Client is being stopped.")
            self.socket.close()


class MyServer():
    def __init__(self, port=8080, ip_addr="localhost"):
        self.c_util = CryptoUtils()
        self.RSA_pubkey, self.RSA_privkey = self.c_util.generate_or_load_RSA_keypair(
            "RSA_server")
        self.RSA_pubkey_sender = self.c_util.load_RSA_public_key("RSA_client")

        self.port = port
        self.ip_addr = ip_addr
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind((ip_addr, port))
        self.public_key = None
        self.private_key = None

        self.run()

    def run(self):
        self.socket.listen()

        try:
            while True:
                client_socket, client_address = self.socket.accept()

                print(f"Client has joined")
                print(
                    f"RSA_public_key_receiver= {self.RSA_pubkey.export_key('DER')}\n")
                print(
                    f"RSA_private_key_receiver= {self.RSA_privkey.export_key('DER')}\n")
                try:
                    while True:
                        # Receive the message from the client
                        response = client_socket.recv(1024)
                        signature = response[:256]
                        message = response[256:]
                        if (self.c_util.verify_signature(
                                message.decode('utf8'), signature, self.RSA_pubkey_sender)):
                            print("VERIFIED MESSAGE")
                        else:
                            print("THERE WAS AN ERROR IN VERIFICAIT")

                        number = int.from_bytes(signature, byteorder='big')
                        print(
                            f"Signature: {number}")
                        print(
                            f"Received message from: {message.decode('utf8')}")
                except Exception as e:
                    print(
                        f'Error processing message from {client_address}: {e}')

                finally:
                    # Close the client socket
                    client_socket.close()
        except KeyboardInterrupt:
            print("Server is being stopped.")
        except Exception as e:
            print(f'Server has encountered error: {e}')
        finally:
            self.socket.close()


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
        MyClient(int(args.port), 'localhost')

    if (args.type == "s" or args.type == "server"):
        MyServer(int(args.port), 'localhost')
