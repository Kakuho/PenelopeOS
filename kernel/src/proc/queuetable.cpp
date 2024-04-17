#include "queuetable.hpp"

namespace proc::queue{

  pid_t queueTable::popFirst(qid_t queue){
    if(isEmpty(queue)){
      return -1;
    }
    else{
      pid_t evicted = this->firstPid(queue);
      _buf[this->head(queue)].next = _buf[evicted].next;
      return evicted;
    }
  }

  pid_t queueTable::popLast(qid_t queue){
    if(isEmpty(queue)){
      return -1;
    }
    else{
      pid_t evicted = this->firstPid(queue);
      _buf[this->head(queue)].next = _buf[evicted].next;
      return evicted;
    }
  }

  pid_t queueTable::pushQueue(qid_t queue, pid_t proc){
    // TODO: check for table overflow
    pid_t oldlast = _buf[tail(queue)].prev;
    // update oldlast
    _buf[oldlast].next = proc;
    // update the process entry in the array
    _buf[proc].prev = oldlast;
    _buf[proc].next = tail(queue);
    // update the tail
    _buf[tail(queue)].prev = proc;
    return proc;
  }

  pid_t queueTable::popQueue(qid_t queue){
    // removes the first element of the queue
    if(isEmpty(queue)){
      return -1;
    }
    else{
      pid_t evicted = this->firstPid(queue);
      // update the head
      _buf[head(queue)].next = _buf[evicted].next;
      // update evicted's next
      _buf[_buf[evicted].next].prev = head(queue);
      // update the evicted
      _buf[evicted].next = -1;
      _buf[evicted].prev = -1;
      return evicted;
    }
  }

  bool queueTable::insert(pid_t proc, qid_t queue, std::uint32_t key){
    if(proc < 0){
      return false;
    }
    // we walk the queue
    pid_t curr = head(queue);
    pid_t prev = -1;
    // our priorities are ordered in ascending order, termination is guaranteed
    // as the key of the tail is -1
    while(key <= _buf[_buf[curr].next].key){
      curr = _buf[curr].next;
    }
    // we've found the immediate next position
    prev = _buf[curr].prev;
    // update the previous entry
    _buf[prev].next = proc;
    // insert and update the entry
    _buf[proc].prev = prev;
    _buf[proc].next = curr;
    // update the next entry
    _buf[curr].prev = proc;
    return true;
  }

  qid_t queueTable::newQueue(){
    qid_t newqueue = nextqid;
    // TODO: check for queue table overflow
    nextqid += 2;
    // update the head
    _buf[newqueue].next = tail(newqueue);
    _buf[newqueue].prev = -1;
    // update the tail
    _buf[tail(newqueue)].next = -1;
    _buf[tail(newqueue)].key = - 1;
    _buf[tail(newqueue)].prev = head(newqueue);
    return newqueue;
  }

  void queueTable::printQueue(qid_t q){
    // sanitary check for valid queueid
    if(q < 0){
      return;
    }
    if(isEmpty(q)){
      kout << "queue with head " << head(q) << " and tail " << tail(q)
           << " is empty" << '\n';
    }
    // we walk the queue
    pid_t curr = head(q);
    // our priorities are ordered in ascending order, termination is guaranteed
    // as the key of the tail is -1
    while(_buf[curr].next != -1){
      kout << " " << curr << ' ';
      curr = _buf[curr].next;
    }
    kout << " " << curr << ' ';
  }

  queueTable _queueTable{};
};
