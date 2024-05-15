#ifndef MEMORY_HPP
#define MEMORY_HPP
#include <cstdint>
#include <kostream.hpp>
#include "limine/limine.h"
#include "limine/requests.hpp"

namespace limine::requests{
  extern volatile limine_hhdm_request hhdm_request;
  extern volatile limine_memmap_request memorymap_request;
  extern volatile limine_kernel_address_request kernel_addr_req;
} // namespace limine::requests

namespace memory{
  // a bunch of miscellenaeous memory
  using paddr64_t = std::uint64_t;
  using vaddr64_t = std::uint64_t;

  inline paddr64_t vaddrToPaddr(vaddr64_t vaddr){
    return vaddr - limine::requests::hhdm_request.response->offset;
  }

  inline vaddr64_t paddrToVaddr(paddr64_t paddr){
    return paddr + limine::requests::hhdm_request.response->offset;
  }

  void printMemoryMap();

  vaddr64_t getKernelVirtualAddress();

  inline void printKernelAddress(){
    vaddr64_t kerneladdr = getKernelVirtualAddress();
    kout << "kernel vaddr::" << kerneladdr << ":: kernel paddr :: " 
         << vaddrToPaddr(kerneladdr) << '\n';
  }

  void probeMemory(std::size_t index);

  std::uint64_t calculateAvailableMemory();

  void printPageFrames();

  void upperLimitProbe();

  void lowerLimitProbe();

  /* DO NOT USE */
  void corruptMemory();
  /* DO NOT USE */

} // namespace memory

#endif
