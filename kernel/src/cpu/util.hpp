#ifndef UTIL_HPP
#define UTIL_HPP


namespace x8664{
  inline void haltAndCatchFire(){
    asm("cli");
    for(;;){
      asm("hlt");
    }
  }
}

#endif
