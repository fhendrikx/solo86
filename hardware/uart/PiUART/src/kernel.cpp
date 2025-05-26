#include "kernel.h"
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
      m_LCD(LCD_WIDTH, LCD_HEIGHT, &m_I2C, LCD_I2C_ADDR, false, false),
      m_USBHCI(&m_InterruptSystem, &m_Timer, true),
      m_EMMC(&m_InterruptSystem, &m_Timer),
      m_WLAN(FIRMWARE_PATH),
      m_Net(0, 0, 0, 0, hostname, NetDeviceTypeWLAN),
      m_WPASupplicant(CONFIG_FILE),
      m_ToSerial(RING_BUF_SIZE),
      m_ToTerminal(RING_BUF_SIZE),
      m_ToNetwork(RING_BUF_SIZE)
{

    m_nLogLevel = m_CmdLine.GetLogLevel();
    // m_pFrameBuffer = NULL;
    m_pTerminal = NULL;
    m_pBuffer = NULL;
    m_pBuffer0 = NULL;
    m_pBuffer1 = NULL;
    m_bBufferSwapped = true;
    m_bDisplayInitComplete = false;

    m_nMode = MODE_CON;
    m_nPrevMode = MODE_CON;
    m_nBusRamPtr = 0;
    m_bReadyForSwap = true;
    m_bAutoIncrementWrite = false;
    m_bAutoIncrementRead = false;

    m_bUartIntEnable = false;
    m_bUartIntActive = false;

    memset(m_pRam, 0, RAM_SIZE * 2);
    m_pBusRam = m_pRam;
    m_pDisRam = &m_pRam[RAM_SIZE];

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
    klog(LogNotice, "Temperature: %u", CCPUThrottle::Get()->GetTemperature());
    klog(LogNotice, "Clock: %u MHz", CCPUThrottle::Get()->GetClockRate() / 1000000);



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

    case CORE_SHOW:

        klog(LogNotice, "CKernel::Run CORE_SHOW");
        Show();

        break;

    default:
        klog(LogNotice, "CKernel::Run default");
        break;

    }

}

//
//  Cores
//

void CKernel::Show() {

    /*

    while(true) {

        if (m_bDisplayInitComplete) {

            if (m_pFrameBuffer != NULL)
                m_pFrameBuffer->WaitForVerticalSync();

            unsigned nStartTime = CTimer::GetClockTicks();

            if (m_pTerminalDevice != NULL)
                m_pTerminalDevice->Update(0);
            
            unsigned nEndTime = CTimer::GetClockTicks();
            if ((nEndTime - nStartTime) > 10)
                klog(LogNotice, "Update time %u", nEndTime - nStartTime);
        }

        // } else {
        //     klog(LogNotice, "Show Waiting");
        //     CTimer::SimpleMsDelay(500);

        // }
    }

    */

}

