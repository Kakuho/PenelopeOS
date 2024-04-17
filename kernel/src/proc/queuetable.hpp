#ifndef QUEUE_TABLE_HPP
#define QUEUE_TABLE_HPP

#include <cstdint>
#include <kostream.hpp>

// TODO: i)  convert the class into a class template?

namespace proc::queue{

const std::size_t NPROC = 10;

using qid_t = std::uint16_t;    // queues are referenced by id
using pid_t = std::int32_t;    // processes are referenced by id

struct node{
  // note this node respects xinu's queue list and thus is composed of 
  // a key, and indexes to other elements in the underlying buffer of the list
  std::uint32_t key;
  pid_t next;
  pid_t prev;
};

class queueTable{
  // this is the implementation of the compact doubly linked list used for 
  // xinu's queue table. It stores process ids, which are 1:1 with processes
  // N.B: this is a global table which stores multiple queues.
  // The following system constraint must hold:
  //  At any one time, a process can only be in one single queue at a time.
  using nodeType = node;
  public:
    // xinu specified functions
    qid_t head(qid_t q)          { return q; }
    qid_t tail(qid_t q)          { return q + 1; }
    bool isEmpty(qid_t q)         { return _buf[head(q)].next == tail(q); }
    bool isNotEmpty(qid_t q)        { return !isEmpty(q); }
    pid_t firstPid(qid_t q)  { return _buf[head(q)].next; }
    pid_t lastPid(qid_t q)   { return _buf[tail(q)].prev; }
    std::uint32_t firstKey(qid_t q) { return _buf[firstPid(q)].key; }
    std::uint32_t lastKey(qid_t q)  { return _buf[lastPid(q)].key; }
    pid_t popFirst(qid_t queue);
    pid_t popLast(qid_t queue);
    // queue operations
    pid_t pushQueue(qid_t queue, pid_t proc);
    pid_t popQueue(qid_t queue);
    bool insert(pid_t pid, qid_t queue, std::uint32_t key); // for priority queue
    qid_t newQueue();
    // print options
    void printQueue(qid_t q);
  private:
    std::uint16_t nextqid{NPROC}; 
    std::size_t maxSlots;
    nodeType _buf[NPROC + 4]; // magic constant 4 - means we can only hold 2 queues
                              // the implementation is an array so we can fit the 
                              // buffer within cachelines
};

extern queueTable _queueTable;

} // namespace proc::queue

#endif
