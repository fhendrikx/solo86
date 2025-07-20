#include "serialtask.h"

#include <circle/sched/scheduler.h>

LOGMODULE("serialtask");

CSerialTask::CSerialTask(CRingBuf<u8> *pFromSerial, CRingBuf<u8> *pToTerminal, CRingBuf<u8> *pToNetwork) {

    SetName("serialtask");

    m_pFromSerial = pFromSerial;
    m_pToTerminal = pToTerminal;
    m_pToNetwork = pToNetwork;

}

CSerialTask::~CSerialTask() {}

void CSerialTask::Run() {

    klog(LogNotice, "Starting Serial Task");

    u8 c;

    while(true) {

        while (m_pFromSerial->Remove(&c)) {

            m_pToTerminal->Add(c);

            if (c & 0x80) {
                u8 *cptr = (u8 *)cp437_conv[c & 0x7f];
                while(*cptr) {
                    // klog(LogDebug, "%x %x", c, *cptr);
                    m_pToNetwork->Add(*cptr++);
                }
            } else {
                m_pToNetwork->Add(c);
            }

        }

        CScheduler::Get()->MsSleep(1);

    }

}
