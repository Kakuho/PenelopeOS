#include "idt.hpp"
#include "memory/memory.hpp"

// some cute interrupt handler :D
extern "C" void sayHi(int x){
  if(x != 0){
    kout << "why this no work" << '\n';
  }
  if(x == 0){
    kout << "this working now" << '\n';
  }
  kout << "the interrupt works now bros" << '\n';
}

namespace idt{
  idtGate constructIdtGate(void(*handler_address)()){
    // similiar to call gates
    mem::vaddr64_t vaddr = reinterpret_cast<mem::vaddr64_t>(handler_address);
    std::uint16_t vaddr0 = vaddr & (0xFFFF);
    std::uint16_t vaddr1 = (vaddr & (0xFFFFull << 0x10)) >> 0x10 ;
    std::uint32_t vaddr2 = (vaddr & (0xFFFF'FFFFul << 0x20)) >> 0x20;
    kout  << "vaddr: "  << vaddr << '\n'
          << "vaddr0: " << vaddr0 << '\n'
          << "vaddr1: " << vaddr1 << '\n'
          << "vaddr2: " << vaddr2 << '\n';
    return idtGate{
      .offset0 = vaddr0,
      .segment_selector = 0x08,  // we only want to select the kernel code segment
      .p_dpl_gatetype_ist = 0b1000'1110'0000'0000,
      .offset1 = vaddr1,
      .offset2 = vaddr2,
      .rsv = 0
    };
  }

  void setIdtEntry(std::uint8_t index, void(*handler)()){
    idt_table[index] = constructIdtGate(handler);
  }

  void initialiseIdt(){
    // set the descriptor
    idtor.limit = sizeof(idt_table);
    idtor.offset = reinterpret_cast<mem::vaddr64_t>(&idt_table);
    // set the table
    kout << intmode::hex << reinterpret_cast<mem::vaddr64_t>(&isr_0) << '\n';
    for (std::uint8_t i = 0; i < 32; i++){
        setIdtEntry(i, &isr_0);
    }
    kout << "idt descriptor location: " 
         << intmode::hex << reinterpret_cast<std::uint64_t>(&idtor) << '\n';
    load_idt(&idtor);
    enable_interrupts();
    kout << "IDT Initialised" << '\n';
  }

  // global variable
  idtGate idt_table[256];
  idtDescriptor idtor;
}
