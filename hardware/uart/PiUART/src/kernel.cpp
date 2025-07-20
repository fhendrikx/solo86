#include "kernel.h"
#include "serialtask.h"
#include "keyboardtask.h"
#include "tcplistenertask.h"
#include "palette/vga_rgb565.h"

#define min(a, b) ((a) < (b) ? (a) : (b))

#define DRIVE           "SD:"
#define FIRMWARE_PATH   DRIVE "/firmware/"              // firmware files must be provided here
#define CONFIG_FILE     DRIVE "/wpa_supplicant.conf"

const char hostname[] = "PiUART";

LOGMODULE("kernel");

CKernel::CKernel(CMemorySystem *pMemorySystem)
    : CMultiCoreSupport(pMemorySystem),
      m_Timer(&m_InterruptSystem),
#ifndef NDEBUG
      m_Logger(m_CmdLine.GetLogLevel(), &m_Timer),
#endif
      m_I2C(CMachineInfo::Get()->GetDevice(DeviceI2CMaster), true),
#ifndef NDEBUG
      m_I2CLogger(&m_I2C),
#endif
      m_ToDisplay(RING_BUF_SIZE),
      m_LCD(LCD_WIDTH, LCD_HEIGHT, &m_I2C, LCD_I2C_ADDR, false, false),
      m_USBHCI(&m_InterruptSystem, &m_Timer, true),
      m_EMMC(&m_InterruptSystem, &m_Timer),
      m_WLAN(FIRMWARE_PATH),
      m_Net(0, 0, 0, 0, hostname, NetDeviceTypeWLAN),
      m_WPASupplicant(CONFIG_FILE),
      m_ToSerial(RING_BUF_SIZE),
      m_FromSerial(RING_BUF_SIZE),
      m_ToTerminal(RING_BUF_SIZE),
      m_ToNetwork(RING_BUF_SIZE)
{

    m_nLogLevel = m_CmdLine.GetLogLevel();

    m_pFrameBuffer = NULL;
    m_pTerminal = NULL;
    m_pLog = NULL;
    m_p2DGraphics = NULL;

    // m_pBuffer = NULL;
    // m_pBuffer0 = NULL;
    // m_pBuffer1 = NULL;
    // m_bBufferSwapped = true;

    // m_nMode = MODE_CON;
    // m_nPrevMode = MODE_CON;
    // m_nBusRamPtr = 0;
    // m_bReadyForSwap = true;
    // m_bAutoIncrementWrite = false;
    // m_bAutoIncrementRead = false;

    m_nScreenWidth = m_CmdLine.GetWidth();
    m_nScreenHeight = m_CmdLine.GetHeight();

    m_nDisplayMode = Uninitialised;
    m_nNextDisplayMode = ConsoleMode;

    m_nConsoleMode = TerminalMode;
    m_nNextConsoleMode = TerminalMode;

    m_bUartIntEnable = false;
    m_bUartIntActive = false;

    // memset(m_pRam, 0, RAM_SIZE * 2);
    // m_pBusRam = m_pRam;
    // m_pDisRam = &m_pRam[RAM_SIZE];

    m_nTestPort = 0;

}

CKernel::~CKernel(void) {}

bool CKernel::Initialize() {

    if (!m_InterruptSystem.Initialize())
        return false;

    if (!m_Timer.Initialize())
        return false;

    if (!m_I2C.Initialize())
        return false;

#ifndef NDEBUG
    if (!m_Logger.Initialize(&m_I2CLogger))
        return false;
#endif

    // now we have a working logging system
    // output some basic debug info
    klog(LogNotice, VERSION);
    klog(LogNotice, "Compile time: " __DATE__ " " __TIME__);
    klog(LogNotice, "Clock: %u MHz, Temperature: %u",
                    CCPUThrottle::Get()->GetClockRate() / 1000000,
                    CCPUThrottle::Get()->GetTemperature());

    // fails if deferred :(
    if (!m_USBHCI.Initialize(false)) {
        klog(LogError, "USBHCI init failed");
        return false;
    } else
        klog(LogNotice, "USBHCI init complete");

    // all other initialisation is deferred and handled by the MAIN core

    return CMultiCoreSupport::Initialize();

}

