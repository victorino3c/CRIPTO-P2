from Crypto.Cipher import DES

# Definir texto plano y clave
plaintext = bytes.fromhex("0123456789ABCDEF")
iv = bytes.fromhex("0000000000000000")
key = bytes.fromhex("133457799BBCDFFD")

# Crear un objeto DES en modo ECB
cipher1 = DES.new(key, DES.MODE_CBC, iv)
cipher2 = DES.new(key, DES.MODE_CBC, iv)

# Cifrar el texto plano
ciphertext = cipher1.encrypt(plaintext)

local = cipher2.decrypt(ciphertext)

print("Texto cifrado:", ciphertext.hex())
print("Texto descifrado:", local.hex())
