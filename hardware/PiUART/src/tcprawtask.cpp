#include "tcprawtask.h"

#include <circle/sched/scheduler.h>
#include <circle/net/in.h>

CTCPRawTask::CTCPRawTask(CSocket *pSocket, CRingBuf<u8> *pToSerial) {

    m_Name.Format("tcprawtask-%x", pSocket);
    From = m_Name;
    SetName(m_Name);

    m_pSocket = pSocket;
    m_pToSerial = pToSerial;

}

CTCPRawTask::~CTCPRawTask() {}

void CTCPRawTask::Run() {

    klog(LogNotice, "Starting TCP Raw Task");

    assert(m_pSocket != NULL);

    u8 Buffer[FRAME_BUFFER_SIZE];
    int nBytesReceived;

    while ((nBytesReceived = m_pSocket->Receive(Buffer, sizeof(Buffer), 0)) > 0) {

        klog(LogDebug, "Got bytes %d", nBytesReceived);

        /*
        for (int i = 0; i < nBytesReceived; i++) {
            u8 c = Buffer[i];
            if ((c >= 32) and (c < 127)) {
                klog(LogDebug, "Got byte [%d]: 0x%x [%c]", i, c, c);
            } else {
                klog(LogDebug, "Got byte [%d]: 0x%x", i, c);
            }
        }
        */

        m_pToSerial->AddSafe(Buffer, nBytesReceived);

    }

    klog(LogNotice, "Closing connection");

    // closes connection
    delete m_pSocket;

}

void CTCPRawTask::Write(const void *pBuffer, unsigned nLength) {

    assert(m_pSocket != NULL);

    m_pSocket->Send(pBuffer, nLength, MSG_DONTWAIT);

}
