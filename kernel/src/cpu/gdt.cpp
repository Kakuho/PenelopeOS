#include "gdt.hpp"
#include "memory/memory.hpp"
#include <cstdint>

namespace gdt{

  SegmentDescriptor constructSegtor(
      void* base_address, std::uint16_t limit, 
      std::uint8_t accessbyte, std::uint8_t flagslimit){
    // note: in long mode address and limit is ignored for stack segment, 
    // code segment
    // parse the virtual address
    mem::vaddr64_t vaddr = reinterpret_cast<mem::vaddr64_t>(base_address);
    std::uint16_t vaddr0 = vaddr & 0xFFFF;
    std::uint8_t vaddr1 = vaddr & (0xFFul << 0x10) >> 0x10;
    std::uint8_t vaddr2 = vaddr & (0xFFul << 0x18) >> 0x18;
    kout << "constructin segtor: " << '\n'
         << "vaddr: " << vaddr << '\n'
         << "vaddr0: " << vaddr0 << '\n'
         << "vaddr1: " << vaddr1 << '\n'
         << "vaddr2: " << vaddr2 << '\n';
    return SegmentDescriptor{
      .limit0 = limit,
      .base0 = vaddr0,
      .base1 = vaddr1,
      .accessbyte = accessbyte,       // 3.4.5 vol3a
      .flags_limit = flagslimit,      // 3.4.5 vol3a
      .base2 = vaddr2
    };
  }

  TSSDescriptor constructTsstor(void* base_address, std::uint32_t limit){
    // parse the vitrual address
    mem::vaddr64_t vaddr = reinterpret_cast<mem::vaddr64_t>(base_address);
    std::uint16_t vaddr0 = vaddr & 0xFFFF;
    std::uint8_t vaddr1 = (vaddr & (0xFFul << 0x10)) >> 0x10;
    std::uint8_t vaddr2 = (vaddr & (0xFFul << 0x18)) >> 0x18;
    std::uint32_t vaddr3 = (vaddr & (0xFFFFFFFFul << 0x20)) >> 0x20;
    kout << intmode::hex << "in constructTsstor" << '\n'
         << "vaddr: " << vaddr << '\n'
         << "vaddr0: " << vaddr0 << '\n'
         << "vaddr1: " << vaddr1 << '\n'
         << "vaddr2: " << vaddr2 << '\n'
         << "vaddr3: " << vaddr3 << '\n';
    // parse the limit
    std::uint16_t limit0 = limit & 0xFFFF;
    std::uint8_t flag_limit1 = (limit & (0xF << 0x10)) >> 0x10; // top byte is 0
    return TSSDescriptor{
      .limit0 = limit0,
      .base0 = vaddr0,
      .base1 = vaddr1,
      .accessbyte = 0b1000'1001,        // 8.2.2 vol3a
      .flags_limit = flag_limit1,       // 3.4.5 vol3a
      .base2 = vaddr2,
      .base3 = vaddr3,
      .reserved0 = 0,
      .res1_zeros = 0,
      .res2 = 0
    };
  }

  void setupGdt(GdtTable& _gdt){
    _gdt.nullseg     = constructSegtor(0, 0, 0, 0);             
                                                                // flaglimit
    _gdt.kernel_code = constructSegtor(0, 0xFFFF, 0b1001'1010, 0b1010'1111);
    _gdt.kernel_data = constructSegtor(0, 0xFFFF, 0b1001'0010, 0b1100'1111);
    _gdt.user_code   = constructSegtor(0, 0xFFFF, 0b1111'1010, 0b1010'1111);
    _gdt.user_data   = constructSegtor(0, 0xFFFF, 0b1111'0010, 0b1100'1111);
    _gdt.tss         = constructTsstor(&tss, sizeof(tss)-1);
  }

  bool initialiseGDT(){
    setupGdt(gdt::table);
    descriptor.limit = tablesize;
    descriptor.offset = reinterpret_cast<std::uint64_t>(&gdt::table); 
    load_gdt(&descriptor);
    load_tsr(0b00101'000);
    reload_segments();
    return true;
  }

  // global variables

  TSS tss{};
  GdtTable table{};
  std::uint16_t tablesize = sizeof(table) - 1;
  GdtDescriptor descriptor{};

}
