from Crypto.Cipher import DES

# Definir texto plano y clave
plaintext = bytes.fromhex("105371917B55818E")
iv = bytes.fromhex("D48001C68F2325A1")
key = bytes.fromhex("B586F3326E842C7A")

# Crear un objeto DES en modo ECB
# cipher = DES.new(key, DES.MODE_ECB)
cipher = DES.new(key, DES.MODE_CBC, iv)

# Cifrar el texto plano
ciphertext = cipher.encrypt(plaintext)
print(ciphertext.hex()) 