bool CKernel::DeferredInitialize() {

    if (!m_LCD.Initialize()) {
        klog(LogError, "LCD init failed");
        return false;
    } else
        klog(LogNotice, "LCD init complete");

    // put something on the LCD display so we know it's working
    m_LCD.Write(VERSION "\n", strlen(VERSION) + 1);

    if (!m_EMMC.Initialize()) {
        klog(LogError, "EMMC init failed");
        return false;
    } else
        klog(LogNotice, "EMMC init complete");

    if (f_mount (&m_FileSystem, DRIVE, 1) != FR_OK) {
        klog(LogError, "Cannot mount drive: %s", DRIVE);
        return false;
    } else
        klog(LogNotice, "Filesystem %s mounted", DRIVE);

    if (!m_WLAN.Initialize()) {
        klog(LogError, "WLAN init failed");
        return false;
    } else
        klog(LogNotice, "WLAN init complete");

    if (!m_Net.Initialize(false)) {
        klog(LogError, "Net init failed");
        return false;
    } else
        klog(LogNotice, "Net init complete");

    if (!m_WPASupplicant.Initialize()) {
        klog(LogError, "WPA Supplicant init failed");
        return false;
    } else
        klog(LogNotice, "WPA Supplicant init complete");

    return true;
}

void CKernel::Run(unsigned nCore) {

    switch(nCore) {

    case CORE_MAIN:

        klog(LogNotice, "CKernel::Run CORE_MAIN");
        Main();

        break;

    case CORE_DISPLAY:

        klog(LogNotice, "CKernel::Run CORE_DISPLAY");
        Display();

        break;

    case CORE_GPIO:

        klog(LogNotice, "CKernel::Run CORE_GPIO");
        GPIO();

        break;

    default:
        klog(LogDebug, "CKernel::Run default");
        break;

    }

}

void CKernel::SetConsole(unsigned nConsoleMode) {

    if (nConsoleMode < NUM_CONSOLE_MODES) {
        klog(LogDebug, "SetConsole %d", nConsoleMode);
        m_nNextConsoleMode = (enum TConsoleMode) nConsoleMode;
    } else {
        klog(LogDebug, "SetConsole, ignoring %d", nConsoleMode);
    }

    return;
}

//
//  Cores
//

void CKernel::Display() {

    int x = 0;
    int y = 0;

    // main display loop
    while(true) {

        if (m_nNextDisplayMode != m_nDisplayMode) {

            // clean up current mode
            switch(m_nDisplayMode) {

                case Uninitialised:
                    // nothing to do
                break;

                case ConsoleMode:
                    DestroyConsole();
                break;

                case GraphicsMode:
                    DestroyGraphics();
                break;

            }

            // setup next mode
            switch(m_nNextDisplayMode) {

                case Uninitialised:
                    klog(LogPanic, "Trying to switch to Uninitialised display mode");
                break;

                case ConsoleMode:
                    CreateConsole();
                break;

                case GraphicsMode:
                    CreateGraphics();
                break;

            }

            m_nDisplayMode = m_nNextDisplayMode;

        }

        if (m_nDisplayMode == ConsoleMode) {
            UpdateConsole();
        }

        if (m_nDisplayMode == GraphicsMode) {

            unsigned nStartTime = CTimer::GetClockTicks();

            m_p2DGraphics->ClearScreen(CDisplay::Black);

            m_p2DGraphics->DrawCircle(x % 512, y % 384, 30, CDisplay::Green);

            m_p2DGraphics->DrawRect((200 + x) % 512, (200 + y) % 384, 30, 30, CDisplay::Yellow);

            unsigned nEndTime = CTimer::GetClockTicks();

            klog(LogNotice, "frame time %u", nEndTime - nStartTime);


            m_p2DGraphics->UpdateDisplay();



            x++;
            y++;

        }

        // TODO process commands and data
        while (m_ToDisplay.GetCount()) {

            u16 wibble = 0;
            m_ToDisplay.Remove(&wibble);
            klog(LogNotice, "ToDisplay %x", wibble);

            if (wibble == 0x100) {
                m_nNextDisplayMode = ConsoleMode;
            }

            if (wibble == 0x101) {
                m_nNextDisplayMode = GraphicsMode;
            }

        }


        // CTimer::SimpleusDelay(1);

    }
/*
    while(true) {

        // make a local copy of Mode, we don't want it changing midway through
        // updating the screen
        u8 nLocalMode = m_nMode;

        // check for mode change
        if (nLocalMode != m_nPrevMode) {

            klog(LogNotice, "Mode switch %u", nLocalMode);

            switch(nLocalMode) {

            case MODE_CON:

                if (InitFB(m_nScreenWidth, m_nScreenHeight)) {

                    klog(LogNotice, "Set FB to %ux%u", m_pFrameBuffer->GetWidth(), m_pFrameBuffer->GetHeight());

                } else {
                    // framebuffer object is unusable, bomb out
                    klog(LogPanic, "Mode switch failed to init FB");
                }

                break;

            case MODE_256x192:
            case MODE_256x192_DB:

                if (ResizeFB(m_nScreenWidth, m_nScreenHeight, 256, 192)) {

                    klog(LogNotice, "Set FB to %ux%u", m_pFrameBuffer->GetWidth(), m_pFrameBuffer->GetHeight());

                } else {
                    // framebuffer object is unusable, bomb out
                    klog(LogPanic, "Mode switch failed to resize FB");
                }

                break;

            default:
                break;
            }

            m_nPrevMode = nLocalMode;

        }

        // update the screen
        switch(nLocalMode) {

        case MODE_CON:

            // m_Terminal.UpdateDisplay((u8 *)m_pBuffer);
            UpdateFB();
            break;

        case MODE_256x192:

            UpdateMode256x192(m_pBusRam);
            UpdateFB();
            break;

        case MODE_256x192_DB:

            if (m_bReadyForSwap == false) {

                UpdateMode256x192(m_pDisRam);
                UpdateFB();

                // clear the buffer
                memset(m_pDisRam, 0, RAM_SIZE);

                // signal redraw is done
                m_bReadyForSwap = true;

            }

            break;

        default:
            CTimer::SimpleMsDelay(1);
            break;
        }

    } */
}

