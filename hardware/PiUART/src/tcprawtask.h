#ifndef TCPRAWTASK_H
#define TCPRAWTASK_H

#include <circle/sched/task.h>
#include <circle/net/netsubsystem.h>
#include <circle/net/socket.h>
#include <circle/net/ipaddress.h>

#include "common.h"
#include "ringbuf.h"

class CTCPRawTask : public CTask {
public:

    CTCPRawTask(CSocket *pSocket, CRingBuf<u16> *pToSerial);
    ~CTCPRawTask();

    void Run();

    void Write(const void *pBuffer, unsigned nLength);
    
private:

    CSocket *m_pSocket;
    CRingBuf<u16> *m_pToSerial;

    // define local instance of From rather than use LOGMODULE
    // so each instantiation of the class has a unique log source
    CString m_Name;
    const char *From;
};

#endif
