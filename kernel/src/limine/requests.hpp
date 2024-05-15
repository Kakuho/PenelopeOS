#ifndef SERVICES_HPP
#define SERVICES_HPP

#include "limine.h"
#include "kostream.hpp"

namespace limine{

// Set the base revision to 1, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info

inline bool is_limine_supported(){
  // checks to see if the bootloader supports our standard
  LIMINE_BASE_REVISION(2);
  if(LIMINE_BASE_REVISION_SUPPORTED == false){
    kout << "LIMINE BASE 2 IS NOT SUPPORTED, ASSSUMING BASE REVISION 0" << '\n';
    return false;
  }
  else{
    return true;
  }
};

  namespace requests{

  extern volatile limine_framebuffer_request framebuffer_request;

  extern volatile limine_memmap_request memorymap_request;

  extern volatile limine_hhdm_request hhdm_request;

  extern volatile limine_kernel_address_request kernel_addr_req;

  } // namespace limine::requests

} // namespace limine

#endif
