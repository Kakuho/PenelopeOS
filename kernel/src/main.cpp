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
#include "cpu/gdt.hpp"
#include "cpu/interrupts.hpp"
#include "cpu/features.hpp"
#include "cpu/idt.hpp"
#include "cpu/util.hpp"
#include "proc/queuetable.hpp"
#include "disk/ataio.hpp"
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
  //x8664::gdt::initialiseGDT();
  x8664::idt::initialiseIdt();
  memory::printMemoryMap();
  memory::list::InitialiseHeaders();
  memory::list::printEntries();

  //---------------------------------------------------------------//

  x8664::interrupts::invokeInterrupt(0);

  // done now, hang around a bit :D
  bootmessage();
  x8664::haltAndCatchFire();
}
