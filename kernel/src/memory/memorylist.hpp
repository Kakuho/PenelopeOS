#ifndef MEMORYLIST_HPP
#define MEMORYLIST_HPP
#include <cstdint>
#include <array>
#include "limine/requests.hpp"
#include "limine/limine.h"
#include "kostream.hpp"
#include "memory.hpp"

// this file details physical memory manegement, which is organised as an 
// linked list of memory blocks. 
namespace memory::list{
  struct memoryBlock{
    memoryBlock* next;
    std::uint64_t length; 
  };

  struct memoryBlock32{
    std::uint32_t next;
    std::uint32_t length; 
  };

  // just a helper struct to keep account of our memory mapping entries
  struct memoryEntry{
    paddr64_t base;
    std::uint64_t length;
  };

  // intialise the headers of every free memory blocks, and set our head
  void InitialiseHeaders(); 

  void printEntries();

  memoryBlock* getHead();

  void setHead(memoryBlock* mptr);
} // namespace memory::list

// this should be our point of contact with the memory system. 
// it should delegate operations to memory::list
namespace memory::pmm{
  void* allocHeap(std::size_t nbytes);

  vaddr64_t allocStack(std::size_t nbytes);

  void free(void* base);

  void printEntries();

  inline void* malloc(std::size_t nbytes){return allocHeap(nbytes);}

  inline std::size_t blockSize(){ return sizeof(list::memoryBlock); }

  inline std::size_t blockSize32(){ return sizeof(list::memoryBlock32); }

  inline list::memoryBlock* head() {return list::getHead();}

  inline void printEntries(){list::printEntries();}

  std::size_t round(std::size_t src, std::uint8_t multiple);
} // namespace memory::pmm

#endif
