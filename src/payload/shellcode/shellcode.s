# -----------------------------------------------------------------------------
#  shellcode.s  —  x86‑64 Linux (Intel syntax)
# -----------------------------------------------------------------------------
#  Entry‑register contract (set up by the C code before jumping here):
#      RDI = to_shell[0]      (stdin for /bin/sh)
#      R8  = from_shell[1]    (stdout/err for /bin/sh)
#      RDX = to_shell[1]      (unused FD, must close)
#      RCX = from_shell[0]    (unused FD, must close)
#
#  Assemble   :  as --64 shellcode.s -o shellcode.o
#  Link       :  ld -N -o shellcode.elf shellcode.o    # -N gives RX PT_LOAD
#  Raw binary :  objcopy -O binary shellcode.elf shellcode.raw
# -----------------------------------------------------------------------------

.intel_syntax noprefix
.section .text
.global _start

_start:
    # --- dup2(to_shell[0], 0) ----------------------------------------------
    mov     rsi, 0              # newfd = STDIN
    mov     eax, 33             # sys_dup2
    syscall

    # --- dup2(from_shell[1], 1) -------------------------------------------
    mov     rdi, r8             # oldfd = from_shell[1]
    mov     rsi, 1              # newfd = STDOUT
    mov     eax, 33             # sys_dup2
    syscall

    # --- dup2(from_shell[1], 2) -------------------------------------------
    mov     rdi, r8             # oldfd = from_shell[1]
    mov     rsi, 2              # newfd = STDERR
    mov     eax, 33
    syscall

    # --- close(to_shell[1]) ----------------------------------------------
    mov     rdi, rdx            # fd to close
    mov     eax, 3              # sys_close
    syscall

    # --- close(from_shell[0]) --------------------------------------------
    mov     rdi, rcx            # fd to close
    mov     eax, 3
    syscall

    # --- execve("/bin/sh", ["/bin/sh"], NULL) ---------------------------
    xor     rdx, rdx            # envp = NULL
    movabs  rbx, 0x0068732f6e69622f  # "/bin/sh\0" little‑endian
    push    rbx                 # push string onto the stack
    mov     rdi, rsp            # filename = rsp
    push    rdx                 # NULL terminator for argv
    push    rdi                 # argv[0] = filename
    mov     rsi, rsp            # argv = rsp
    mov     eax, 59             # sys_execve
    syscall                     # no return if successful

    # --- exit(1) if execve failed ----------------------------------------
    mov     rdi, 1              # status = 1
    mov     eax, 60             # sys_exit
    syscall