void CKernel::GPIO() {

    // setup the GPIO pins
    GPIOInit();

    // set PWAIT output to 0 to indicate we're ready
    GPIOPWaitReady();

    // main GPIO loop
    while (true) {

        // raise an interrupt if necessary
        if (m_bUartIntEnable and not m_bUartIntActive and
            m_ToSerial.GetCount() > 0) {

            m_bUartIntActive = true;
            GPIOInterruptRaise();

        }

        // check for an event
        u32 pins = GPIORead();

        if (pins & PEVENT) {

            u32 address = (pins >> 4) & 0x7; // A2-A0
            u32 prdwr = pins & PRDWR; // 0 == read

            if (prdwr == 0) {
                // bus io read

                // fetch the data
                u32 data = BusIORead(address);

                // output the data
                GPIODataOutput(data);

                // release the wait signal
                GPIOPWaitBusy();

            } else {
                // bus io write

                // release the wait signal as we have everything we need
                GPIOPWaitBusy();

                // grab the data
                u8 data = (pins >> 20) & 0xff;

                // process the data
                BusIOWrite(address, data);

            }

            // wait for event to finish
            while (GPIORead() & PEVENT);

            if (prdwr == 0) {
                // bus read

                // set data pins back to input mode
                GPIODataInput();
            }

            // signal that we're done with the bus and are safe
            GPIOPWaitReady();

        }
    }

}

