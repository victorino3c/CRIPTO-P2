from Crypto.Cipher import DES3
from binascii import unhexlify

# Clave en formato hexadecimal
hex_key = "133457799BBCDFF112233445566778899AABBCCDDEEFF001"
key = unhexlify(hex_key)
key = DES3.adjust_key_parity(key)

# Texto a cifrar en formato hexadecimal (8 bytes exactos)
plaintext = bytes.fromhex("164a047363ea99c0")

# Cifrado en modo ECB (sin IV ni padding)
cipher = DES3.new(key, DES3.MODE_ECB)
ciphertext = cipher.encrypt(plaintext)

# Descifrado
decipher = DES3.new(key, DES3.MODE_ECB)
decrypted_text = decipher.decrypt(ciphertext)

# Salida
print(f"Clave (hex): {hex_key}")
print(f"Texto original (hex):   {plaintext.hex()}")
print(f"Texto cifrado (hex):    {ciphertext.hex()}")
print(f"Texto descifrado (hex): {decrypted_text.hex()}")
