from Crypto.Cipher import DES

# Definir texto plano y clave
plaintext = bytes.fromhex("9d67a0d7fa892abd")
iv = bytes.fromhex("d48001c68f2325a1")
key = bytes.fromhex("12345F789AB23456")

# Crear un objeto DES en modo ECB
# cipher = DES.new(key, DES.MODE_ECB)
cipher1 = DES.new(key, DES.MODE_CBC, iv)
cipher2 = DES.new(key, DES.MODE_CBC, iv)

# Cifrar el texto plano
ciphertext = cipher1.encrypt(plaintext)

local = cipher2.decrypt(ciphertext)

print("Texto cifrado:", ciphertext.hex())
print("Texto descifrado:", local.hex())
