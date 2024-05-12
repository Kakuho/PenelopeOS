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

  void* memoryManager::allocHeap(std::size_t nbytes){
    // walk the memory list to find the earliest suitable block size
    // return the virtual address of the allocated chunk
    if(nbytes <= 0){
      return reinterpret_cast<void*>(-1);
    }
    // we need to allocate a multiple of memoryBlock
    if(nbytes < 16){
      // allocate 32 bytes at minimum to account for padding
      nbytes = 16 + nbytes;
    }
    nbytes = round(nbytes, 16);
    // define placeholders to walk the memory list
    memoryBlock* mptr = m_freelist.head();
    memoryBlock* next = mptr->next;
    memoryBlock* prev = nullptr;
    std::size_t index = 0;
    while(mptr != nullptr){
      if(mptr->length > nbytes){
        // we need to reduce the size of the chunk
        std::uint64_t length = mptr->length;
        std::uint64_t allocsize = nbytes;
        mem::vaddr64_t allocAddr = reinterpret_cast<mem::vaddr64_t>(mptr);
        kout << allocAddr << '\n';
        //kout << reinterpret_cast<vaddr64_t>(mptr) << '\n';
        //kout << reinterpret_cast<vaddr64_t>(allocated) << '\n';
        mptr = reinterpret_cast<memoryBlock*>(
            reinterpret_cast<vaddr64_t>(mptr) + allocsize);
        // we shall define a chunk of allocated memory to include both the next free block, 
        // and the length of the allocated chunk
        memoryBlock* allocated = reinterpret_cast<memoryBlock*>(allocAddr);
        allocated->next = mptr;  // note this should not be used!
        allocated->length = allocsize;
        kout << reinterpret_cast<vaddr64_t>(mptr) << '\n';
        // now we perform memory chunk book keeping
        if(prev != nullptr){
          prev->next = mptr; // bruh
        }
        mptr->next = next;
        mptr->length = length - allocsize;
        if(index == 0){
          m_freelist.setHead(mptr);
        }
        // +1 is required to move to the required heap memory
        return reinterpret_cast<void*>(allocated+1); 
      }
      kout << "ayo!" << '\n';
      // the current block is too small
      prev = mptr;
      mptr = mptr->next;
      next = mptr->next;
      index++;
    }
    return reinterpret_cast<void*>(-1);
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
    // walk the memory list
    while(next != nullptr){
      prev = mptr;
      mptr = mptr->next;
      next = mptr->next;
    }
    // next is nullptr, so mptr points to the last entry
    // allocate from the top of available ram
    std::uint64_t allocsize = nbytes;
    memoryBlock* allocated = reinterpret_cast<memoryBlock*>(
        reinterpret_cast<vaddr64_t>(mptr) - allocsize);
    // finish the bookkeeping, write data at the memory chunks
    allocated->next=nullptr;
    allocated->length = allocsize;
    // update mptr to point to the 
    mptr->length = mptr->length - allocsize;
    return reinterpret_cast<mem::vaddr64_t>(allocated+1);
  }

  void memoryManager::free(void* base){
    // free must be provided a base address of a allocated memory chunk
    // n.b free puts the next address and length in an allocated block
    // all cases:
    // chunk_* free         need to check if chunk_* is at the end of the memory map
    // free chunk_*         need to check if chunk_* is at the start of the memory map
    // chunk_* chunk        
    // chunk chunk_*
    memoryBlock* mptr = reinterpret_cast<memoryBlock*>(base);
    kout << "beginning address of memory: " << reinterpret_cast<mem::vaddr64_t>(mptr) << '\n'
         << "beginning addrsss of chunk: " << reinterpret_cast<mem::vaddr64_t>(mptr - 1) << '\n';
    // set the mptr to contain the memory chunk's header information 
    mptr = mptr - 1;
    kout << "length :: " << mptr->length << " ::address:: " << reinterpret_cast<vaddr64_t>(mptr) 
         << '\n';
    kout << "head: " << reinterpret_cast<mem::vaddr64_t>(m_freelist.head()) << '\n';
    // easy case: bottom of heap, check with the next block if it is the head
    if( 
        (reinterpret_cast<vaddr64_t>(mptr) + mptr->length) == 
        reinterpret_cast<vaddr64_t>(m_freelist.head()) 
      ){

      // coalesce the head of freelist and the current block
      mptr->length = mptr->length + m_freelist.head()->length;
      mptr->next = m_freelist.head()->next;
      m_freelist.setHead(mptr);
      return;
    }
    // we need to find which two memory blocks on the free list, the current allocated memory
    // is between
    memoryBlock* current = m_freelist.head();
    memoryBlock* next = current->next;
    // the memory block is less than the first guy, and is adjacent to an
    // allocated memory block
    if(reinterpret_cast<mem::vaddr64_t>(mptr) < reinterpret_cast<mem::vaddr64_t>(current)){
      kout << "we here" << '\n';
      mptr->next = m_freelist.head();
      m_freelist.setHead(mptr);
      return;
    }
    // allocated memory block > memorylist.head, so we need to walk the memory list to 
    // determine which two memory blocks the allocated block is between.
    bool is_between = (current <= mptr) && (mptr <= next);
    while(!is_between){
      current = next;
      next = current->next;
      is_between = (current >= mptr) && (mptr <= next);
    }
    // current < memory_block < next
    kout << "ayo! " << reinterpret_cast<vaddr64_t>(mptr) << " is between: " 
         << reinterpret_cast<vaddr64_t>(current) << " and "
         << reinterpret_cast<vaddr64_t>(next) << '\n';
    // test to see if it is right next to free memory block next
    if( 
        (reinterpret_cast<vaddr64_t>(mptr) + mptr->length) == 
        reinterpret_cast<vaddr64_t>(next) 
      ){
      // coalesce mptr and next
      mptr->length = mptr->length + next->length;
      mptr->next = next->next;
      current->next = mptr;
      return;
    }
    else{
      // we part of the banana squad
      current->next = mptr;
      mptr->next = next;
      return;
    }
  }

  std::size_t round(std::size_t src, std::uint8_t multiple){
    return ((src + multiple/2) / multiple) * multiple;
  }

  
  memoryManager pmm{};
} // namespace mem
