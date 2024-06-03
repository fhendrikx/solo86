#ifndef TCPTELNETTASK_H
#define TCPTELNETTASK_H

#include <circle/sched/task.h>
#include <circle/net/netsubsystem.h>
#include <circle/net/socket.h>
#include <circle/net/ipaddress.h>

#include "common.h"
#include "ringbuf.h"
#include "libtelnet.h"

class CTCPTelnetTask : public CTask {
public:

    CTCPTelnetTask(CSocket *pSocket, CRingBuf *pToSerial);
    ~CTCPTelnetTask();

    void Run();

    void Write(const void *pBuffer, unsigned nLength);

private:

    static void TelnetEventCB(telnet_t *telnet, telnet_event_t *ev, void *arg);
    
    CSocket *m_pSocket;
    CRingBuf *m_pToSerial;
    telnet_t *m_pTelnet;

    // define local instance of From rather than use LOGMODULE
    // so each instantiation of the class has a unique log source
    CString m_Name;
    const char *From;
};

#endif
