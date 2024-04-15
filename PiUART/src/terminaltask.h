#ifndef TERMINALTASK_H
#define TERMINALTASK_H

#include <circle/sched/task.h>

#include "common.h"
#include "ringbuf.h"
#include "terminal.h"

class CTerminalTask : public CTask {
public:
    CTerminalTask(CTerminal *pTerminal, CRingBuf *pToTerminal);
    ~CTerminalTask();
    void Run();
    
private:

    CTerminal *m_pTerminal;
    CRingBuf *m_pToTerminal;

};

#endif