void CKernel::Display() {

    m_pTerminal = new CScreenDevice(m_CmdLine.GetWidth(), m_CmdLine.GetHeight(),
                                    DEFAULT_FONT, CCharGenerator::FontFlagsNone);

    m_pTerminal->Initialize();
    // TODO error check

    m_nScreenWidth = m_pTerminal->GetWidth();
    m_nScreenHeight = m_pTerminal->GetHeight();
    // m_nScreenPitch = m_pFrameBuffer->GetPitch();

    klog(LogNotice, "ScreenWidth %u", m_nScreenWidth);
    klog(LogNotice, "ScreenHeight %u", m_nScreenHeight);

    // m_pTerminal->Update(1);

    klog(LogNotice, "Cols %u", m_pTerminal->GetColumns());
    klog(LogNotice, "Rows %u", m_pTerminal->GetRows());

    m_bDisplayInitComplete = true;

    while(true) {

        int peak_waiting = 0;

        while (m_ToTerminal.GetCount()) {

            int waiting = m_ToTerminal.GetCount();
            if (waiting > peak_waiting)
                peak_waiting = waiting;

            //klog(LogNotice, "Char waiting %u", m_ToTerminal.GetCount());

            // remove 16 chars at a time from the ring buffer
            // we could do more, but then we'd be holding the lock for longer
            u8 buf[256];

            // unsigned nStartTime = CTimer::GetClockTicks();
            int removed = m_ToTerminal.Remove(buf, 256);
 
            // unsigned nEndTime = CTimer::GetClockTicks();
            // klog(LogNotice, "Lock time %u", nEndTime - nStartTime);

            m_pTerminal->Write((char *)buf, removed);

        }

        if (peak_waiting) {
            klog(LogNotice, "Peak %u", peak_waiting);
        }

        CTimer::SimpleusDelay(1);

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
                    CMultiCoreSupport::HaltAll();
                }

                break;

            case MODE_256x192:
            case MODE_256x192_DB:

                if (ResizeFB(m_nScreenWidth, m_nScreenHeight, 256, 192)) {

                    klog(LogNotice, "Set FB to %ux%u", m_pFrameBuffer->GetWidth(), m_pFrameBuffer->GetHeight());

                } else {
                    // framebuffer object is unusable, bomb out
                    klog(LogPanic, "Mode switch failed to resize FB");
                    CMultiCoreSupport::HaltAll();
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

    // finish the rest of the initialisation
    if (!DeferredInitialize())
        return;

    klog(LogNotice, "Deferred init complete");

    // launch the task that looks after the USB keyboard
    new CKeyboardTask(&m_USBHCI, &m_ToSerial);

    // dodgy hack, stop new tasks from running so the DHCP process doesn't start
    // also interferes with the netphy task
    m_Scheduler.SuspendNewTasks();

    klog(LogNotice, "WPA waiting for connection");

    // wait for WPA to become connected
    while(!m_WPASupplicant.IsConnected()) {
        m_Scheduler.MsSleep(100);
    }

    klog(LogNotice, "WPA connected");

    // allow dhcp to run now
    m_Scheduler.ResumeNewTasks();

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

void CKernel::UpdateMode256x192(u8 *pRam) {

    //unsigned nStartTime = CTimer::GetClockTicks();

    /*

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

    */

    //unsigned nEndTime = CTimer::GetClockTicks();
    //klog(LogNotice, "frame time %u", nEndTime - nStartTime);

}

bool CKernel::InitFB(unsigned nWidth, unsigned nHeight) {

    /*
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

    */

    return true;

}


// try and determine the best resolution for the frame buffer given the display resolution
// and the target resolution
bool CKernel::ResizeFB(unsigned nWidth, unsigned nHeight, unsigned nTargetWidth, unsigned nTargetHeight) {

    /*
    unsigned nNextWidth = nWidth >> 1;
    unsigned nNextHeight = nHeight >> 1;

    unsigned nHorizMult = nNextWidth / nTargetWidth;
    unsigned nVertMult = nNextHeight / nTargetHeight;
    unsigned nMult = min(nHorizMult, nVertMult);

    unsigned nHorizGutter = (nNextWidth - (nMult * nTargetWidth)) / 2;
    unsigned nVertGutter = (nNextHeight - (nMult * nTargetHeight)) / 2;

    unsigned nHorizGutterRatio = (nHorizGutter * 100) / nNextWidth;
    unsigned nVertGutterRatio = (nVertGutter * 100) / nNextHeight;

    klog(LogDebug, "ResizeFB %u %u %u %u", nWidth, nHeight, nTargetWidth, nTargetHeight);

    bool bModeSet = false;

    // if the next size is greater than the target and the gutters are less than 10%
    // then try the next size
    if ((nNextWidth >= nTargetWidth) and nNextHeight >= nTargetHeight and
        min(nHorizGutterRatio, nVertGutterRatio) <= 10) {

        bModeSet = ResizeFB(nNextWidth, nNextHeight, nTargetWidth, nTargetHeight);

    }

    if (not bModeSet) {
        bModeSet = InitFB(nWidth, nHeight);
    }

    return bModeSet;

    */

    return false;

}

void CKernel::UpdateFB() {
/*
    m_pFrameBuffer->SetVirtualOffset(0, m_bBufferSwapped ? m_pFrameBuffer->GetHeight() : 0);
    m_pBuffer = m_bBufferSwapped ? m_pBuffer0 : m_pBuffer1;
    m_pFrameBuffer->WaitForVerticalSync();
    m_bBufferSwapped = !m_bBufferSwapped;
*/

}

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

    case VC_CTRL:

        data = m_nMode;

        if (m_bAutoIncrementWrite)
            data |= VC_CTRL_AUTO_INCREMENT_WRITE;

        if (m_bAutoIncrementRead)
            data |= VC_CTRL_AUTO_INCREMENT_READ;

        if (m_bReadyForSwap)
            data |= VC_CTRL_READY_FOR_SWAP;

        break;

    case VC_HIGH_ADDR:
        data = m_nBusRamPtr >> 8;
        break;

    case VC_LOW_ADDR:
        data = m_nBusRamPtr & 0xff;
        break;

    case VC_DATA:
        data = m_pBusRam[m_nBusRamPtr];
        if (m_bAutoIncrementRead)
            m_nBusRamPtr = ++m_nBusRamPtr % RAM_SIZE;
        break;

    default:

        data = m_nTestPort;
        break;

    }

    // klog(LogDebug, "IO_READ Address: 0x%x, Data: 0x%x", address, data);

    return data;
}