void CKernel::Main() {

    /*
      This method runs on core 0 so handles all interrupts and multi-tasking.
      Don't do anything that requires accurate timing here.
    */

    new CSerialTask(&m_FromSerial, &m_ToTerminal, &m_ToNetwork);

    // finish the rest of the initialisation
    if (!DeferredInitialize())
        return;

    klog(LogNotice, "Deferred init complete");

    // launch the task that looks after the USB keyboard
    new CKeyboardTask(&m_USBHCI, &m_ToSerial, this);

    klog(LogNotice, "WPA waiting for connection");

    // wait for WPA to become connected
    while(!m_WPASupplicant.IsConnected()) {
        m_Scheduler.MsSleep(100);
    }

    klog(LogNotice, "WPA connected");

    // wait for the network (dhcp) to be ready
    while(!m_Net.IsRunning()) {
        m_Scheduler.MsSleep(100);
    }

    // do one-off work now the network is available
    CString IPString;
    m_Net.GetConfig()->GetIPAddress()->Format(&IPString);
    klog(LogDebug, "IP %s", (const char *)IPString);
    m_LCD.Write((const char *)IPString, IPString.GetLength());

    // start TCP listeners
    CTCPListenerTask *TelnetListener = new CTCPListenerTask(&m_Net, TELNET_PORT, &m_ToSerial, telnet);
    CTCPListenerTask *RawListener = new CTCPListenerTask(&m_Net, RAW_PORT, &m_ToSerial, raw);

    u8 Buffer[FRAME_BUFFER_SIZE];
    int nBytesSent;
    int nBytesWaiting;
    unsigned nNetworkDelayClockTicks = 0;

    /*
      The network stack sends data as soon as it's written.
      Given the speed of the UART writes from the bus this results in
      a stream of 1 byte packets being sent.
      Instead wait a bit before sending so we can accumulate more data.
    */

    // main loop
    while(true) {

        //m_Scheduler.ListTasks(&m_I2CLogger);

        nBytesWaiting = m_ToNetwork.GetCount();
        unsigned nNow = CTimer::GetClockTicks();

        if (nBytesWaiting > 0) {

            if (nNetworkDelayClockTicks == 0)
                nNetworkDelayClockTicks = nNow;

            if (nBytesWaiting >= NETWORK_DELAY_BYTES or
                (nNow - nNetworkDelayClockTicks) >= NETWORK_DELAY_US) {

                if (RawListener->IsConnected()) {

                    nBytesSent = m_ToNetwork.Remove(Buffer, FRAME_BUFFER_SIZE);

                    RawListener->Write(Buffer, nBytesSent);
                    nNetworkDelayClockTicks = 0;

                    klog(LogDebug, "Sent raw bytes %d", nBytesSent);
                    /*
                      for (int i = 0; i < nBytesSent; i++) {
                      u8 c = Buffer[i];
                      klog(LogDebug, "Sent byte: 0x%x", c);
                      }
                    */

                } else if (TelnetListener->IsConnected()) {

                    nBytesSent = m_ToNetwork.Remove(Buffer, FRAME_BUFFER_SIZE);

                    TelnetListener->Write(Buffer, nBytesSent);
                    nNetworkDelayClockTicks = 0;

                    klog(LogDebug, "Sent telnet bytes %d", nBytesSent);
                }

            }

        }

        m_Scheduler.MsSleep(1);
        //m_Scheduler.Yield();

    }

}

//
//  Helper functions
//

