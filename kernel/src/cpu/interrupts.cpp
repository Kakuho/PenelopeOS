#include <cstdint>
#include "interrupts.hpp"

namespace x8664::interrupts{
  bool isDoTripleFault(){
    dosti();
    return false;  // we reach here only if the preceding instruction was good
  }

  void invokeInterrupt(std::uint8_t no){
    switch(no){
      case 1:
        invokeinterrupt1();
        break;
      case 5:
        invokeinterrupt5();
        break;
    }
  }

  void parseAddress(void* address){
    memory::vaddr64_t vaddr = reinterpret_cast<memory::vaddr64_t>(address);
    kout << vaddr << '\n';
    std::uint16_t offset0 = (vaddr & 0xFFFF); 
    std::uint16_t offset1 = (vaddr & 0xFFFF << 0x10) >> 0x10;
    std::uint32_t offset2 = (vaddr & 0xFFFFFFFFul << 0x20) >> 0x20;
    kout << intmode::hex << "bits 15...0 : " << offset0 << '\n';
    kout << intmode::hex << "bits 31...16: " << offset1 << '\n';
    kout << intmode::hex << "bits 63...32: " << offset2 << '\n';
  }

  interruptGate formInterruptGate(void* address, std::uint16_t segSelector){
    // fields of an interrupt gate
    // offset     - just a pointer to the idt handler
    // selector   - segment selector to a code seg in the gdt
    // ist        - do we even need a interrupt stack table?
    // dpl = 0b00 - only the kernel can access you
    // p = 1      - present flag, needs to be 1
    // type is dependent on whether interrupt gate or trap gate
    memory::vaddr64_t vaddr = reinterpret_cast<memory::vaddr64_t>(address);
    std::uint16_t offset0 = (vaddr & 0xFFFF); 
    std::uint16_t offset1 = (vaddr & 0xFFFF << 0x10) >> 0x10;
    std::uint32_t offset2 = (vaddr & 0xFFFFFFFFul << 0x20) >> 0x20;
    kout << vaddr << '\n';
    return interruptGate{.offset0 = offset0, 
                         .offset1 = offset1,
                         .offset2 = offset2
                        };
  } 

  // there can only be 256 entries.
  interruptGate idt[256];
}
