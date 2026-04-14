#ifndef TCPLISTENERTASK_H
#define TCPLISTENERTASK_H

#include <circle/sched/task.h>
#include <circle/net/netsubsystem.h>
#include <circle/net/socket.h>
#include <circle/net/ipaddress.h>

#include "charconv.h"
#include "common.h"
#include "tcptelnettask.h"
#include "tcprawtask.h"
#include "ringbuf.h"

class CTCPListenerTask : public CTask {
public:
    CTCPListenerTask(CNetSubSystem *pNet, u16 nListenPort, CRingBuf<u16> *pToSerial, TCPMode TMode, CCharConv *pCharConv);
    ~CTCPListenerTask();

    void Run();

    bool IsConnected();
    void Write(const void *pBuffer, unsigned nLength);
    
private:
    CNetSubSystem *m_pNet;
    u16 m_nListenPort;
    CRingBuf<u16> *m_pToSerial;
    CSocket *m_pListenSocket;
    CTCPTelnetTask *m_pTelnetTask;
    CTCPRawTask *m_pRawTask;
    TCPMode m_TMode;
    CCharConv *m_pCharConv;

    // define local instance of From rather than use LOGMODULE
    // so each instantiation of the class has a unique log source
    CString m_Name;
    const char *From;

};

#endif
