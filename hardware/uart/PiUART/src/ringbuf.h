#ifndef RINGBUF_H
#define RINGBUF_H

#include <circle/spinlock.h>

class CRingBuf {
public:

    CRingBuf(int nSize);
    ~CRingBuf();

/*
  AddChar and Add will overwrite old data when the buffer is full.
  This means the add is always successful. They will return the number of bytes
  written without discarding old data.

  AddCharSafe and AddSafe will wait for sufficient free space in the buffer
  before adding the chars. They will Lock and Unlock the buffer as necessary.
  If the Scheduler is active they will sleep for 1ms between attempts to write
  to the buffer. The delay is to stop the routine hogging the buffer lock.
*/

    int AddChar(u8 c);
    int Add(u8 *c, int nLen);
    int AddCharSafe(u8 c);
    int AddSafe(u8 *c, int nLen);
    u8 RemoveChar();
    int Remove(u8 *c, int nSize);
    int GetCount();
    int GetFree();
    void Lock();
    void Unlock();
    
private:

    CSpinLock m_Lock;
    u8 *m_pBuffer;
    int m_nSize;
    int m_nReadPos;
    int m_nWritePos;
    int m_nCount;

};

#endif
