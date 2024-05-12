.globl outb
outb: 
  movl %edi, %edx
  movl %esi, %eax
  outb %al , %dx
  ret

.globl inb
.type inb, @function
inb:
  movl %edi, %edx
  inb %dx, %al
  ret
