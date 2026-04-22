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

CTCPTelnetTask::CTCPTelnetTask(CSocket *pSocket, CRingBuf<u16> *pToSerial, CCharConv *pCharConv) {

    m_Name.Format("tcptelnettask-%x", pSocket);
    From = m_Name;
    SetName(m_Name);

    m_pSocket = pSocket;
    m_pToSerial = pToSerial;
    m_pCharConv = pCharConv;
    m_pTelnet = NULL;

    m_nParserState = StatePlain;
    m_nDigitVal = 0;

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
    // ELKS seems to require \r and linux telnet seems to send \r\0 by default
    m_pTelnet = telnet_init(my_telopts, TelnetEventCB, TELNET_FLAG_NVT_EOL, this);
    // m_pTelnet = telnet_init(my_telopts, TelnetEventCB, 0, this);

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

            char c = ev->data.buffer[i];

            if ((c >= 0x20) and (c < 0x7f)) {
                klog(LogDebug, "Got byte [%d]: 0x%x [%c]", i, c, c);
            } else {
                klog(LogDebug, "Got byte [%d]: 0x%x", i, c);
            }

            // nothing should be giving us chars with the high bit set
            if ((c & 0x80) == 0x00) {
                // Linux telnet sends \r\0 (translated to \r, see above) for enter
                // Linux telnet sends DEL for backspace
                // convert CR/LF, and DEL/^H, etc
                c = me->m_pCharConv->Convert(c);

                klog(LogDebug, "Translated byte [%d]: 0x%x", i, c);
                
                // parse any escape sequences supplied over telnet and convert into scan codes
                me->Parse(c);

            } else {
                klog(LogWarning, "Ignoring bad char 0x%x", c);
            }

        }

        me->Parse(0xFF); // signal to Parse we're done

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

void CTCPTelnetTask::Parse(u8 c) {

    klog(LogDebug, "Process 0x%x State %d", c, m_nParserState);

    u16 s = 0;

    switch(m_nParserState) {
        case StatePlain:
            switch(c) {
                case 0xFF:
                    // normal end of buffer
                    m_nParserState = StatePlain;
                break;

                case '\x1b':
                    m_nParserState = StateEscape;
                break;

                default:
                    m_nParserState = StatePlain;
                    s = m_pCharConv->ScanCode(c);
                break;
            }
        break;

        case StateEscape:
            switch(c) {
                case 0xFF:
                    // bare escape char
                    m_nParserState = StatePlain;
                    s = m_pCharConv->ScanCode('\e');
                break;

                case '[':
                    m_nParserState = StateCSI;
                    m_nDigitVal = 0;
                break;

                case 'O':
                    m_nParserState = StateSS3;
                break;

                default:
                    if (c >= 'a' and c <= 'z') {
                        // ALT a -> ALT z
                        m_nParserState = StatePlain;
                        s = m_pCharConv->ScanCode(c);
                        s &= 0xFF00; // remove the ascii value, leave the scan code
                    } else {
                        // unrecognised escape sequence
                        m_nParserState = StateError;
                    }
                break;
            }
        break;

        // https://en.wikipedia.org/wiki/ANSI_escape_code#Terminal_input_sequences
        case StateCSI:
            switch(c) {
                case 0xFF:
                    m_nParserState = StatePlain;
                    klog(LogDebug, "Process Error, premature end");
                break;

                case 'A':
                    m_nParserState = StatePlain;
                    s = 0x4800; // Up
                break;

                case 'B':
                    m_nParserState = StatePlain;
                    s = 0x5000; // Down
                break;

                case 'C':
                    m_nParserState = StatePlain;
                    s = 0x4D00; // Right
                break;

                case 'D':
                    m_nParserState = StatePlain;
                    s = 0x4B00; // Left
                break;

                case 'F':
                    m_nParserState = StatePlain;
                    s = 0x4F00; // End
                break;

                case 'H':
                    m_nParserState = StatePlain;
                    s = 0x4700; // Home
                break;

                case '~': // end of number sequence
                    m_nParserState = StatePlain;
                    switch(m_nDigitVal) {
                        case 1:
                            s = 0x4700; // Home
                        break;

                        case 3:
                            s = 0x5300; // Delete
                        break;

                        case 4:
                            s = 0x4F00; // End
                        break;

                        case 5:
                            s = 0x4900; // PgUp
                        break;

                        case 6:
                            s = 0x5100; // PgDown
                        break;

                        case 11:
                            s = 0x3B00; // F1
                        break;

                        case 12:
                            s = 0x3C00; // F2
                        break;

                        case 13:
                            s = 0x3D00; // F3
                        break;

                        case 14:
                            s = 0x3E00; // F4
                        break;

                        case 15:
                            s = 0x3F00; // F5
                        break;

                        case 17:
                            s = 0x4000; // F6
                        break;

                        case 18:
                            s = 0x4100; // F7
                        break;

                        case 19:
                            s = 0x4200; // F8
                        break;

                        case 20:
                            s = 0x4300; // F9
                        break;

                        case 21:
                            s = 0x4400; // F10
                        break;

                        default:
                            klog(LogDebug, "Process Error, bad number sequence");
                        break;
                    }
                break;

                default:
                    if (c >= '0' and c <= '9') {
                        m_nParserState = StateCSI;
                        m_nDigitVal *= 10;
                        m_nDigitVal += c - '0';
                    } else {
                        // unrecognised escape sequence
                        m_nParserState = StateError;
                    }
                break;
            }
        break;

        case StateSS3: // function key F1-F4
            switch(c) {
                case 0xFF:
                    m_nParserState = StatePlain;
                    klog(LogNotice, "Process Error, premature end");
                break;

                case 'P':
                    m_nParserState = StatePlain;
                    s = 0x3B00; // F1
                break;

                case 'Q':
                    m_nParserState = StatePlain;
                    s = 0x3C00; // F2
                break;

                case 'R':
                    m_nParserState = StatePlain;
                    s = 0x3D00; // F3
                break;

                case 'S':
                    m_nParserState = StatePlain;
                    s = 0x3E00; // F4
                break;

                case '2':
                    m_nParserState = StateSS3Shift;
                break;

                default:
                    // unrecognised escape sequence
                    m_nParserState = StateError;
                break;
            }
        break;

        case StateSS3Shift: // shift function key F1-F4
            switch(c) {
                case 0xFF:
                    m_nParserState = StatePlain;
                    klog(LogDebug, "Process Error, premature end");
                break;

                case 'P':
                    m_nParserState = StatePlain;
                    s = 0x5400; // shift F1
                break;

                case 'Q':
                    m_nParserState = StatePlain;
                    s = 0x5500; // shift F2
                break;

                case 'R':
                    m_nParserState = StatePlain;
                    s = 0x5600; // shift F3
                break;

                case 'S':
                    m_nParserState = StatePlain;
                    s = 0x5700; // shift F4
                break;

                default:
                    // unrecognised escape sequence
                    m_nParserState = StateError;
                break;
            }
        break;

        case StateError:
            // unrecognised escape sequence
            // wait for end of buffer
            if (c == 0xFF) {
                m_nParserState = StatePlain;
                klog(LogDebug, "Process Error, bad sequence");
            }
        break;
    }

    if (s) {
        m_pCharConv->AddSafe(m_pToSerial, s);
    }

}
