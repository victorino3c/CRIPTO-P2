from Crypto.Cipher import DES

# Definir texto plano y clave
plaintext = bytes.fromhex("c2bd324f9e0a7706")
iv = bytes.fromhex("0232f2f10b980837")
key = bytes.fromhex("d543d6ea8c64d3b9")

# Crear un objeto DES en modo ECB
cipher1 = DES.new(key, DES.MODE_CBC, iv)
cipher2 = DES.new(key, DES.MODE_CBC, iv)

# Cifrar el texto plano
ciphertext = cipher1.encrypt(plaintext)

local = cipher2.decrypt(ciphertext)

print("Texto cifrado:", ciphertext.hex())
print("Texto descifrado:", local.hex())
