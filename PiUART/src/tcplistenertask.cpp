#include "tcplistenertask.h"

#include <circle/sched/scheduler.h>
#include <circle/net/in.h>

CTCPListenerTask::CTCPListenerTask(CNetSubSystem *pNet, u16 nListenPort, CRingBuf *pToSerial, TCPMode TMode) {

    m_Name.Format("tcplistenertask-%u", nListenPort);
    From = m_Name;
    SetName(m_Name);
    
    m_pNet = pNet;
    m_nListenPort = nListenPort;
    m_pToSerial = pToSerial;
    m_pListenSocket = NULL;
    m_pTelnetTask = NULL;
    m_pRawTask = NULL;
    m_TMode = TMode;

}

CTCPListenerTask::~CTCPListenerTask() {}

void CTCPListenerTask::Run() {

    switch(m_TMode) {

    case telnet:
	klog(LogNotice, "Starting TCP Listener Task (telnet)");
	break;
	
    case raw:
	klog(LogNotice, "Starting TCP Listener Task (raw)");
	break;

    default:
	assert(false);

    }
    
    assert(m_pNet != NULL);
    
    m_pListenSocket = new CSocket (m_pNet, IPPROTO_TCP);

    assert(m_pListenSocket != NULL);

    if (m_pListenSocket->Bind(m_nListenPort) < 0) {
	
	klog(LogError, "Error binding socket to port %u", m_nListenPort);
	delete m_pListenSocket;
	m_pListenSocket = NULL;
	return;

    }

    if (m_pListenSocket->Listen() < 0) {

	klog(LogError, "Error listening on socket");
	delete m_pListenSocket;
	m_pListenSocket = NULL;
	return;

    }
    
    while(true) {

	CIPAddress RemoteIP;
	u16 nRemotePort;
	CSocket *pConnection = m_pListenSocket->Accept(&RemoteIP, &nRemotePort);

	if (pConnection == NULL) {

	    klog(LogWarning, "Failed to accept connection");
	    continue;

	}

	CString IPString;
	RemoteIP.Format(&IPString);
	klog(LogNotice, "Connection from %s %u", (const char *) IPString, nRemotePort);

	if (IsConnected()) {

	    klog(LogWarning, "Rejecting connection, already connected");
	    delete pConnection;
	    continue;
	    
	}

	if (m_TMode == telnet)
	    m_pTelnetTask = new CTCPTelnetTask(pConnection, m_pToSerial);

	if (m_TMode == raw)
	    m_pRawTask = new CTCPRawTask(pConnection, m_pToSerial);
	
    }

}

bool CTCPListenerTask::IsConnected() {

    if (m_TMode == telnet)
	return CScheduler::Get()->IsValidTask(m_pTelnetTask);

    if (m_TMode == raw)
	return CScheduler::Get()->IsValidTask(m_pRawTask);

    return false;
    
}

void CTCPListenerTask::Write(const void *pBuffer, unsigned nLength) {

    if (IsConnected()) {

	if (m_TMode == telnet)
	    m_pTelnetTask->Write(pBuffer, nLength);

	if (m_TMode == raw)
	    m_pRawTask->Write(pBuffer, nLength);
	
    }
	
}
