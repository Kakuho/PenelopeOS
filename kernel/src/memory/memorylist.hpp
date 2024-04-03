#ifndef MEMORYLIST_HPP
#define MEMORYLIST_HPP
#include "memory.hpp"
#include <cstdint>
#include <limine_services.hpp>
#include <limine.h>
#include <array>

namespace mem{

  // memory management is organised as a linked list to memory blocks
  // memoryBlock is a single node
  struct memoryBlock{
    memoryBlock* next;
    std::uint64_t length; 
  };

  struct memoryBlock32{
    std::uint32_t next;
    std::uint32_t length; 
  };

  struct memoryEntry{
    mem::paddr64_t base;
    std::uint64_t length;
  };

  class memoryList{
    // n.b the free memory list, stores virtual addresses
    memoryBlock m_head;
    public:
      memoryList();
      void printEntries() const;
      memoryBlock* head() {return m_head.next;}
      memoryBlock* head() const {return m_head.next;}
      void setHead(memoryBlock* mptr) { m_head.next = mptr;}
  };

  class memoryManager{
    memoryList m_freelist;
    public:
      void printEntries() const;
      void* allocHeap(std::size_t nbytes);
      mem::vaddr64_t allocStack(std::size_t nbytes);
      void* malloc(std::size_t nbytes){return allocHeap(nbytes);}
      void free(void* base);
      std::size_t blockSize() const{ return sizeof(memoryBlock); }
      std::size_t blockSize32() const{ return sizeof(memoryBlock32); }
      memoryBlock* head() {return m_freelist.head();}
  };

  std::size_t round(std::size_t src, std::uint8_t multiple);

  extern memoryManager pmm;

} // namespace mem

#endif
