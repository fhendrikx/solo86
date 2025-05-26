#ifndef RINGBUF_H
#define RINGBUF_H

#include <circle/spinlock.h>

class CRingBuf {
public:

    CRingBuf(int nSize);
    ~CRingBuf();

/*
  Add() will overwrite old data when the buffer is full.
  This means the add is always successful.
  Add() will return the number of bytes written without discarding old data.

  AddSafe() will wait for free space in the buffer before adding the data.
  AddSafe() will Lock and Unlock the buffer as necessary.
  If the Scheduler is active AddSafe() will sleep for 1ms between attempts to write
  to the buffer. The delay is to stop the routine hogging the buffer lock and
  allow other threads to run.

  Remove() returns the number of elements removed from the buffer.

  To minimise latency the lock is only held for long enough to add or remove one
  element at a time.

  If there are are multiple threads calling Add() or AddSafe() data may be interleved.
*/

    int Add(u8 c);
    int Add(u8 *c, int nLen);
    void AddSafe(u8 c);
    void AddSafe(u8 *c, int nLen);
    int Remove(u8 *c);
    int Remove(u8 *c, int nSize);
    int GetCount();
    int GetFree();
    
private:

    CSpinLock m_Lock;
    u8 *m_pBuffer;
    volatile int m_nSize;
    volatile int m_nReadPos;
    volatile int m_nWritePos;
    volatile int m_nCount;

};

#endif
