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

struct memoryEntry{
  mem::paddr64_t base;
  std::uint64_t length;
};

class memoryList{
  memoryBlock m_head;
  public:
    memoryList();
    void printEntries() const;
};

class memoryManager{
  memoryList m_freelist;
  public:
    void printEntries() const;
};

} // namespace mem

#endif
