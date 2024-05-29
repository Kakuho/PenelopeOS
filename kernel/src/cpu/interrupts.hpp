#ifndef INTERRUPT_HPP
#define INTERRUPT_HPP
#include <cstdint>
#include "memory/memory.hpp"

extern "C" void dosti();
extern "C" void dointo();
extern "C" void clear_interrupts();
extern "C" void invokeinterrupt1();
extern "C" void invokeinterrupt2();
extern "C" void invokeinterrupt3();
extern "C" void invokeinterrupt4();
extern "C" void invokeinterrupt5();
extern "C" void invokeinterrupt6();
extern "C" void invokeinterrupt33();

namespace x8664::interrupts{
  bool isDoTripleFault();

  void invokeInterrupt(std::uint8_t no);

  void parseAddress(void* address);

  struct __attribute__((packed)) interruptGate{
    std::uint16_t offset0;
    std::uint16_t segSelector;
    std::uint16_t bitfield;
    std::uint16_t offset1;
    std::uint32_t offset2;
    std::uint32_t reserved;
  };

  struct idtr{
    std::uint16_t limit;
    std::uint64_t base;
  };

  interruptGate formInterruptGate(void* address, std::uint16_t segSelector);

  extern interruptGate idt[];
}

#endif
