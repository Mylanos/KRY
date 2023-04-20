import os
import time
import argparse
import socket
from Crypto.PublicKey import RSA
from Crypto.Cipher import AES
import rsa
import hashlib
host_name = "localhost"
server_port = 8080
key_folder = "./cert/"
RSA_key_size = 2048
AES_key_size = 128


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

    def rsa_sign_md5_with_padding(self, message, private_key):
        # calc MD5
        md5_hash = hashlib.md5(message.encode()).digest()
        hash_len = len(md5_hash)
        print("c:MD5=" + str(md5_hash))
        # 3  bytes for reserved hexa numbers
        padding_len = private_key.size_in_bytes() - hash_len - 3

        # padding the message following the PKCS #1 v1.5 Padding Scheme
        padding_bytes = b"\x00\x02" + \
            (self.generate_octet_string_except_zero(padding_len)) + b"\x00"

        padding_hash = padding_bytes + md5_hash
        print("c:MD5_padding=" + str(padding_hash) + "\n")

        # sign padded hash with private key
        padded_hash_int = int.from_bytes(padding_hash, byteorder='big')
        signature_int = pow(padded_hash_int, private_key.d, private_key.n)
        signature_bytes = signature_int.to_bytes(
            (signature_int.bit_length() + 7) // 8, byteorder='big')

        print("c:RSA_MD5_hash=" + str(signature_bytes) + "\n")
        return signature_bytes

    # Hash the message using MD5
    def generate_MD5(self, content):
        return hashlib.md5(content).digest()

    # credits to https://www.youtube.com/watch?v=bcBBEdpLhlg&t=240s&ab_channel=SoftwareSecurityandCryptography
    def verify_signature(self, content, signature, public_key):
        signature_int = int.from_bytes(signature, byteorder='big')

        decrypted_signature_int = pow(
            signature_int, public_key.e, public_key.n)

        decrypted_signature_bytes = decrypted_signature_int.to_bytes(
            ((decrypted_signature_int.bit_length() + 7) // 8) + 1, byteorder='big')

        # if (public_key.has_private()):
        #    if (len(content) < public_key.size_in_bytes()):
        #        print("a")
        #        return False

        # check if the padding is starting with bytes according to PKCS #1 v1.5 Padding Scheme
        if (decrypted_signature_bytes[0] != 0 or decrypted_signature_bytes[1] != 2):
            return False

        # remove padding
        i = 2
        while i < public_key.size_in_bytes():
            if decrypted_signature_bytes[i] == 0:
                i += 1
                break
            i += 1
        if i == public_key.size_in_bytes():
            print("c")
            return False

        signature_content = decrypted_signature_bytes[i:]
        # print("decrypted padded aes int : " + str(signature_content))
        # print("cyphertext : " + str(content))
        return signature_content == content

    # credits to https://www.youtube.com/watch?v=bcBBEdpLhlg&t=240s&ab_channel=SoftwareSecurityandCryptography
    def decrypt_rsa(self, content, private_key, exponent, modulus):
        signature_int = int.from_bytes(content, byteorder='big')

        # use pow function to decrypt signature with RSA private keys exponent and modulus
        decrypted_signature_int = pow(
            signature_int, exponent, modulus)

        # convert to bytes so that the resulting size will be 256 bytes
        decrypted_signature_bytes = decrypted_signature_int.to_bytes(
            ((decrypted_signature_int.bit_length() + 7) // 8) + 1, byteorder='big')

        # check if the padding is starting with bytes according to PKCS #1 v1.5 Padding Scheme
        if (decrypted_signature_bytes[0] != 0 or decrypted_signature_bytes[1] != 2):
            return None

        # remove padding
        i = 2
        while i < private_key.size_in_bytes():
            if decrypted_signature_bytes[i] == 0:
                i += 1
                break
            i += 1
        if i == private_key.size_in_bytes():
            return None

        decrypted_content = decrypted_signature_bytes[i:]
        # extract last 16 bytes containing iv
        iv = decrypted_content[-16:]
        # extract everything except last 16 containing aes key
        aes_key = decrypted_content[:-16]
        return aes_key, iv

    def rsa_encrypt_aes_key(self, aes_key, public_key):
        aes_size = len(aes_key)
        # 3  bytes for reserved hexa numbers
        padding_len = public_key.size_in_bytes() - aes_size - 3

        # following the PKCS #1 v1.5 Padding Scheme
        padding_bytes = b"\x00\x02" + \
            (self.generate_octet_string_except_zero(padding_len)) + b"\x00"

        padding_aes = padding_bytes + aes_key
        print("c:AES_key_IV_padding=" + str(padding_aes) + "\n")

        # sign padded key with private key
        padded_aes_int = int.from_bytes(padding_aes, byteorder='big')
        signature_int = pow(padded_aes_int, public_key.e, public_key.n)
        signature_bytes = signature_int.to_bytes(
            (signature_int.bit_length() + 7) // 8, byteorder='big')
        # print("padded aes int : " + str(padding_aes))
        return signature_bytes

    def symmetric_encryption(self, message, aes_key, iv):

        # padding the message so that it alignes exactly to blocks of 16 bytes
        padded_message = message + b"\x00" + \
            self.generate_octet_string_except_zero(
                (16 - (len(message) + 2) % 16)) + b"\x00"

        # encrypt the message with AES using CBC mode
        cipher = AES.new(aes_key, AES.MODE_CBC, iv)
        ciphertext = cipher.encrypt(padded_message)

        print("c:AES_cipher=" + str(ciphertext) + "\n")
        return ciphertext

    def symmetric_decryption(self, message, aes_key, iv):

        # decrypt the message with AES in CBC mode
        cipher = AES.new(aes_key, AES.MODE_CBC, iv)
        cipher_aes = cipher.decrypt(message)

        # remove padding from the back
        if (cipher_aes[-1] != 0):
            return None

        i = -2
        while i < len(cipher_aes):
            if cipher_aes[i] == 0:
                break
            i -= 1
        aes_key = cipher_aes[:i]

        return aes_key

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
        publicKey = None
        if os.path.exists(key_folder + owner + 'publicKey.pem'):
            with open(key_folder + owner + 'publicKey.pem', 'rb') as f:
                publicKey = RSA.import_key(f.read())
        else:
            print("Error: opening the reciever public key!\n")
            exit(1)

        return publicKey


class MyClient:
    def __init__(self, port=8080, ip_addr='localhost'):
        self.c_util = CryptoUtils()
        self.RSA_pubkey, self.RSA_privkey = self.c_util.generate_or_load_RSA_keypair(
            "RSA_client")
        self.RSA_pubkey_reciever = self.c_util.load_RSA_public_key(
            "RSA_server")
        self.aes_key = self.c_util.generate_octet_string(AES_key_size//8)
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
                    print(f"c:Successfully connected server")
                    print(
                        f"c:RSA_public_key_sender= {self.RSA_pubkey.export_key('DER')}\n")
                    print(
                        f"c:RSA_private_key_sender= {self.RSA_privkey.export_key('DER')}\n")
                    print(
                        f"RSA_public_key_receiver= {self.RSA_pubkey_reciever.export_key('DER')}\n")
                    break
                except:
                    print(
                        "ERROR: Failed to establish conection. Gonna sleep briefly & try again")
                    time.sleep(10)
                    continue
            while True:

                # get a message
                message = input("c:Enter input: ")
                self.generate_new_aes()
                # hash it and create signature
                signature = self.c_util.rsa_sign_md5_with_padding(
                    message, self.RSA_privkey)

                # create a random iv for the session
                package = signature + message.encode('utf8')
                session_iv = self.c_util.generate_octet_string(16)

                # generate symmetric key and encrypt the message with signaure using clients private RSA key
                print("c:AES_key=" + str(self.aes_key) + "\n")
                cyphertext = self.c_util.symmetric_encryption(
                    package, self.aes_key, session_iv)

                # encrypt iv and aes key with public key of receiver
                encrypted_aesKey_iv = self.c_util.rsa_encrypt_aes_key(
                    self.aes_key + session_iv, self.RSA_pubkey_reciever)
                print("c:RSA_AES_key_IV=" + str(encrypted_aesKey_iv) + "\n")
                package2 = encrypted_aesKey_iv + cyphertext
                print("c:ciphertext=" + str(package2) + "\n")
                self.socket.send(package2)

        except KeyboardInterrupt:
            print("Client is being stopped.")
            self.socket.close()

    def generate_new_aes(self):
        self.aes_key = self.c_util.generate_octet_string(AES_key_size//8)


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

                print(f"s:Client has joined\n")
                print(
                    f"s:RSA_public_key_receiver= {self.RSA_pubkey.export_key('DER')}\n")
                print(
                    f"s:RSA_private_key_receiver= {self.RSA_privkey.export_key('DER')}\n")
                print(
                    f"s:RSA_public_key_sender= {self.RSA_pubkey_sender.export_key('DER')}\n")
                try:
                    while True:
                        # Receive the message from the client
                        response = client_socket.recv(1024)
                        print(f"s:ciphertext= {str(response)}\n")
                        cipher_aes_iv = response[:256]
                        ciphertext = response[256:]
                        print(f"s:RSA_AES_key_IV= {str(cipher_aes_iv)}\n")
                        print(f"s:AES_cipher= {str(ciphertext)}\n")

                        aes_key, session_iv = self.c_util.decrypt_rsa(
                            cipher_aes_iv, self.RSA_privkey, self.RSA_privkey.d, self.RSA_privkey.n)
                        print(f"s:AES_key= {str(aes_key)}\n")

                        decrypted_cyphertext = self.c_util.symmetric_decryption(
                            ciphertext, aes_key, session_iv)
                        print(f"s:text_hash= {str(decrypted_cyphertext)}\n")

                        message = decrypted_cyphertext[256:]
                        print(f"s:plaintext= {str(message)}\n")

                        signature = decrypted_cyphertext[:256]
                        content = self.c_util.generate_MD5(message)
                        print(f"s:MD5= {str(content)}\n")
                        if (self.c_util.verify_signature(content, signature, self.RSA_pubkey_sender)):
                            print(
                                "The integrity of the message has not been compromised.\n")
                        else:
                            print(
                                "The integrity of the report has been compromised.\n")
#
                        # number = int.from_bytes(signature, byteorder='big')
                        # print(
                        #    f"Signature: {number}")
                        # print(
                        #    f"Received message from: {message.decode('utf8')}")
                except Exception as e:
                    print(
                        f'Error processing message from {client_address}: {e}\n')

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
