#include <cstdint>
#include "memory/memory.hpp"
#include "kostream.hpp"

extern "C" void load_idt(void* idtor_address);
extern "C" void enable_interrupts();
extern "C" void isr_0();
extern "C" void sayHi(int x);
extern "C" void(*isr[])();
extern "C" void(*isr[])();

namespace x8664::idt{
  // https://wiki.osdev.org/Interrupt_Descriptor_Table
  // intel manual vol3a ??
  struct __attribute__((packed)) idtDescriptor{
    std::uint16_t limit;
    std::uint64_t offset;
  };

  struct __attribute__((packed)) idtGate{
    std::uint16_t offset0;
    std::uint16_t segment_selector;   // isnt this ignored in 64 bit mode??
    std::uint16_t p_dpl_gatetype_ist; // bit packed word
    std::uint16_t offset1;
    std::uint32_t offset2;
    std::uint32_t rsv;
  };

  idtGate constructIdtGate(void(*handler_address)());

  void setIdtEntry(std::uint8_t index, void(*handler)());

  void initialiseIdt();

  extern idtGate idt_table[256];
  extern idtDescriptor idtor;
}