void CKernel::CreateConsole() {

    m_pFrameBuffer = new CBcmFrameBuffer(m_nScreenWidth, m_nScreenHeight, DEPTH);

    if (m_pFrameBuffer == NULL) {
        // framebuffer object is unusable, bomb out
        klog(LogPanic, "Failed to create frame buffer");
    }

    m_pFrameBuffer->SetPalette (BLACK_COLOR, BLACK_COLOR);
    m_pFrameBuffer->SetPalette (RED_COLOR, RED_COLOR16);
    m_pFrameBuffer->SetPalette (GREEN_COLOR, GREEN_COLOR16);
    m_pFrameBuffer->SetPalette (YELLOW_COLOR, YELLOW_COLOR16);
    m_pFrameBuffer->SetPalette (BLUE_COLOR, BLUE_COLOR16);
    m_pFrameBuffer->SetPalette (MAGENTA_COLOR, MAGENTA_COLOR16);
    m_pFrameBuffer->SetPalette (CYAN_COLOR, CYAN_COLOR16);
    m_pFrameBuffer->SetPalette (WHITE_COLOR, WHITE_COLOR16);
    m_pFrameBuffer->SetPalette (BRIGHT_BLACK_COLOR, BRIGHT_BLACK_COLOR16);
    m_pFrameBuffer->SetPalette (BRIGHT_RED_COLOR, BRIGHT_RED_COLOR16);
    m_pFrameBuffer->SetPalette (BRIGHT_GREEN_COLOR, BRIGHT_GREEN_COLOR16);
    m_pFrameBuffer->SetPalette (BRIGHT_YELLOW_COLOR, BRIGHT_YELLOW_COLOR16);
    m_pFrameBuffer->SetPalette (BRIGHT_BLUE_COLOR, BRIGHT_BLUE_COLOR16);
    m_pFrameBuffer->SetPalette (BRIGHT_MAGENTA_COLOR, BRIGHT_MAGENTA_COLOR16);
    m_pFrameBuffer->SetPalette (BRIGHT_CYAN_COLOR, BRIGHT_CYAN_COLOR16);
    m_pFrameBuffer->SetPalette (BRIGHT_WHITE_COLOR, BRIGHT_WHITE_COLOR16);

    if (!m_pFrameBuffer->Initialize()) {
        // framebuffer object is unusable, bomb out
        klog(LogPanic, "Failed to init frame buffer");
    } else
        klog(LogNotice, "Console frame buffer init %ux%u", m_pFrameBuffer->GetWidth(),
                        m_pFrameBuffer->GetHeight());

    m_nScreenWidth = m_pFrameBuffer->GetWidth();
    m_nScreenHeight = m_pFrameBuffer->GetHeight();

    const TFont *pTerminalFont;

    if (m_nScreenHeight <= 600) {
        // 640x480, 800x600
        klog(LogNotice, "Using small fonts");
        pTerminalFont = &ter_i16n;
        TLogFont = Font8x10;
        TLogFont.extra_height = 0;
    } else if (m_nScreenHeight <= 900) {
        // 1024x768
        klog(LogNotice, "Using medium fonts");
        pTerminalFont = &ter_i24b;
        TLogFont = Font8x14;
        TLogFont.extra_height = 0;
    } else {
        // everything larger
        klog(LogNotice, "Using large fonts");
        pTerminalFont = &ter_i32b;
        TLogFont = Font8x16;
        TLogFont.extra_height = 0;
    }

    m_pTerminal = new CTerminalDevice(m_pFrameBuffer, 0, *pTerminalFont);

    if (m_pTerminal == NULL or !m_pTerminal->Initialize()) {
        // terminal is unusable, bomb out
        klog(LogPanic, "Failed to create or init Terminal");
    } else {
        klog(LogNotice, "Terminal init complete %ux%u", m_pTerminal->GetColumns(), m_pTerminal->GetRows());
    }

    m_pLog = new CTerminalDevice(m_pFrameBuffer, 0, TLogFont);

    if (m_pLog == NULL or !m_pLog->Initialize()) {
        // log is unusable, bomb out
        klog(LogPanic, "Failed to create or init Log");
    } else {
        klog(LogNotice, "Log init complete %ux%u", m_pLog->GetColumns(), m_pLog->GetRows());
    }

    // by default CScreenDevice uses an underscore as a cursor, but it does so by
    // placing it beneath the character. This requires the font to have extra height
    // to accomodate it which makes the spacing look weird.
    // instead use the block cursor so we can specify a zero extra height in our fonts
    m_pTerminal->SetCursorBlock(true);
    m_pLog->SetCursorBlock(true);

    for (int i=0; i < NUM_CONSOLE_MODES; i++) {
        m_pFrameBufferBackups[i] = new u8[m_pFrameBuffer->GetSize()];
        memset(m_pFrameBufferBackups[i], 0, m_pFrameBuffer->GetSize());
    }

    m_nConsoleMode = TerminalMode;
    m_nNextConsoleMode = TerminalMode;

}

void CKernel::DestroyConsole() {

    klog(LogNotice, "DestroyConsole");

    for (int i=0; i < NUM_CONSOLE_MODES; i++) {
        if (m_pFrameBufferBackups[i] != NULL)
            delete m_pFrameBufferBackups[i];
        m_pFrameBufferBackups[i] = NULL;
    }

    if (m_pLog != NULL)
        delete m_pLog;
    m_pLog = NULL;

    if (m_pTerminal != NULL)
        delete m_pTerminal;
    m_pTerminal = NULL;

    if (m_pFrameBuffer != NULL)
        delete m_pFrameBuffer;
    m_pFrameBuffer = NULL;

}

void CKernel::UpdateConsole() {

    enum TConsoleMode nNextConsoleMode = m_nNextConsoleMode;

    if (m_nConsoleMode != m_nNextConsoleMode) {
        // backup the current frame buffer, then restore the next frame buffer
        memcpy(m_pFrameBufferBackups[m_nConsoleMode], (void *)m_pFrameBuffer->GetBuffer(), m_pFrameBuffer->GetSize());
        memcpy((void *)m_pFrameBuffer->GetBuffer(), m_pFrameBufferBackups[nNextConsoleMode], m_pFrameBuffer->GetSize());
        m_nConsoleMode = nNextConsoleMode;
    }

    if (m_nConsoleMode == TerminalMode) {

        while (m_ToTerminal.GetCount()) {

            // copy chars into a buffer so we can handle multiple chars per Write()
            // this is because each call to Write() results in a screen update so it's
            // more efficient to bundle the chars together
            u8 buf[TERM_BUF_SIZE];
            int nRemoved = m_ToTerminal.Remove(buf, TERM_BUF_SIZE);

            m_pTerminal->Write((char *)buf, nRemoved);

        }

    }

    if (m_nConsoleMode == LogMode) {

        char text[LOGGER_BUFSIZE];

        int numBytes = m_Logger.Read(text, LOGGER_BUFSIZE, true);

        // Read returns -1 if there's nothing to read
        if (numBytes > 0)
            m_pLog->Write(text, numBytes);

    }

}

