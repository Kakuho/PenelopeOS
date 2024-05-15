// this file contains replacement functions for the usual allocation and 
// deallocation functions.

#include <cstdint>
#include "memorylist.hpp"

// I won't be using exceptions to build this os
[[nodiscard]] void* operator new(std::size_t n) noexcept{
  void* ptr = memory::pmm::allocHeap(n);
  return ptr;
}

void operator delete(void* vaddr) noexcept{
  memory::pmm::free(vaddr);
  return;
}

void operator delete(void* vaddr, std::size_t size) noexcept{
  memory::pmm::free(vaddr);
  kout << size << '\n';
  return;
}
