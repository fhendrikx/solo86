#include "ringbuf.h"

#include <circle/sched/scheduler.h>

CRingBuf::CRingBuf(int nSize) {

    m_pBuffer = new u8[nSize];
    m_nSize = nSize;
    m_nReadPos = 0;
    m_nWritePos = 0;
    m_nCount = 0;

}

CRingBuf::~CRingBuf() {
    delete[] m_pBuffer;
}

int CRingBuf::AddChar(u8 c) {

    m_pBuffer[m_nWritePos] = c;
    m_nWritePos = (m_nWritePos + 1) % m_nSize;
    if (m_nCount < m_nSize) {
        // space in the buffer
        m_nCount++;
        return 1;
    } else {
        // buffer was full, we have overwitten the oldest value
        m_nReadPos = (m_nReadPos + 1) % m_nSize;
        return 0;
    }

}

int CRingBuf::Add(u8 *c, int nLen) {

    /*
      this could be more efficient using memcpy etc but by calling
      AddChar all the reasoning about updating the indexes
      is handled in one place and simpler
    */

    int nWritten = 0;
    for (int i = 0; i < nLen; i++)
        nWritten += AddChar(c[i]);
    return nWritten;

}

int CRingBuf::AddCharSafe(u8 c) {

    bool bWritten = false;

    do {

        Lock();

        if (GetFree() > 0) {
            AddChar(c);
            Unlock();
            bWritten = true;
        } else {
            Unlock();
            if (CScheduler::IsActive())
                CScheduler::Get()->MsSleep(1);
        }

    } while (bWritten == false);

    return 1;

}

int CRingBuf::AddSafe(u8 *c, int nLen) {

    if (nLen > m_nSize)
        return 0;

    bool bWritten = false;

    do {

        Lock();

        if (GetFree() >= nLen) {
            Add(c, nLen);
            Unlock();
            bWritten = true;
        } else {
            Unlock();
            if (CScheduler::IsActive())
                CScheduler::Get()->MsSleep(10);
        }

    } while (bWritten == false);

    return nLen;

}

u8 CRingBuf::RemoveChar() {

    u8 c = 0;

    if (m_nCount > 0) {
        c = m_pBuffer[m_nReadPos];
        m_nReadPos = (m_nReadPos + 1) % m_nSize;
        m_nCount--;
    }

    return c;

}

int CRingBuf::Remove(u8 *c, int nSize) {

    /*
      this could be more efficient using memcpy etc but by calling
      RemoveChar all the reasoning about updating the indexes
      is handled in one place and simpler
    */

    int nRemoved = 0;
    while(m_nCount > 0 && nRemoved < nSize)
        c[nRemoved++] = RemoveChar();

    return nRemoved;

}

int CRingBuf::GetCount() {
    return m_nCount;
}

int CRingBuf::GetFree() {
    return m_nSize - m_nCount;
}

void CRingBuf::Lock() {
    m_Lock.Acquire();
}

void CRingBuf::Unlock() {
    m_Lock.Release();
}

