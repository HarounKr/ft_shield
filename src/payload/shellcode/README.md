# Génération et Chiffrement d’un Shellcode

## Étapes

### 1. Assembler le fichier source

```bash
as --64 shellcode.s -o shellcode.o
```

### 2. Lier l’objet en un exécutable ELF brut

```bash
ld -N -o shellcode.elf shellcode.o
```

### 3. Convertir l’exécutable ELF en fichier binaire brut
```bash
objcopy -O binary shellcode.elf shellcode.raw
```

### 4. Chiffrer le shellcode avec AES-128-CBC

```bash
openssl enc -aes-128-cbc -nosalt -e -in shellcode.raw -out shellcode.enc -K '2b7e151628aed2a6abf7158809cf4f3c' -iv '000102030405060708090a0b0c0d0e0f'
```
Chiffrement symétrique avec **AES-128** en mode **CBC** :

- `-nosalt` désactive l'ajout de sel.
- `-K` est la **clé hexadécimale** de 128 bits.
- `-iv` est le **vecteur d'initialisation** de 128 bits.

### 5. Convertir le fichier chiffré en tableau C

```bash
xxd -i shellcode.enc shellcode_hex.h
```