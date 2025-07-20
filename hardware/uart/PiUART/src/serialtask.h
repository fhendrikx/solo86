#ifndef SERIALTASK_H
#define SERIALTASK_H

#include <circle/sched/task.h>

#include "common.h"
#include "ringbuf.h"
#include "kernel.h"

class CSerialTask : public CTask {
public:
    CSerialTask(CRingBuf<u8> *pFromSerial, CRingBuf<u8> *pToTerminal, CRingBuf<u8> *pToNetwork);
    ~CSerialTask();
    void Run();

private:
    CRingBuf<u8> *m_pFromSerial;
    CRingBuf<u8> *m_pToTerminal;
    CRingBuf<u8> *m_pToNetwork;
};

#endif