void CKernel::CreateGraphics() {

    m_p2DGraphics = new C2DGraphics(512, 384);

    if (m_p2DGraphics == NULL || !m_p2DGraphics->Initialize()) {
        klog(LogPanic, "Failed to create or init 2DGraphics");
    } else {
        klog(LogNotice, "2DGraphics init complete %ux%u", m_p2DGraphics->GetWidth(), m_p2DGraphics->GetHeight());
    }

    m_p2DGraphics->ClearScreen(CDisplay::Black);

}

void CKernel::DestroyGraphics() {

    klog(LogNotice, "DestroyGraphics");

    if (m_p2DGraphics != NULL)
        delete m_p2DGraphics;
    m_p2DGraphics = NULL;

}

/*
void CKernel::UpdateMode256x192(u8 *pRam) {

    //unsigned nStartTime = CTimer::GetClockTicks();

    unsigned nModeWidth = 256;
    unsigned nModeHeight = 192;

    unsigned nFBWidth = m_pFrameBuffer->GetWidth();
    unsigned nFBHeight = m_pFrameBuffer->GetHeight();

    unsigned nHorizMult = nFBWidth / nModeWidth;
    unsigned nVertMult = nFBHeight / nModeHeight;
    unsigned nMult = min(nHorizMult, nVertMult);

    unsigned nHorizGutter = (nFBWidth - (nMult * nModeWidth)) / 2;
    unsigned nVertGutter = (nFBHeight - (nMult * nModeHeight)) / 2;

    unsigned nIdxZ = 0;
    unsigned nIdxP = 0;

    nIdxP = nFBWidth * nVertGutter;

    // klog(LogDebug, "nFBWidth %u", nFBWidth);
    // klog(LogDebug, "nFBHeight %u", nFBHeight);
    // klog(LogDebug, "nHorizMult %u", nHorizMult);
    // klog(LogDebug, "nHorizGutter %u", nHorizGutter);
    // klog(LogDebug, "nVertMult %u", nVertMult);
    // klog(LogDebug, "nVertGutter %u", nVertGutter);
    // klog(LogDebug, "nMult %u", nMult);

    for (unsigned nY = 0; nY < nModeHeight; nY++) {

        unsigned nIdxZZ = 0;

        for (unsigned nYY = 0; nYY < nMult; nYY++) {

            nIdxZZ = nIdxZ;
            nIdxP += nHorizGutter;

            for (unsigned nX = 0; nX < nModeWidth; nX++) {

#if DEPTH == 8
                TPixel nC = pRam[nIdxZZ++];
#elif DEPTH == 16
                TPixel nC = pCMap[pRam[nIdxZZ++]];
#endif

                for (unsigned nXX = 0; nXX < nMult; nXX++) {

                    m_pBuffer[nIdxP++] = nC;

                }

            }

            nIdxP += nHorizGutter;

        }

        nIdxZ = nIdxZZ;

    }

    //unsigned nEndTime = CTimer::GetClockTicks();
    //klog(LogNotice, "frame time %u", nEndTime - nStartTime);

}
*/

/*
bool CKernel::InitFB(unsigned nWidth, unsigned nHeight) {

    klog(LogDebug, "InitFB");

    if (m_pFrameBuffer != NULL)
        delete m_pFrameBuffer;

    m_pFrameBuffer = new CBcmFrameBuffer (nWidth, nHeight, DEPTH, 0, 0, 0, true);

    if (m_pFrameBuffer == NULL) {
        klog(LogError, "FrameBuffer alloc failed");
        return FALSE;
    }

#if DEPTH == 8

    // set the palette
    for (int i=0; i < 256; i++) {
        m_pFrameBuffer->SetPalette(i, pCMap[i]);
    }

#endif

    if (!m_pFrameBuffer->Initialize())
        return false;

    // setup the buffer pointers
    m_pBuffer0 = (TPixel *) m_pFrameBuffer->GetBuffer();
    m_pBuffer1 = m_pBuffer0 + m_pFrameBuffer->GetWidth() * m_pFrameBuffer->GetHeight();
    m_pBuffer = m_pBuffer1;

    m_bBufferSwapped = true;

    memset(m_pBuffer0, 0, m_pFrameBuffer->GetSize());

    return true;

}
*/

