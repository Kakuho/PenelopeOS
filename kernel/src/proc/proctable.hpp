#ifndef PROC_TABLE_HPP
#define PROC_TABLE_HPP
#include <cstdint>
#include <array>

namespace proc{
  using pid_t = std::int32_t;
  enum class state{
    free,             // proc table entry is unused
    current,          // currently executing process
    ready,            // on the ready queue
    recv,             // waiting for a message
    sleep,            // is sleeping
    susp,             // is suspended
    wait,             // on a semaphore queue
    rectimer          // recieving with timeout
  };
  
  const std::uint8_t processNameLength{16};
  const std::uint32_t NPROC{32};
  const pid_t nullproc{0};

  struct processInfo{
    state processState;
    std::uint16_t priority;
    void* pstackPointer;
    void* pstackBase;
    std::uint32_t stackLength;
    char name[processNameLength];
    std::uint32_t semaphore;
    pid_t parent;
  };

  class processTable{
    public:
      // can i make this private, but have a friend construct it?
      processTable();
      pid_t create(void(*func)());
    private:
      // hidden from user
      pid_t newpid();
      std::array<processInfo, NPROC> _buf;
      pid_t last;
  };

  extern processTable _proctable;
} // namespace proc

#endif
