#ifndef RINGBUF_H
#define RINGBUF_H

#include <circle/spinlock.h>
#include <circle/sched/scheduler.h>

template <class T>
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

    inline int Add(T e);
    int Add(T *e, int nLen);
    void AddSafe(T e);
    void AddSafe(T *e, int nLen);
    inline int Remove(T *e);
    int Remove(T *e, int nSize);
    inline int GetCount();
    inline int GetFree();

private:

    CSpinLock m_Lock;
    T *m_pBuffer;
    volatile int m_nSize;
    volatile int m_nReadPos;
    volatile int m_nWritePos;
    volatile int m_nCount;

};

template <class T>
CRingBuf<T>::CRingBuf(int nSize) {

    m_pBuffer = new T[nSize];
    m_nSize = nSize;
    m_nReadPos = 0;
    m_nWritePos = 0;
    m_nCount = 0;

}

template <class T>
CRingBuf<T>::~CRingBuf() {
    delete[] m_pBuffer;
}

template <class T>
inline int CRingBuf<T>::Add(T e) {

    m_Lock.Acquire();

    m_pBuffer[m_nWritePos] = e;
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

template <class T>
void CRingBuf<T>::AddSafe(T e) {

    while(true) {

        m_Lock.Acquire();

        if (m_nCount < m_nSize) {
            // space in the buffer
            m_pBuffer[m_nWritePos] = e;
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

template <class T>
int CRingBuf<T>::Add(T *e, int nLen) {

    int nWritten = 0;
    for (int i = 0; i < nLen; i++)
        nWritten += Add(e[i]);

    return nWritten;

}

template <class T>
void CRingBuf<T>::AddSafe(T *e, int nLen) {

    for (int i = 0; i < nLen; i++)
        AddSafe(e[i]);

    return;

}

template <class T>
inline int CRingBuf<T>::Remove(T *e) {

    m_Lock.Acquire();

    if (m_nCount > 0) {
        *e = m_pBuffer[m_nReadPos];
        m_nReadPos = (m_nReadPos + 1) % m_nSize;
        m_nCount--;
        m_Lock.Release();
        return 1;
    }

    m_Lock.Release();
    return 0;

}

template <class T>
int CRingBuf<T>::Remove(T *e, int nSize) {

    int nRemoved = 0;

    while(nRemoved < nSize && Remove(&e[nRemoved]))
        nRemoved++;

    return nRemoved;

}

template <class T>
inline int CRingBuf<T>::GetCount() {
    return m_nCount;
}

template <class T>
inline int CRingBuf<T>::GetFree() {
    return m_nSize - m_nCount;
}

#endif
