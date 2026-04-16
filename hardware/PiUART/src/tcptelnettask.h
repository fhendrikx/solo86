#ifndef TCPTELNETTASK_H
#define TCPTELNETTASK_H

#include <circle/sched/task.h>
#include <circle/net/netsubsystem.h>
#include <circle/net/socket.h>
#include <circle/net/ipaddress.h>

#include "charconv.h"
#include "common.h"
#include "ringbuf.h"
#include "libtelnet.h"

enum ProcessState { StatePlain, StateEscape, StateCSI, StateSS3, StateSS3Shift, StateError };

class CTCPTelnetTask : public CTask {
public:

    CTCPTelnetTask(CSocket *pSocket, CRingBuf<u16> *pToSerial, CCharConv *pCharConv);
    ~CTCPTelnetTask();

    void Run();

    void Write(const void *pBuffer, unsigned nLength);

private:

    void Process(u8 c);
    ProcessState m_nProcessState;
    int m_nDigitVal;

    static void TelnetEventCB(telnet_t *telnet, telnet_event_t *ev, void *arg);
    
    CSocket *m_pSocket;
    CRingBuf<u16> *m_pToSerial;
    telnet_t *m_pTelnet;
    CCharConv *m_pCharConv;

    // define local instance of From rather than use LOGMODULE
    // so each instantiation of the class has a unique log source
    CString m_Name;
    const char *From;
};

#endif