/*
void CKernel::UpdateFB() {

    m_pFrameBuffer->SetVirtualOffset(0, m_bBufferSwapped ? m_pFrameBuffer->GetHeight() : 0);
    m_pBuffer = m_bBufferSwapped ? m_pBuffer0 : m_pBuffer1;
    m_pFrameBuffer->WaitForVerticalSync();
    m_bBufferSwapped = !m_bBufferSwapped;

}
*/

inline u32 CKernel::BusIORead(u32 address) {

    u32 data;

    switch(address) {

    case UART_CTRL:
        // read UART control register

        // return 0x1 if there are bytes waiting
        data = m_ToSerial.GetCount() > 0;

        break;

    case UART_DATA:
        // read UART data register

        data = 0;
        m_ToSerial.Remove((u8 *)&data);

        if (m_bUartIntActive) {
            m_bUartIntActive = false;
            GPIOInterruptRelease();
        }

        break;

    // case VC_CTRL:

        // data = m_nMode;

        // if (m_bAutoIncrementWrite)
        //     data |= VC_CTRL_AUTO_INCREMENT_WRITE;

        // if (m_bAutoIncrementRead)
        //     data |= VC_CTRL_AUTO_INCREMENT_READ;

        // if (m_bReadyForSwap)
        //     data |= VC_CTRL_READY_FOR_SWAP;

    //     break;

    // case VC_HIGH_ADDR:
        // data = m_nBusRamPtr >> 8;
        // break;

    // case VC_LOW_ADDR:
        // data = m_nBusRamPtr & 0xff;
        // break;

    // case VC_DATA:
        // data = m_pBusRam[m_nBusRamPtr];
        // if (m_bAutoIncrementRead)
        //     m_nBusRamPtr = ++m_nBusRamPtr % RAM_SIZE;
        // break;

    default:

        klog(LogNotice, "IO_READ Address: 0x%x", address);
        data = m_nTestPort;
        break;

    }

    // klog(LogDebug, "IO_READ Address: 0x%x, Data: 0x%x", address, data);

    return data;
}

inline void CKernel::BusIOWrite(u32 address, u8 data) {

    switch(address) {

    // case UART_CTRL:
    //     // write UART control register

    //     klog(LogNotice, "UART control write %u", data);
    //     m_bUartIntEnable = data & UART_INT_ENABLE;

    //     break;

    case UART_DATA:
        // write UART data register
        m_FromSerial.Add(data);

        break;


    case VC_CTRL:

        m_ToDisplay.Add(VC_CMD | data);

        break;

    case VC_DATA:

        m_ToDisplay.Add(data);

        break;

    // case VC_CTRL:
    //     // write Video Control register
    //     {
    //         u8 mode = data & VC_CTRL_MODE_MASK;
    //         m_bAutoIncrementWrite = data & VC_CTRL_AUTO_INCREMENT_WRITE;
    //         m_bAutoIncrementRead = data & VC_CTRL_AUTO_INCREMENT_READ;

    //         switch(mode) {

    //         case MODE_CON:
    //             klog(LogNotice, "Setting mode %u", mode);
    //             m_nMode = mode;

    //             break;

    //         case MODE_256x192:
    //             klog(LogNotice, "Setting mode %u", mode);
    //             m_nMode = mode;
    //             m_nBusRamPtr = 0;

    //             break;

    //         case MODE_256x192_DB:

    //             if (mode == m_nMode) {
    //                 // double buffer swap
    //                 if (m_bReadyForSwap) {
    //                     u8 *tmp = m_pBusRam;
    //                     m_pBusRam = m_pDisRam;
    //                     m_pDisRam = tmp;

    //                     m_bReadyForSwap = false;
    //                     m_nBusRamPtr = 0;

    //                 } else {
    //                     klog(LogWarning, "Bad Swap");
    //                 }
    //             } else {
    //                 // set video mode
    //                 klog(LogNotice, "Setting mode %u", mode);
    //                 m_nMode = mode;
    //                 m_nBusRamPtr = 0;
    //             }

    //             break;

    //         default:
    //             klog(LogWarning, "Bad mode %u", mode);

    //             break;
    //         }
    //     }
    //     break;

    // case VC_HIGH_ADDR:
    //     // set high byte of video address
    //     m_nBusRamPtr = ((data << 8) + (m_nBusRamPtr & 0xff)) % RAM_SIZE;

    //     break;

    // case VC_LOW_ADDR:
    //     // set low byte of video address
    //     m_nBusRamPtr = (m_nBusRamPtr & 0xff00) + data;

    //     break;

    // case VC_DATA:
    //     // write byte to display ram
    //     m_pBusRam[m_nBusRamPtr] = data;
    //     if (m_bAutoIncrementWrite)
    //         m_nBusRamPtr = ++m_nBusRamPtr % RAM_SIZE;

    //     break;

    default:

        // DEBUGING, signal to scope we've seen an unexpected write
        GPIOInterruptRaise();
        GPIOInterruptRelease();

        klog(LogError, "IO_WRITE Address: 0x%x Data: 0x%x", address, data);
        m_nTestPort = data;


        break;

    }

    // klog(LogDebug, "IO_WRITE Address: 0x%x Data: 0x%x", address, data);

}