inline void CKernel::BusIOWrite(u32 address, u8 data) {

    switch(address) {

    case UART_CTRL:
        // write UART control register

        klog(LogNotice, "UART control write %u", data);
        m_bUartIntEnable = data & UART_INT_ENABLE;

        break;

    case UART_DATA:
        // write UART data register
        m_ToTerminal.Add(data);
        m_ToNetwork.Add(data);

        break;

    case VC_CTRL:
        // write Video Control register
        {
            u8 mode = data & VC_CTRL_MODE_MASK;
            m_bAutoIncrementWrite = data & VC_CTRL_AUTO_INCREMENT_WRITE;
            m_bAutoIncrementRead = data & VC_CTRL_AUTO_INCREMENT_READ;

            switch(mode) {

            case MODE_CON:
                klog(LogNotice, "Setting mode %u", mode);
                m_nMode = mode;

                break;

            case MODE_256x192:
                klog(LogNotice, "Setting mode %u", mode);
                m_nMode = mode;
                m_nBusRamPtr = 0;

                break;

            case MODE_256x192_DB:

                if (mode == m_nMode) {
                    // double buffer swap
                    if (m_bReadyForSwap) {
                        u8 *tmp = m_pBusRam;
                        m_pBusRam = m_pDisRam;
                        m_pDisRam = tmp;

                        m_bReadyForSwap = false;
                        m_nBusRamPtr = 0;

                    } else {
                        klog(LogWarning, "Bad Swap");
                    }
                } else {
                    // set video mode
                    klog(LogNotice, "Setting mode %u", mode);
                    m_nMode = mode;
                    m_nBusRamPtr = 0;
                }

                break;

            default:
                klog(LogWarning, "Bad mode %u", mode);

                break;
            }
        }
        break;

    case VC_HIGH_ADDR:
        // set high byte of video address
        m_nBusRamPtr = ((data << 8) + (m_nBusRamPtr & 0xff)) % RAM_SIZE;

        break;

    case VC_LOW_ADDR:
        // set low byte of video address
        m_nBusRamPtr = (m_nBusRamPtr & 0xff00) + data;

        break;

    case VC_DATA:
        // write byte to display ram
        m_pBusRam[m_nBusRamPtr] = data;
        if (m_bAutoIncrementWrite)
            m_nBusRamPtr = ++m_nBusRamPtr % RAM_SIZE;

        break;

    default:

        klog(LogNotice, "default write Address: 0x%x Data: 0x%x", address, data);
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
