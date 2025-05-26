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

int CRingBuf::Add(u8 c) {

    m_Lock.Acquire();

    m_pBuffer[m_nWritePos] = c;
    m_nWritePos = (m_nWritePos + 1) % m_nSize;
    if (m_nCount < m_nSize) {
        // space in the buffer
        m_nCount++;
        m_Lock.Release();
        return 1;
    } else {
        // buffer was full, we have overwitten the oldest value
        m_nReadPos = (m_nReadPos + 1) % m_nSize;
        m_Lock.Release();
        return 0;
    }

}

void CRingBuf::AddSafe(u8 c) {

    bool bWritten = false;

    while(true) {

        m_Lock.Acquire();

        if (m_nCount < m_nSize) {
            // space in the buffer
            m_pBuffer[m_nWritePos] = c;
            m_nWritePos = (m_nWritePos + 1) % m_nSize;
            m_nCount++;
            m_Lock.Release();
            return;
        } else {
            m_Lock.Release();
            if (CScheduler::IsActive())
                CScheduler::Get()->MsSleep(1);
        }

    }

}

int CRingBuf::Add(u8 *c, int nLen) {
    
    int nWritten = 0;
    for (int i = 0; i < nLen; i++)
        nWritten += Add(c[i]);

    return nWritten;

}

void CRingBuf::AddSafe(u8 *c, int nLen) {

    for (int i = 0; i < nLen; i++)
        AddSafe(c[i]);

    return;

}

int CRingBuf::Remove(u8 *c) {

    m_Lock.Acquire();
    
    if (m_nCount > 0) {
        *c = m_pBuffer[m_nReadPos];
        m_nReadPos = (m_nReadPos + 1) % m_nSize;
        m_nCount--;
        m_Lock.Release();
        return 1;
    }

    m_Lock.Release();
    return 0;

}

int CRingBuf::Remove(u8 *c, int nSize) {

    int nRemoved = 0;

    while(nRemoved < nSize && Remove(&c[nRemoved]))
        nRemoved++;

    return nRemoved;

}

int CRingBuf::GetCount() {
    return m_nCount;
}

int CRingBuf::GetFree() {
    return m_nSize - m_nCount;
}

