#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

using namespace std;

class RingBuffer
{
public:
    explicit RingBuffer(uint32_t size = 1024*1024);
    ~RingBuffer();
    uint32_t put(const unsigned char *buffer, uint32_t len);
    uint32_t get(unsigned char *buffer, uint32_t len);
    uint32_t getSize();
    uint32_t clear();

protected:
    unsigned char *pointerOffset(unsigned char *pointer, uint32_t offset);

protected:
    pthread_mutex_t m_buffMutex;
    uint32_t m_buffSize;
    unsigned char *m_ringBuffer;
    unsigned char *m_writePointer;
    unsigned char *m_readPointer;

};

#endif // RINGBUFFER_H
