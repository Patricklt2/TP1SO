#ifndef TP1SO_QUEUEFILE_H_
#define TP1SO_QUEUEFILE_H_

typedef struct FileQueue* Fqueue;

Fqueue newQueue();

void dequeue(Fqueue queue,char* buff);
void enqueue(Fqueue queue, const char* path);
int isempty(Fqueue queue);
#endif
