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
      memoryBlock* pmblk = reinterpret_cast<memoryBlock*>(mem::paddrToVaddr(base));
      pmblk->length = buffer[i].length;
      if(i == index-1){
        pmblk->next = nullptr;
      }
      else{
        pmblk->next = reinterpret_cast<memoryBlock*>(mem::paddrToVaddr(nextBase));
      }
    }
    // remember to place the start
    m_head.next = 
      reinterpret_cast<memoryBlock*>(mem::paddrToVaddr(buffer[0].base));
  }

  void memoryList::printEntries() const{
    memoryBlock* mptr = m_head.next;
    std::size_t index = 0;
    while(mptr != nullptr){
      kout << intmode::hex 
           << "index :: " << index++
           << " :: address :: " << reinterpret_cast<paddr64_t>(mptr) 
           << " :: length :: " << mptr->length 
           << " :: number of pages :: " << mptr->length / 0x1000
           << '\n';
      mptr = mptr->next;
    }
  }

  void memoryManager::printEntries() const{
    m_freelist.printEntries();
  }

  // omg i made this wrong :3
  // there should be a fkn hole mann
  mem::vaddr64_t memoryManager::allocHeap(std::size_t nbytes){
    // walk the memory list to find the earliest suitable block size
    // return the virtual address of the allocated chunk
    if(nbytes <= 0){
      return -1;
    }
    // we need to allocate a multiple of memoryBlock
    if(nbytes < 16){
      // allocate 32 bytes at minimum to account for padding
      nbytes = 32 + nbytes;
    }
    // define placeholders because this is a singly linked list
    memoryBlock* mptr = m_freelist.head();
    memoryBlock* next = mptr->next;
    memoryBlock* prev = nullptr;
    while(mptr != nullptr){
      if(mptr->length > nbytes){
        // construct a new entry to the memory list
        std::uint64_t length = mptr->length;
        //kout << reinterpret_cast<vaddr64_t>(mptr) << '\n';
        // placeholder for our allocated chunk
        memoryBlock* allocated = mptr;
        std::uint64_t allocsize = nbytes;
        //kout << reinterpret_cast<vaddr64_t>(allocated) << '\n';
        mptr = reinterpret_cast<memoryBlock*>(
            reinterpret_cast<vaddr64_t>(mptr) + allocsize);
        //kout << reinterpret_cast<vaddr64_t>(mptr) << '\n';
        // now we perform memory chunk book keeping
        if(prev != nullptr){
          prev->next = allocated; // bruh
        }
        allocated->next = mptr;
        allocated->length = allocsize;
        mptr->next = next;
        mptr->length = length - allocsize;
        return reinterpret_cast<mem::vaddr64_t>(
            reinterpret_cast<std::uint64_t>(allocated) + sizeof(memoryBlock));
      }
      kout << "ayo!" << '\n';
      // the current block is too small
      prev = mptr;
      mptr = mptr->next;
      next = mptr->next;
    }
    return -1;
  }

  mem::vaddr64_t memoryManager::allocStack(std::size_t nbytes){
    // walk the memory list to find the latest suitable block size. stack 
    // should be allocated from the top of the address space.
    // return the virtual address of the allocated chunk
    if(nbytes <= 0){
      return -1;
    }
    // we need to allocate a multiple of memoryBlock
    if(nbytes < 16){
      // allocate 32 extra bytes at minimum to account for padding
      nbytes = 32 + nbytes;
    }
    // define placeholders because this is a singly linked list
    memoryBlock* mptr = m_freelist.head();
    memoryBlock* next = mptr->next;
    memoryBlock* prev = nullptr;
    while(next != nullptr){
      // walk the memory list
      prev = mptr;
      mptr = mptr->next;
      next = mptr->next;
    }
    kout << "we here?" << '\n';
    // next is nullptr, so mptr points to the last entry
    // allocate from the top of available ram
    std::uint64_t allocsize = nbytes;
    memoryBlock* allocated = reinterpret_cast<memoryBlock*>(
        reinterpret_cast<vaddr64_t>(mptr) - allocsize);
    // finish the bookkeeping, write data at the memory chunks
    allocated->next = mptr;
    allocated->length = allocsize;
    mptr->length = mptr->length - allocsize;
    prev->next = allocated;
    return reinterpret_cast<mem::vaddr64_t>(allocated);
  }

  void memoryManager::free(void* base){
    // free must be provided a base address of a allocated memory chunk
    memoryBlock* mptr = reinterpret_cast<memoryBlock*>(base);
    kout << "length :: " << mptr->length 
         << " :: next :: " << reinterpret_cast<mem::vaddr64_t>(mptr->next)<< '\n';

    // we still need to walk the memory list, but this time just to find an entry
    memoryBlock* current = m_freelist.head();
    memoryBlock* next = current->next;
    memoryBlock* prev = nullptr;
    while(current != mptr){
      prev = current;
      current = next;
      next = current->next;
    }
    // we found u 
    kout << "hehe found you :: length :: " << current->length 
         << " :: next :: " << reinterpret_cast<mem::vaddr64_t>(current->next)<< '\n';
    // first chunk of memory
    if(prev == nullptr){
      // we coalesce the first two memories
      current->length = current->length + next->length;
      current->next = next->next;
    }
    // last chunk of memory
    else if(next == nullptr){
      // we coalesce the last two memories == prev and current
      prev->length = prev->length + current->length;
      prev->next = nullptr;
    }
    else{
      // we coalesce current and next
      current->length = current->length + next->length;
      current->next = next->next;
    } 
  }
  
  memoryManager pmm{};
} // namespace mem

