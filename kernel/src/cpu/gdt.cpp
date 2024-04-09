#include "gdt.hpp"
#include "memory/memory.hpp"
#include <cstdint>

namespace gdt{

  SegmentDescriptor constructSegtor(void* address, std::uint16_t limit, 
      std::uint8_t accessbyte, std::uint8_t flagslimit){
    // note: in long mode address and limit is ignored for stack segment, 
    // code segment
    mem::vaddr64_t vaddr = reinterpret_cast<mem::vaddr64_t>(address);
    std::uint16_t vaddr0 = vaddr & (0xFFFF << 0x10);
    std::uint8_t vaddr1 = vaddr & (0xFFul << 0x20);
    std::uint8_t vaddr2 = vaddr & (0xFFul << 0x38);
    return SegmentDescriptor{
      .limit0 = limit,
      .base0 = vaddr0,
      .base1 = vaddr1,
      .accessbyte = accessbyte,
      .flags_limit = flagslimit,
      .base2 = vaddr2
    };
  }

  bool initialisegdt(GdtTable& _gdt){

  }
}
