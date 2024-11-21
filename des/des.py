from Crypto.Cipher import DES

# Definir texto plano y clave
plaintext = bytes.fromhex("FEDCBA9876543210")
key = bytes.fromhex("133457799BBCDFF1")

# Crear un objeto DES en modo ECB
cipher = DES.new(key, DES.MODE_ECB)

# Cifrar el texto plano
ciphertext = cipher.encrypt(plaintext)
print(ciphertext.hex()) 
