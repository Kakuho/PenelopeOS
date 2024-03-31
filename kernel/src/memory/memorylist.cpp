#include "memorylist.hpp"
#include "memory.hpp"

namespace mem{
  memoryList::memoryList(){
    limine_memmap_entry** entries = req::memorymap_request.response->entries;
    std::uint64_t mem_entries = req::memorymap_request.response->entry_count;
    std::uint64_t hhdm_offset = req::hhdm_request.response->offset;
    // preallocate a contiguous block of memory for the useable 
    // blocks
    std::array<memoryEntry, 20> buffer;
    std::size_t index = 0;
    // populate the index
    for(std::size_t j = 0; j < mem_entries; j++){
      if(entries[j]->type == LIMINE_MEMMAP_USABLE){
        buffer[index++] = memoryEntry{
          .base = entries[j]->base, 
          .length = entries[j]->length
        };
      }
    }
    // construct the linked list
    for(int i = 0; i < index; i++){
      // physical addresses of the available memories
      mem::paddr64_t base = buffer[i].base;
      mem::paddr64_t nextBase = buffer[i+1].base;
      // populate list nodes - note we place the block structures 
      // at the start of each available memories.
      memoryBlock* pmblk = reinterpret_cast<memoryBlock*>(base);
      pmblk->length = buffer[i].length;
      if(i == index-1){
        pmblk->next = nullptr;
      }
      else{
        pmblk->next = reinterpret_cast<memoryBlock*>(nextBase);
      }
    }
    // remember to place the start
    m_head.next = reinterpret_cast<memoryBlock*>(buffer[0].base);
  }

  void memoryList::printEntries() const{
    memoryBlock* mptr = m_head.next;
    while(mptr != nullptr){
      kout << intmode::hex 
           << "address :: " << reinterpret_cast<paddr64_t>(mptr) 
           << " :: length :: " << mptr->length 
           << " :: number of pages :: " << mptr->length / 0x1000
           << '\n';
      mptr = mptr->next;
    }
  }

  void memoryManager::printEntries() const{
    m_freelist.printEntries();
  }

} // namespace mem
