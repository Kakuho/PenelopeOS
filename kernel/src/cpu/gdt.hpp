#ifndef GDT_HPP
#define GDT_HPP
#include <cstdint>
#include <memory/memory.hpp>
#include <kostream.hpp>

extern "C" void load_gdt(void* address);
extern "C" void load_tsr(std::uint16_t index);
extern "C" void reload_segments();

namespace gdt{

  // the os shall operate in a long / flat mode. This is 
  // protected flat mode 3.2.2 in the intel manual
  // N.B: limine puts us into long mode (referred to as 64-bit mode in the intel 
  // docs), thus my data structures below assumes the 64-bit mode.

  struct __attribute__((packed)) GdtDescriptor{
    std::uint16_t limit;        // size of gdt - 1
    std::uint64_t offset;       // linear address of the GDT - long mode
  };

  // https://wiki.osdev.org/Global_Descriptor_Table - segment descriptor
  // intel docs vol3a 3.4.5
  struct __attribute__((packed)) SegmentDescriptor{
    std::uint16_t limit0;
    std::uint16_t base0;
    std::uint8_t  base1;
    std::uint8_t  accessbyte;  // accessbyte is a bit packed byte:
                            // composed of type, s, dpl, p
    std::uint8_t  flags_limit; // flag and limit are packed together
                            // flags composed of avl, l, d/b and g
    std::uint8_t  base2;
  };

  SegmentDescriptor constructSegtor(
      void* base_address, std::uint16_t limit, 
      std::uint8_t accessbyte, std::uint8_t flagslimit);

  // https://wiki.osdev.org/Task_State_Segment long mode
  // intel manual vol3a 8.7 - tss in 64bit mode
  struct __attribute__((packed)) TSS{
    std::uint32_t reserved0;
    // RSPs - used to store stack pointers.
    std::uint64_t rsp0;
    std::uint64_t rsp1;
    std::uint64_t rsp3;
    // dead zone
    std::uint64_t reserved1;
    // interrupt stack tables
    std::uint64_t ist1;
    std::uint64_t ist2;
    std::uint64_t ist3;
    std::uint64_t ist4;
    std::uint64_t ist5;
    std::uint64_t ist6;
    std::uint64_t ist7;
    // dead zone
    std::uint64_t reserved3;
    std::uint8_t  reserved4;
    // IO Map Base Address
    std::uint8_t iopbm;
  };

  // intel vol3a8.2.3 tss descriptor in 64bit mode
  struct __attribute__((packed)) TSSDescriptor{
    std::uint16_t limit0;
    std::uint16_t base0;
    std::uint8_t  base1;
    std::uint8_t  accessbyte;  // same byte packed as the segment descriptor
    std::uint8_t  flags_limit; // same byte packed as the segment descriptor, 
    std::uint8_t  base2;
    // second entry of tss's gdt entry
    std::uint32_t base3;
    std::uint8_t reserved0;
    std::uint8_t res1_zeros;
    std::uint16_t res2;
  };

  TSSDescriptor constructTsstor(void* base_address, std::uint32_t limit);

  struct __attribute__((packed)) GdtTable{
    // I treat the whole linear space.
    SegmentDescriptor nullseg;
    SegmentDescriptor kernel_code;
    SegmentDescriptor kernel_data;
    SegmentDescriptor user_code;
    SegmentDescriptor user_data;
    TSSDescriptor tss;
  };

  extern std::uint16_t tablesize;

  void setupGdt(GdtTable& _gdt);
  bool initialiseGDT();

  extern TSS tss;
  extern GdtTable table;
  extern GdtDescriptor descriptor;
} // namespace gdt

#endif
