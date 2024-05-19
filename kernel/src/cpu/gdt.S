.globl load_gdt
load_gdt:
  LGDT (%rdi) // no offset, use rax as our base
  ret

.globl load_tsr
load_tsr:
  LTR %di
  ret

.globl reload_segments
reload_segments:
   // Reload CS register:
   // look at far return vol2.4 for how to change the cs register
   // note the values correspond to segment selectors, not index.
   PUSH $0x08                 
   LEA .reload_CS, %rax       // Load address of .reload_CS into RAX
   PUSH %rax                  // Push this value to the stack
      // stack is now -> &.reload_CS -> segtor for cs
   RETFQ                      // F to indicate far return

.reload_CS:
   // Reload data segment registers
   // documented in 3.4.4
   MOV   $0x10, %ax            
   MOV   %ax, %ds
   MOV   %ax, %es
   MOV   %ax, %fs
   MOV   %ax, %gs
   MOV   %ax, %ss
   RET
