#ifndef MOOSE_PARSER_THREAD_ARG_H
#define MOOSE_PARSER_THREAD_ARG_H

#define MOOSE_PARSER_QUEUE_INIT_NUM_ENTRIES 512

struct ParserArg {
  CkVec<int> queue_;
  pthread_t *parserThread_;
  pthread_mutex_t parserLock;

  ParserArg(pthread_t *parserThread) : 
    parserThread_(parserThread)
  {
    queue_.reserve(MOOSE_PARSER_QUEUE_INIT_NUM_ENTRIES);
    CkAssert(!pthread_mutex_init(&parserLock, NULL));
  }
};

#endif // MOOSE_PARSER_THREAD_ARG_H
