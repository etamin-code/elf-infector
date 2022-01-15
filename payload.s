

.text
.globl _start
_start:
  #save the base pointer
  pushq %rbp
  pushq %rax
  pushq %rbx
  mov %rsp,%rbp
  


mov     $1, %rax                # system call 1 is write
mov     $1, %rdi                # file handle 1 is stdout
lea     message(%rip), %rsi     # address of string to output
mov     $13, %rdx               # number of bytes


       
  syscall
  

  movq $0, %rax
  movq $0, %rdi
  movq $0, %rdx

  pop %rbx
  pop %rax
  pop %rbp
  ret
  
 message:
        .ascii  "Hello, World\n"
