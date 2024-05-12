.globl dosti
dosti:
  sti
  ret

.globl dointo
dointo:
  ret

.globl clear_interrupts
clear_interrupts:
  cli
  ret

.globl invokeinterrupt1
invokeinterrupt1:
  int $1
  ret

.globl invokeinterrupt2
invokeinterrupt2:
  int $2
  ret

.globl invokeinterrupt3
invokeinterrupt3:
  int $3
  ret

.globl invokeinterrupt4
invokeinterrupt4:
  int $4
  ret

.globl invokeinterrupt5
invokeinterrupt5:
  int $5
  ret

.globl invokeinterrupt6
invokeinterrupt6:
  int $6
  ret

.globl invokeinterrupt7
invokeinterrupt7:
  int $7
  ret


.globl invokeinterrupt33
invokeinterrupt33:
  int $33
  ret

.globl disableInterrupts
disableInterrupts:
  cli
  ret
