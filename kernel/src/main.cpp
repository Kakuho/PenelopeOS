#include <cstdint>
#include <array>
#include <cstddef>
#include <stdio.h>
#include <string.h>
#include <cassert>
#include <limine.h>
#include <logger.hpp>
#include <cpu/gdt.hpp>
#include <memory/memory.hpp>
#include <memory/memorylist.hpp>
#include <kostream.hpp>
#include <limine_services.hpp>
#include <cpu/interrupts.hpp>
#include <cpu/features.hpp>
#include <cpu/idt.hpp>
#include "proc/queuetable.hpp"
#include "disk/ataio.hpp"
#include "./misc/ascii.hpp"

extern "C" void outb(int, char val);
extern "C" unsigned int memory_low();

const int port = 0x3F8;
logger serialLogger{port};

// Halt and catch fire function.
namespace {

void hcf() {
  asm ("cli");
  for (;;) {
      asm ("hlt");
  }
}

}

// The following two stubs are required by the Itanium C++ ABI (the one we use,
// regardless of the "Itanium" nomenclature).
// Like the memory functions above, these stubs can be moved to a different .cpp file,
// but should never be removed.
extern "C" {
  int __cxa_atexit(void (*)(void *), void *, void *) { return 0; }
  void __cxa_pure_virtual() { hcf(); }
}

void printKernelAddress(){
  mem::vaddr64_t kerneladdr = mem::getKernelVirtualAddress();
  kout << "kernel vaddr::" << kerneladdr << ":: kernel paddr :: " << mem::vaddrToPaddr(kerneladdr) << '\n';
}


void* operator new(std::size_t n){
  return mem::pmm.allocHeap(n);
}

void operator delete(void* vaddr) noexcept{
  mem::pmm.free(vaddr);
}

void operator delete(void* vaddr, std::size_t size) noexcept{
  mem::pmm.free(vaddr);
}

void playMemory(){
  mem::memoryManager mm{};
  mm.printEntries();
  int* one = reinterpret_cast<int*>(mm.allocHeap(5));
  mm.printEntries();
  int* two = reinterpret_cast<int*>(mm.allocHeap(128));
  mm.printEntries();
  mm.free(two);
  mm.printEntries();
  kout << "----\n";
  int* three = reinterpret_cast<int*>(mm.allocStack(32));
  mm.printEntries();
}

void playfree(){
  // mem1 
  // mem2 
  // mem3
  mem::pmm.printEntries();
  char* pch1 = reinterpret_cast<char*>(mem::pmm.allocHeap(10));
  mem::pmm.printEntries();
  char* pch2 = reinterpret_cast<char*>(mem::pmm.allocHeap(10));
  mem::pmm.printEntries();
  char* pch3 = reinterpret_cast<char*>(mem::pmm.allocHeap(10));
  mem::pmm.printEntries();
  char* pch4 = reinterpret_cast<char*>(mem::pmm.allocHeap(10));
  mem::pmm.printEntries();
  // pch1
  // pch2
  // pch3
  // pch4
  // mem1
  // mem2
  // mem3
  mem::pmm.free(pch3);
  // pch1
  // pch2
  // mem1
  // pch4
  // mem2
  // mem3
  // mem4
  mem::pmm.printEntries();
  mem::pmm.free(pch1);
  // mem1
  // pch2
  // mem2
  // pch4
  // mem3
  // mem4
  // mem5
  mem::pmm.printEntries();
}

class A{
  int a;
  int b;
  long c;
};

void playfree2(){
  // mem1 
  // mem2 
  // mem3
  mem::pmm.printEntries();
  A* pch1 = new A;
  mem::pmm.printEntries();
  A* pch2 = new A;
  mem::pmm.printEntries();
  A* pch3 = new A;
  mem::pmm.printEntries();
  A* pch4 = new A;
  mem::pmm.printEntries();
  // pch1
  // pch2
  // pch3
  // pch4
  // mem1
  // mem2
  // mem3
  delete pch3;
  // pch1
  // pch2
  // mem1
  // pch4
  // mem2
  // mem3
  // mem4
  mem::pmm.printEntries();
  delete pch1;
  // mem1
  // pch2
  // mem2
  // pch4
  // mem3
  // mem4
  // mem5
  mem::pmm.printEntries();
}

void startup(){
  PrintTwixie();
  kout << "Welcome to PenelopeOS! I hope you enjoy your stay! :D\n";
}


// Extern declarations for global constructors array.
extern void (*__init_array[])();
extern void (*__init_array_end[])();

// The following will be our kernel's entry point.
// If renaming _start() to something else, make sure to change the
// linker script accordingly.
extern "C" void _start() {
  if(req::is_limine_supported()){
    hcf();
  }

  // Call global constructors.
  // global ctors are just ctors for global objects
  for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
      __init_array[i]();
  }

  // Ensure we got a framebuffer.
  if (req::framebuffer_request.response == nullptr
   || req::framebuffer_request.response->framebuffer_count < 1) {
      hcf();
  }

  // Fetch the first framebuffer.
  limine_framebuffer *framebuffer = req::framebuffer_request.response->framebuffers[0];

  // Note: we assume the framebuffer model is RGB with 32-bit pixels.
  for (std::size_t i = 0; i < 100; i++) {
      volatile std::uint32_t *fb_ptr = static_cast<volatile std::uint32_t *>(framebuffer->address);
      //fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
      //fb_ptr[i * (framebuffer->pitch / 4)] = 0xff;
      fb_ptr[i] = 0xff0000;
      //kout << static_cast<std::uint32_t>(fb_ptr[i]);
  }

  //gdt::initialiseGDT();
  idt::initialiseIdt();

  /*
  kout << pmmaddr << '\n';

  features::probecr3();
  kout << intmode::hex << features::getPML4() << '\n';
  features::probePhysicalWidth();
  features::probecr4();
  features::probecr0();
  */

  //features::disablePaging();
  //features::probecr0();
  //mem::printMemoryMap();
  /*
  mem::paddr64_t paddr = 0x4e000;
  char* pchar = reinterpret_cast<char*>(mem::paddrToVaddr(paddr));
  *pchar = 'C';
  //kout << *pchar << '\n';
  */

  //playMemory();
  //playfree();
  
  mem::printMemoryMap();
  /*
  using proc::queue::_queueTable;
  proc::queue::qid_t newq = _queueTable.newQueue();
  _queueTable.pushQueue(newq, 3);
  _queueTable.printQueue(newq);
  */

  /*
  clear_interrupts();
  invokeinterrupt5();
  invokeinterrupt33();
  */
  startup();
  //Disk::InitialiseAtaPio(0, 0);
  // done now, hang around a bit :D
  hcf();
}
