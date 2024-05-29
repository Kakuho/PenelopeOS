#ifndef PSTRUCT_HPP
#define PSTRUCT_HPP

// TODO:  1) Functions to create Paging Structures
//        2) Functions to install / uninstall address spaces

#include <cstdint>
#include "memory.hpp"
#include "cpu/features.hpp"
#include "cpu/util.hpp"
#include "kostream.hpp"

extern "C" std::uint64_t load_paging_table();

namespace memory::paging{

  constexpr std::uint16_t structureBytes = 4096;  // intel 4.2
  constexpr std::uint16_t structureSize = 512;    // intel 4.2
  
  // All paging entries have the same structure, so this struct acts as an 
  // universal type.

  class PagingEntryDescriptor;

  using P4eDescriptor   = PagingEntryDescriptor;
  using PdpteDescriptor = PagingEntryDescriptor;
  using PdeDescriptor   = PagingEntryDescriptor;
  using PteDescriptor   = PagingEntryDescriptor;

  // definition of the universal paging entry type
  
  class PagingEntryDescriptor{
    public:

    PagingEntryDescriptor(paddr64_t paddr): m_entry{paddr}{}
    PagingEntryDescriptor& operator=(const PagingEntryDescriptor& src);

    // fields
    bool Present() const{ return m_entry & 1 ? true : false; }
    bool Writeable() const{ return m_entry & 0b10 ? true : false; }
    paddr64_t address() const{ return (m_entry & (0xFFF'FFFF << 12)); }

    // implicit conversions
    operator std::uint64_t() const{ return m_entry; }

    private:

    std::uint64_t m_entry;
  };

  inline void ParsePageTableCommon(paddr64_t ptaddr){
    // universal function to parse a page table.
    for(std::uint16_t j = 0; j < structureSize; j++){
      const PagingEntryDescriptor* const pe = 
        reinterpret_cast<P4eDescriptor*>(ptaddr + 8*j);
      kout << "pml4e entry :: " << j 
           << " :: value :: " << static_cast<std::uint64_t>(*pe) << '\n';
      if(pe->Present()){
        kout << "present" << '\n';
      }
      if(pe->Writeable()){
        kout << "writeable" << '\n';
      }
    }
  }


  // PM4L Related

  paddr64_t GetCurrentPM4L();

  inline void ParsePM4L(paddr64_t pm4laddr){
    for(std::uint16_t j = 0; j < structureSize; j++){
      const P4eDescriptor* const pml4e = 
        reinterpret_cast<P4eDescriptor*>(pm4laddr + 8*j);
      kout << "pml4e entry :: " << j 
           << " :: value :: " << static_cast<std::uint64_t>(*pml4e) << '\n';
      if(pml4e->Present()){
        kout << "present" << '\n';
      }
      if(pml4e->Writeable()){
        kout << "writeable" << '\n';
      }
    }
  }


  inline void DeletePM4LE(std::uint16_t entry){
    P4eDescriptor* const pml4e = 
      reinterpret_cast<P4eDescriptor*>(GetCurrentPM4L() + 8*entry);
    *pml4e =  0;
  }

  inline void SetPM4LE(std::uint16_t entry, P4eDescriptor pdptr){
    P4eDescriptor* const pml4e = 
      reinterpret_cast<P4eDescriptor*>(GetCurrentPM4L() + 8*entry);
    *pml4e = pdptr;
  }


  // PDPT Related

  inline void ParsePDPT(paddr64_t pdptaddr){
    for(std::uint16_t j = 0; j < structureSize; j++){
      const PdpteDescriptor* const pdpte = 
        reinterpret_cast<PdpteDescriptor*>(pdptaddr + 8*j);
      kout << "pdpte entry :: " << j 
           << " :: value :: " << static_cast<std::uint64_t>(*pdpte) << '\n';
      if(pdpte->Present()){
        kout << "present" << '\n';
      }
      if(pdpte->Writeable()){
        kout << "writeable" << '\n';
      }
    }
  }


  // Page Directory Related

  inline void ParsePD(paddr64_t pdaddr){
    for(std::uint16_t j = 0; j < structureSize; j++){
      PdeDescriptor* pde = reinterpret_cast<PdeDescriptor*>(pdaddr + 8*j);
      kout << "pde entry :: " << j 
           << " :: value :: " << static_cast<std::uint64_t>(*pde) << '\n';
      if(pde->Present()){
        kout << "present" << '\n';
      }
      if(pde->Writeable()){
        kout << "writeable" << '\n';
      }
    }
  }

  // Page Table Related

  inline void ParsePT(paddr64_t ptaddr){
    for(std::uint16_t j = 0; j < structureSize; j++){
      PteDescriptor* pte = reinterpret_cast<PteDescriptor*>(ptaddr + 8*j);
      kout << "pte entry :: " << j 
           << " :: value :: " << static_cast<std::uint64_t>(*pte) << '\n';
      if(pte->Present()){
        kout << "present" << '\n';
      }
      if(pte->Writeable()){
        kout << "writeable" << '\n';
      }

    }
  }

  // Page frame Related

  inline void ParsePage(paddr64_t paddr){
    for(std::uint16_t j = 0; j < 0x1000; j++){
      char* pch = reinterpret_cast<char*>(paddr + j);
      kout << *pch << '\n';
    }
  }

  inline void ParseBootLoaderTable(){
    paddr64_t bootpm4 = GetCurrentPM4L();
    kout << "cr3 = " << load_paging_table() << '\n';
    kout << "address of boot paging table: " << bootpm4 << '\n';
    for(std::uint16_t i = 0; i < structureSize; i++){
      P4eDescriptor* pm4le = reinterpret_cast<P4eDescriptor*>(bootpm4 + 8*i);
      kout << "pm4l entry :: " << i 
           << " :: value :: " << static_cast<std::uint64_t>(*pm4le)
           << " :: address :: " << bootpm4 + 8*i << '\n';
      if(pm4le -> Present()){
        kout << "PM4LE IS PRESENT, TRAVERSING PDPT PAGE TABLE ... " << "\n\n";
        paddr64_t pdpt = pm4le->address();
        for(std::uint16_t j = 0; j < structureSize; j++){
          PdpteDescriptor* pdpte = reinterpret_cast<PdpteDescriptor*>(pdpt + 8*j);
          kout << "pdpte entry :: " << j 
               << " :: value :: " << static_cast<std::uint64_t>(*pdpte) << '\n';
          if(pdpte -> Present()){
            kout << "PDPTE IS PRESENT, TRAVERSING PD PAGE TABLE ... " << "\n\n";
            paddr64_t pd = pdpte->address();
            for(std::uint16_t j = 0; j < structureSize; j++){
              PdeDescriptor* pde = reinterpret_cast<PdeDescriptor*>(pd + 8*j);
              kout << "PD entry :: " << j 
                   << " :: value :: " << static_cast<std::uint64_t>(*pde) << '\n';
              if(pde -> Present()){
                kout << "PDE IS PRESENT, TRAVERSING PT PAGE TABLE ... " << "\n\n";
                paddr64_t pt = pde->address();
                for(std::uint16_t j = 0; j < structureSize; j++){
                  PteDescriptor* pte = reinterpret_cast<PteDescriptor*>(pt + 8*j);
                  kout << "Page Table entry :: " << j 
                        << " :: value :: " << static_cast<std::uint64_t>(*pte) 
                        << '\n';
                  kout << "alpaca" << '\n';
                  if(pte->Present()){
                    kout << "there is a present page at address :: " 
                         << pte->address() << '\n';
                  }
                }
                kout << "finished parsing PDE's Page Table" << "\n\n";
              }
            }
            kout << "Finished parsing PDPTE's PD" << "\n\n";
          }
        }
        kout << "Finished parsing PM4LE's PDPT" << "\n\n";
      }
      //if(i != 0) x8664::haltAndCatchFire();
    }
    kout << "Finished parsing PM4L" << '\n';
  }

} // namespace memory::paging

#endif
