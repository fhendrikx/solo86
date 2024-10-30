#include "terminaltask.h"

#include <circle/sched/scheduler.h>

LOGMODULE("terminaltask");

CTerminalTask::CTerminalTask(CTerminal *pTerminal, CRingBuf *pToTerminal) {

    SetName("terminaltask");

    m_pTerminal = pTerminal;
    m_pToTerminal = pToTerminal;
}

CTerminalTask::~CTerminalTask() {}

void CTerminalTask::Run() {

    klog(LogNotice, "Starting Terminal Task");

    while(true) {

        while (m_pToTerminal->GetCount()) {

            m_pToTerminal->Lock();
            char c = m_pToTerminal->RemoveChar();
            m_pToTerminal->Unlock();

            if (c) {
                m_pTerminal->Write(c);
            }

        }

        CScheduler::Get()->MsSleep(1);

    }

}

