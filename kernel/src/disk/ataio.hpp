#ifndef ATAIO_HPP
#define ATAIO_HPP

#include "kostream.hpp"

extern "C" void outb(int port, char val);
extern "C" std::uint8_t inb(int port);
extern "C" std::uint8_t dosomething();

namespace Disk{
  void InitialiseAtaPio(std::uint8_t drive, std::uint8_t bus);
}

#endif
