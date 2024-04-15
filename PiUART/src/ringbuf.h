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
*/

    int AddChar(u8 c);
    int Add(u8 *c, int nLen);
    u8 RemoveChar();
    int Remove(u8 *c, int nSize);
    int GetCount();
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
