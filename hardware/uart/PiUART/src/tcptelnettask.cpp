#include "tcptelnettask.h"

#include <circle/sched/scheduler.h>
#include <circle/net/in.h>
#include <circle/memio.h>

static const telnet_telopt_t my_telopts[] = {
    { TELNET_TELOPT_ECHO,      TELNET_WILL, TELNET_DO },   // I don't think this is necessary
    { TELNET_TELOPT_SGA,       TELNET_WILL, TELNET_DO },   // don't require go-ahead
    { TELNET_TELOPT_LINEMODE,  TELNET_WILL, TELNET_DONT }, // discourage the client from using line mode
    { TELNET_TELOPT_BINARY,    TELNET_WILL, TELNET_DO },   // allow negotiation of binary mode
    { -1, 0, 0 }
};

CTCPTelnetTask::CTCPTelnetTask(CSocket *pSocket, CRingBuf *pToSerial) {

    m_Name.Format("tcptelnettask-%x", pSocket);
    From = m_Name;
    SetName(m_Name);

    m_pSocket = pSocket;
    m_pToSerial = pToSerial;
    m_pTelnet = NULL;

}

CTCPTelnetTask::~CTCPTelnetTask() {

    if (m_pTelnet != NULL)
        telnet_free(m_pTelnet);

}

void CTCPTelnetTask::Run() {

    klog(LogNotice, "Starting TCP Telnet Task");

    assert(m_pSocket != NULL);

    // TELNET_FLAG_NVT_EOL => translate new line sequences
    // \r\0 -> \r
    // \r\n -> \n
    m_pTelnet = telnet_init(my_telopts, TelnetEventCB, TELNET_FLAG_NVT_EOL, this);

    // tell the client not to do local echo, it'll be handled by whatever is out the UART
    telnet_negotiate(m_pTelnet, TELNET_DO, TELNET_TELOPT_ECHO);
    telnet_negotiate(m_pTelnet, TELNET_WILL, TELNET_TELOPT_ECHO);

    telnet_printf(m_pTelnet, "Connected to " VERSION "\n");

    u8 Buffer[FRAME_BUFFER_SIZE];
    int nBytesReceived;

    while ((nBytesReceived = m_pSocket->Receive(Buffer, sizeof(Buffer), 0)) > 0) {

        klog(LogDebug, "Got bytes %d", nBytesReceived);

        /*
        for (int i = 0; i < nBytesReceived; i++) {
            u8 c = Buffer[i];
            if ((c >= 32) and (c < 127)) {
                klog(LogNotice, "Got byte [%d]: 0x%x [%c]", i, c, c);
            } else {
                klog(LogNotice, "Got byte [%d]: 0x%x", i, c);
            }
        }
        */

        telnet_recv(m_pTelnet, (const char *)Buffer, nBytesReceived);
    }

    klog(LogNotice, "Closing connection");

    // closes connection
    delete m_pSocket;

}

void CTCPTelnetTask::Write(const void *pBuffer, unsigned nLength) {

    assert(m_pTelnet != NULL);
    telnet_send_text(m_pTelnet, (const char *)pBuffer, nLength);

}

void CTCPTelnetTask::TelnetEventCB(telnet_t *telnet, telnet_event_t *ev, void *arg) {

    assert(arg != NULL);

    CTCPTelnetTask *me = (CTCPTelnetTask *)arg;
#ifndef NDEBUG
    const char *From = me->From;
#endif

    switch (ev->type) {
    case TELNET_EV_DATA:

        klog(LogDebug, "TelnetEventCB TELNET_EV_DATA size = %u", ev->data.size);

        for (unsigned i = 0; i < ev->data.size; i++) {

            u8 c = ev->data.buffer[i];

            if ((c >= 0x20) and (c < 0x7f)) {
                klog(LogDebug, "Got byte [%d]: 0x%x [%c]", i, c, c);
            } else {
                klog(LogDebug, "Got byte [%d]: 0x%x", i, c);
            }

            // fix carriage return
            if (c == '\r')
                c = '\n';

            // fix backspace
            // if (c == 0x7f)
            //     c = 0x8; // ascii backspace

            klog(LogDebug, "Translated byte [%d]: 0x%x", i, c);

            me->m_pToSerial->AddCharSafe(c);

        }

        break;

    case TELNET_EV_SEND:

        klog(LogDebug, "TelnetEventCB TELNET_EV_SEND size = %u", ev->data.size);

        assert(me->m_pSocket != NULL);
        me->m_pSocket->Send(ev->data.buffer, ev->data.size, MSG_DONTWAIT);

        break;

    case TELNET_EV_IAC:

        klog(LogDebug, "TelnetEventCB TELNET_EV_IAC cmd = 0x%x", ev->iac.cmd);

        if (ev->iac.cmd == 0xf3) {
            klog(LogNotice, "TelnetEventCB BREAK received");

            // This might be dodgy, using the main thread to update the GPIO registers
            // that are also being updated by the GPIO thread

            //GPIOBreakReset();
            write32(ARM_GPIO_GPSET0, 1 << 16);
            CTimer::SimpleMsDelay(100);
            write32(ARM_GPIO_GPCLR0, 1 << 16);

        }

        break;

    case TELNET_EV_ERROR:

        klog(LogNotice, "TelnetEventCB TELNET_EV_ERROR %s\n", ev->error.msg);

        break;

        /*
    case TELNET_EV_WILL:
    case TELNET_EV_WONT:
    case TELNET_EV_DO:
    case TELNET_EV_DONT:

        klog(LogNotice, "TelnetEventCB TELNET_EV_ negotiation type = %u telopt = %u", ev->type, ev->neg.telopt);

        break;
        */

    default:

        klog(LogDebug, "TelnetEventCB default type = %u", ev->type);

        break;
    }

}
