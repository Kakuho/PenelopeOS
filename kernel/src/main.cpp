#include <cstdint>
#include <array>
#include <cstddef>
#include <stdio.h>
#include <string.h>
#include <cassert>
#include "kostream.hpp"
#include "limine/requests.hpp"
#include "limine/bb.hpp"
#include "memory/memory.hpp"
#include "memory/memorylist.hpp"
#include "memory/other.hpp"
#include "memory/pages.hpp"
#include "memory/pstructs.hpp"
#include "cpu/gdt.hpp"
#include "cpu/interrupts.hpp"
#include "cpu/features.hpp"
#include "cpu/idt.hpp"
#include "cpu/util.hpp"
#include "proc/queuetable.hpp"
#include "io/ataio.hpp"
#include "io/pci.hpp"
#include "misc/ascii.hpp"

// The following two stubs are required by the Itanium C++ ABI (the one we use,
// regardless of the "Itanium" nomenclature).
// Like the memory functions above, these stubs can be moved to a different .cpp 
// file, but should never be removed.
extern "C" {
  int __cxa_atexit(void (*)(void *), void *, void *) { return 0; }
  void __cxa_pure_virtual() { x8664::haltAndCatchFire(); }
}

void bootmessage(){
  PrintTwixie();
  kout << "Welcome to PenelopeOS! I hope you enjoy your stay!\n";
}

// Extern declarations for global constructors array.
extern void (*__init_array[])();
extern void (*__init_array_end[])();

extern "C" void _start() {
  if(limine::is_limine_supported()){
    x8664::haltAndCatchFire();
  }

  // Call global constructors.
  for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
      __init_array[i]();
  }

  limine::initialiseFramebuffer();
  x8664::gdt::initialiseGDT();
  x8664::idt::initialiseIdt();
  kout << "boot pages :: " << memory::GetBootPages() << '\n';
  memory::printMemoryMap();

  memory::CheckBootPages();
  memory::InitialisePageFrames();
  bootmessage();

  /*
  memory::list::InitialiseHeaders();
  memory::list::printEntries();
  */

  //---------------------------------------------------------------//

  memory::PrintSizeofPagetorMap();

  //memory::paging::ParseBootLoaderTable();

  x8664::features::probePhysicalWidth();

  kout << intmode::hex;
  
  //x8664::interrupts::invokeInterrupt(5);

  // FFFF800280000000 is a faulting address
  // FFFF800280000000 is a faulting address
  memory::ExtractPagingIndices(0xFFFF800280000000);
  // now we check why is it faulting by checking table entries
  /*
  kout << "----\n";
  memory::paging::ParsePDPT(0x7FF25000);
  kout << "----\n";
  memory::paging::ParsePD(0x7FF23000);
  kout << "----\n";
  memory::paging::ParsePT(0x7FC00000);
  */

  //memory::paging::ParsePage(0x53B0B3A0E031000);
  /*
  kout << intmode::hex;
  kout << "start of ram " << memory::startOfRam << '\n';
  memory::paddr64_t paddr = 0x4f901;
  kout << "physical memory location :: " << paddr << " :: page frame number :: " << memory::PhysicalToPageN(paddr) << '\n';
  */

  //memory::PrintPageFrames();

  kout << "attempting to read from pci device: " << io::pci::ReadDeviceHeader() << '\n';
  io::pci::EnumeratePCI();
  //Disk::InitialiseAtaPio(0, 0);

  // done now, hang around a bit :D
  kout << "\n\npony?" << '\n';
  x8664::haltAndCatchFire();
}
