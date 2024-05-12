#include "proctable.hpp"

namespace proc{
  processTable::processTable(): last{0}{
    for(processInfo& pi: _buf){
      pi.processState = state::free;
    }
  }

  pid_t processTable::newpid(){
    pid_t newpid = last;
    for(auto i = 0u; i < NPROC; i++){
      if(_buf[newpid].processState == state::free){
        return newpid;
      }
      else{
        newpid = (newpid + 1) % NPROC;
      }
    }
    // we couldn't find a free entry
    return -1;
  }

  pid_t processTable::create(void(*func)()){
    // process' in xinu only executes a function, and 
    // thus only need a stack. all processes share the same address space
    pid_t allocated = newpid();
    if(allocated == -1){  // ABORT return -1; }
    // * allocate stack space
    }

    // * fill in stack as if it is a function just calle
    // * fills in process table entry
    // * returns pid of the newly allocated process
    return 0;
  }

} // namespace proc