void CKernel::GPIOInit() {

    // set PWAIT output to 1 to indicate we're not ready yet
    GPIOPWaitBusy();

    // set Interrupt HIGH
    GPIOInterruptRelease();

    // set GPIO pin mode
    write32(ARM_GPIO_GPFSEL0, 0x901); // GPIO 0 output (PWAIT), GPIO 1,4-9 input, GPIO 2,3 I2C
    write32(ARM_GPIO_GPFSEL1, 0x1040000); // GPIO 10-15, 17, 19 input, GPIO 16, 18 output
    write32(ARM_GPIO_GPFSEL2, 0x8000000); // GPIO 20-28 input, GPIO 29 output (ActLED)

    // disable GPIO pullups (see CGPIOPin::SetPullMode), only works for Pi <= 3
    // also see bcm2835-peripherals.pdf page 101
    // there are physical pullups on the I2C pins, so safe to disable here
    write32(ARM_GPIO_GPPUD, 0); // pullups off
    CTimer::SimpleusDelay(5);
    write32(ARM_GPIO_GPPUDCLK0, 0xfffffff); // GPIO 0-27
    CTimer::SimpleusDelay(5);
    write32(ARM_GPIO_GPPUD, 0);
    write32(ARM_GPIO_GPPUDCLK0, 0);

    // mess with slew rates and drive strength (GPIO 0-27)
    // https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#gpio-pads-control
    // 2mA drive strength caues occasional data corruption with bus I/O reads, using 8mA instead
    write32(ARM_PM_PADS0, ARM_PM_PASSWD | ARM_PADS_SLEW | ARM_PADS_HYST | ARM_PADS_DRIVE(1));

}

inline u32 CKernel::GPIORead() {
    return read32(ARM_GPIO_GPLEV0);
}

inline void CKernel::GPIOPWaitReady() {
    write32(ARM_GPIO_GPCLR0, 0x1);
}

inline void CKernel::GPIOPWaitBusy() {
    write32(ARM_GPIO_GPSET0, 0x1);
}

inline void CKernel::GPIOInterruptRaise() {
    write32(ARM_GPIO_GPCLR0, 1 << 18);
}

inline void CKernel::GPIOInterruptRelease() {
    write32(ARM_GPIO_GPSET0, 1 << 18);
}

inline void CKernel::GPIOBreakReset() {
    write32(ARM_GPIO_GPSET0, 1 << 16);
    CTimer::SimpleMsDelay(100);
    write32(ARM_GPIO_GPCLR0, 1 << 16);
}

inline void CKernel::GPIODataOutput(u32 data) {

    /*
      We could release the wait signal at the same time as setting
      the data pins but this creates complications.
      If we set the output mode first then the previous value is outputted,
      then rapidly switched to the new value.
      If we set the output mode second then we're raising PWAIT before
      we're finished so there's a small window for a race condition.
      Playing it safe and doing this in three steps:
      1. set the output values
      2. enable output
      3. raise PWAIT
    */

    data = data << 20;

    u32 data_clear = ~data & DATA_MASK;
    u32 data_set = data & DATA_MASK;

    // set the output pin values
    write32(ARM_GPIO_GPCLR0, data_clear);
    write32(ARM_GPIO_GPSET0, data_set);

    // set data pins to output mode
    write32(ARM_GPIO_GPFSEL2, 0x8249249); // GPIO 20-27, 29 output, GPIO 28 input

}

// set data pins to input mode
inline void CKernel::GPIODataInput() {
    write32(ARM_GPIO_GPFSEL2, 0x8000000); // GPIO 20-28 input, GPIO 29 output
}
