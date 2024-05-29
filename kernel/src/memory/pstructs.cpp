#include "pstructs.hpp"

namespace memory::paging{
  PagingEntryDescriptor& PagingEntryDescriptor::operator=(
      const PagingEntryDescriptor& src
  ){
    m_entry = src.m_entry;
    return *this;
  }

  paddr64_t GetCurrentPM4L(){
    std::uint64_t currentCr3 = load_paging_table();
    paddr64_t pm4laddr = (currentCr3 >> 12) << 12;
    return pm4laddr;
  }

}
