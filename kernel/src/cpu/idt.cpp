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

extern "C" void generalIRQ(int x){
  kout << "AN INTERRUPT HAS OCCURED" << '\n'
       << "interrupt code: " << x << '\n'
       << "END OF HANDLING" << '\n';
}

extern "C" void contextSwitchISR(void* stackaddr){
  std::uint64_t* saddr = reinterpret_cast<std::uint64_t*>(stackaddr);
  // above the stack pointer
  //std::uint64_t* prsp = reinterpret_cast<std::uint64_t*>(saddr + 1);
  std::uint64_t* prdi = reinterpret_cast<std::uint64_t*>(saddr);
  // below the stack pointer
  std::uint64_t* prax = reinterpret_cast<std::uint64_t*>(saddr - 1);
  std::uint64_t* prcx = reinterpret_cast<std::uint64_t*>(saddr - 2);
  std::uint64_t* prdx = reinterpret_cast<std::uint64_t*>(saddr - 3);
  std::uint64_t* prbx = reinterpret_cast<std::uint64_t*>(saddr - 4);
  std::uint64_t* prbp = reinterpret_cast<std::uint64_t*>(saddr - 5);
  std::uint64_t* prsi = reinterpret_cast<std::uint64_t*>(saddr - 6);
  std::uint64_t* prflags = reinterpret_cast<std::uint64_t*>(saddr - 7);
  kout << "AN INTERRUPT HAS OCCURED" << '\n'
       << reinterpret_cast<memory::vaddr64_t>(saddr) << '\n'
       << "rax = " << *prax << '\n'
       << "rbx = " << *prbx << '\n'
       << "rcx = " << *prcx << '\n'
       << "rdx = " << *prdx << '\n'
       << "rsi = " << *prsi << '\n'
       << "rdi = " << *prdi << '\n'
       << "rbp = " << *prbp << '\n'
       //<< "rsp = " << *prsp << '\n'
       << "rflags = " << *prflags << '\n'
       << "we tryna to ctxt sw" << '\n';
  kout << "END OF HANDLING" << '\n';
}

namespace x8664::idt{
  idtGate constructIdtGate(void(*handler_address)()){
    // similiar to call gates
    memory::vaddr64_t vaddr = reinterpret_cast<memory::vaddr64_t>(handler_address);
    std::uint16_t vaddr0 = vaddr & (0xFFFF);
    std::uint16_t vaddr1 = (vaddr & (0xFFFFull << 0x10)) >> 0x10 ;
    std::uint32_t vaddr2 = (vaddr & (0xFFFF'FFFFul << 0x20)) >> 0x20;
    kout  << "Constructing IDT GATE: " << '\n'
          << "vaddr: "  << vaddr << '\n'
          << "vaddr0: " << vaddr0 << '\n'
          << "vaddr1: " << vaddr1 << '\n'
          << "vaddr2: " << vaddr2 << '\n';
    return idtGate{
      .offset0 = vaddr0,
      .segment_selector = 0b0101'000,  
            // we only want to select the kernel code segment -> check limine protocol.md
      .p_dpl_gatetype_ist = 0b1'00'0'1110'00000'000,
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
    idtor.limit = sizeof(idt_table)-1;
    idtor.offset = reinterpret_cast<memory::vaddr64_t>(&idt_table);
    // set the table
    kout << intmode::hex << reinterpret_cast<memory::vaddr64_t>(&isr_0) << '\n';
    for (std::uint8_t i = 0; i < 34; i++){
      setIdtEntry(i, reinterpret_cast<void(*)()>(isr[i]));
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
