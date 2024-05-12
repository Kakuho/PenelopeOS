#ifndef INTERRUPT_HPP
#define INTERRUPT_HPP
#include <cstdint>
#include <memory/memory.hpp>

extern "C" {
  void dosti();
  void dointo();
  void clear_interrupts();
  void invokeinterrupt1();
  void invokeinterrupt2();
  void invokeinterrupt3();
  void invokeinterrupt4();
  void invokeinterrupt5();
  void invokeinterrupt6();
  void invokeinterrupt33();
}

namespace inter{
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
