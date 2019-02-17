#include "RingBuffer.h"

RingBuffer::RingBuffer(uint32_t size)
{
    m_buffSize = size;
    m_ringBuffer = new unsigned char[size]();
    m_writePointer = m_ringBuffer;
    m_readPointer = m_ringBuffer;
    pthread_mutex_init(&m_buffMutex, NULL);
}

RingBuffer::~RingBuffer()
{
    delete[] m_ringBuffer;
    pthread_mutex_destroy(&m_buffMutex);
}

uint32_t RingBuffer::put(const unsigned char *buffer, uint32_t len)
{
    pthread_mutex_lock(&m_buffMutex);

    while ( len-- )
    {
        memcpy(m_writePointer, buffer, 1);
        m_writePointer = this->pointerOffset(m_writePointer, 1);
        buffer++;
    }

    pthread_mutex_unlock(&m_buffMutex);
    return len;
}

uint32_t RingBuffer::get(unsigned char *buffer, uint32_t len)
{
    pthread_mutex_lock(&m_buffMutex);

    int validLen = m_writePointer-m_readPointer;
    validLen = (validLen>=0) ? validLen : validLen+m_buffSize;
    uint32_t ret = len = min(len, (uint32_t)validLen);

    while ( len-- )
    {
        memcpy(buffer, m_readPointer, 1);
        m_readPointer = this->pointerOffset(m_readPointer, 1);
        buffer++;
    }

    pthread_mutex_unlock(&m_buffMutex);
    return ret;
}

uint32_t RingBuffer::getSize()
{
    return m_buffSize;
}

uint32_t RingBuffer::clear()
{
    pthread_mutex_lock(&m_buffMutex);

    m_writePointer = m_ringBuffer;
    m_readPointer = m_ringBuffer;

    pthread_mutex_unlock(&m_buffMutex);
    return 0;
}

unsigned char *RingBuffer::pointerOffset(unsigned char *pointer, uint32_t offset)
{
    unsigned char *pret = pointer;
    while ( offset-- )
    {
        if ( (uint32_t)((++pret) - m_ringBuffer) > m_buffSize )
        {
            pret = m_ringBuffer;
        }
    }

    return pret;
}
