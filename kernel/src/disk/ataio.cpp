#include "ataio.hpp"
#include <cstdint>

void Disk::InitialiseAtaPio(std::uint8_t drive, std::uint8_t bus){
  if(bus == 0 && drive == 0){
    kout << "trying to initialise drive 0 on bus 0" << '\n';
  }
  else{
    return;
  }
  int driveSelectPort = 0x1f6;
  outb(driveSelectPort, 0xa0);
  for(int i = 0; i < 20; i++){
    inb(0x1f7);			/* wait 400ns for drive select to work */
  }
  // setting Sectorcount, LBAlo, LBAmid, and LBAhi IO ports to 0:
  for(int i = 0x1f2; i <= 0x1f5; i++){
    outb(i, 0);
  }
  // sending identify to command port
  outb(0x1f7, 0xec);
  // now we read
  std::uint8_t output = 0;
  output = inb(0x1f7);
  if(output == 0){
    kout << "no good :(" << '\n';
  }
  else{
    kout << "we good :D" << '\n';
  }
  // now we need to poll status port until bit 7 clears
  std::uint8_t status = inb(0x1f7);
  while(status & 0x80){
    status = inb(0x1f7);
  }
  kout << "finished polling!" << '\n';
}
