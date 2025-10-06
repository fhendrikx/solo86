#include "kernel.h"
#include "serialtask.h"
#include "keyboardtask.h"
#include "tcplistenertask.h"

#define DRIVE           "SD:"
#define FIRMWARE_PATH   DRIVE "/firmware/"              // firmware files must be provided here
#define CONFIG_FILE     DRIVE "/wpa_supplicant.conf"

const char hostname[] = "PiUART";

LOGMODULE("kernel");

CKernel::CKernel(CMemorySystem *pMemorySystem)
    : CMultiCoreSupport(pMemorySystem),
      m_Timer(&m_InterruptSystem),
#ifndef NDEBUG
      m_Logger(m_KernelOptions.GetLogLevel(), &m_Timer),
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

    m_nScreenWidth = m_KernelOptions.GetWidth();
    m_nScreenHeight = m_KernelOptions.GetHeight();
    m_nDisplayMode = Uninitialised;
    m_nNextDisplayMode = TerminalMode;
    m_bDisplayBusy = false;

    m_pTerminal = NULL;
    m_pDebugLog = NULL;
    m_pGraphics = NULL;

    m_bUartIntEnable = false;
    m_bUartIntActive = false;

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
    } else {
        klog(LogNotice, "USBHCI init complete");
    }

    // create Graphics before we start multi-core in case GPIO tries to read video memory
    m_pGraphics = new CGraphics();
    if (!m_pGraphics->Initialize()) {
        klog(LogError, "Graphics init failed");
        return false;
    } else {
        klog(LogNotice, "Graphics init complete");
    }

    // all other initialisation is deferred and handled by the MAIN core

    return CMultiCoreSupport::Initialize();

}

bool CKernel::DeferredInitialize() {

    if (!m_LCD.Initialize()) {
        klog(LogError, "LCD init failed");
        return false;
    } else {
        klog(LogNotice, "LCD init complete");
    }

    // put something on the LCD display so we know it's working
    m_LCD.Write(VERSION "\n", strlen(VERSION) + 1);

    if (!m_EMMC.Initialize()) {
        klog(LogError, "EMMC init failed");
        return false;
    } else {
        klog(LogNotice, "EMMC init complete");
    }

    if (f_mount (&m_FileSystem, DRIVE, 1) != FR_OK) {
        klog(LogError, "Cannot mount drive: %s", DRIVE);
        return false;
    } else {
        klog(LogNotice, "Filesystem %s mounted", DRIVE);
    }

    if (!m_WLAN.Initialize()) {
        klog(LogError, "WLAN init failed");
        return false;
    } else {
        klog(LogNotice, "WLAN init complete");
    }

    if (!m_Net.Initialize(false)) {
        klog(LogError, "Net init failed");
        return false;
    } else {
        klog(LogNotice, "Net init complete");
    }

    if (!m_WPASupplicant.Initialize()) {
        klog(LogError, "WPA Supplicant init failed");
        return false;
    } else {
        klog(LogNotice, "WPA Supplicant init complete");
    }

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

    if (nConsoleMode < NUM_DISPLAY_MODES) {
        klog(LogDebug, "SetConsole %d", nConsoleMode);
        m_nNextDisplayMode = (enum TDisplayMode) nConsoleMode;
    } else {
        klog(LogDebug, "SetConsole, ignoring %d", nConsoleMode);
    }

    return;
}

//
//  Cores
//

void CKernel::Display() {

    //
    // create a temporary frame buffer to determine the screen resolution
    //

    CBcmFrameBuffer *pTmpFrameBuffer = new CBcmFrameBuffer(m_nScreenWidth, m_nScreenHeight, DEPTH);

    if (pTmpFrameBuffer == NULL || !pTmpFrameBuffer->Initialize()) {
        // framebuffer object is unusable, bomb out
        klog(LogPanic, "Failed to create temp frame buffer");
        CMultiCoreSupport::HaltAll();
    }

    m_nScreenWidth = pTmpFrameBuffer->GetWidth();
    m_nScreenHeight = pTmpFrameBuffer->GetHeight();

    klog(LogNotice, "Display %ux%u", m_nScreenWidth, m_nScreenHeight);

    delete pTmpFrameBuffer;

    //
    // figure out which fonts to use based on the screen resolution
    //

    const TFont *pTerminalFont;
    TFont TLogFont;

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

    //
    // Setup the display objects
    //

    m_pTerminal = new CTerminalWrapper("Terminal", m_nScreenWidth, m_nScreenHeight, *pTerminalFont);
    if (m_pTerminal == NULL || !m_pTerminal->Initialize()) {
        klog(LogPanic, "TerminalWrapper init failed");
        CMultiCoreSupport::HaltAll();
    }

    m_pDebugLog = new CTerminalWrapper("DebugLog", m_nScreenWidth, m_nScreenHeight, TLogFont);
    if (m_pDebugLog == NULL || !m_pDebugLog->Initialize()) {
        klog(LogPanic, "TerminalWrapper init failed");
        CMultiCoreSupport::HaltAll();
    }

    #ifdef NDEBUG
    const char *NoDebugMsg = "Debugging disabled\n";
    m_pDebugLog->Write(NoDebugMsg, strlen(NoDebugMsg));
    #endif

    u8 TermBuffer[TERM_BUF_SIZE];
    u8 ParamBuffer[PARAM_BUF_SIZE];
    unsigned nParamBufferIndex = 0;

    //
    // main display loop
    //

    while(true) {

        // grab a copy of NextDisplayMode is it can't change while we're using it
        enum TDisplayMode nNextDisplayMode = m_nNextDisplayMode;

        if (nNextDisplayMode != m_nDisplayMode) {

            // clean up current mode
            switch(m_nDisplayMode) {

                case Uninitialised:
                    // nothing to do
                break;

                case TerminalMode:
                    if (!m_pTerminal->Deactivate()) {
                        CMultiCoreSupport::HaltAll();
                    }
                break;

                case GraphicsMode:
                    if (!m_pGraphics->Deactivate()) {
                        CMultiCoreSupport::HaltAll();
                    }
                break;

                case DebugLogMode:
                    if (!m_pDebugLog->Deactivate()) {
                        CMultiCoreSupport::HaltAll();
                    }
                break;
            }

            // setup next mode
            switch(nNextDisplayMode) {

                case Uninitialised:
                    // klog(LogPanic, "Trying to switch to Uninitialised display mode");
                    // CMultiCoreSupport::HaltAll();
                break;

                case TerminalMode:
                    if (!m_pTerminal->Activate()) {
                        CMultiCoreSupport::HaltAll();
                    }
                break;

                case GraphicsMode:
                    if (!m_pGraphics->Activate()) {
                        CMultiCoreSupport::HaltAll();
                    }
                break;

                case DebugLogMode:
                    if (!m_pDebugLog->Activate()) {
                        CMultiCoreSupport::HaltAll();
                    }
                break;
            }

            m_nDisplayMode = nNextDisplayMode;

        }

        // update the terminal
        if (m_ToTerminal.GetCount()) {

            // copy chars into a buffer so we can handle multiple chars per Write()
            // this is because each call to Write() results in a screen update so it's
            // more efficient to bundle the chars together
            int nRemoved = m_ToTerminal.Remove(TermBuffer, TERM_BUF_SIZE);

            m_pTerminal->Write((char *)TermBuffer, nRemoved);

        }


        // update the debug log
        #ifndef NDEBUG
        int numBytes = m_Logger.Read(TermBuffer, TERM_BUF_SIZE, true);

        if (numBytes > 0)
            m_pDebugLog->Write(TermBuffer, numBytes);
        #endif

        // process commands and data
        if (m_ToDisplay.GetCount()) {

            u16 value = 0;
            u16 type = 0;

            m_bDisplayBusy = true;

            m_ToDisplay.Remove(&value);

            type = value >> 8;
            value &= 0xFF;

            switch(type) {

                case VC_CTRL:

                    switch(value) {

                        case 0x00:
                            m_nNextDisplayMode = TerminalMode;
                        break;

                        case 0x01:
                            m_nNextDisplayMode = GraphicsMode;
                        break;

                        case 0x02:
                            m_nNextDisplayMode = DebugLogMode;
                        break;

                        // 0x03 -> 0x3F reserved

                        // case 0x10:
                            // RESET
                            // empty m_ToDisplay
                            // nParamBufferIndex = 0
                            // clear terminal
                            // clear debug log (?)
                            // reset graphics (can't delete, GPIO read)
                        // break;

                        default:

                            if (value >= 0x40) {
                                m_pGraphics->Command(value, ParamBuffer, nParamBufferIndex);
                                nParamBufferIndex = 0;
                            } else {
                                klog(LogWarning, "Undefined VC_CTRL %u", value);
                            }

                        break;

                    }

                break;

                case VC_PARAM:
                    if (nParamBufferIndex < PARAM_BUF_SIZE) {
                        ParamBuffer[nParamBufferIndex++] = value;
                    } else {
                        klog(LogWarning, "Param Buffer Full");
                    }
                break;

                case VC_DATA:
                    m_pGraphics->MemWrite(value);
                break;

                default:
                    klog(LogWarning, "Unknown Type");
                break;

            }

            m_bDisplayBusy = false;

        }

        // CTimer::SimpleusDelay(1);

    }

}

void CKernel::GPIO() {

    // setup the GPIO pins
    GPIOInit();

    // set PWAIT output to 0 to indicate we're ready
    GPIOPWaitReady();

    // main GPIO loop
    while (true) {

        // raise an interrupt if necessary
        if (m_bUartIntEnable and not m_bUartIntActive and m_ToSerial.GetCount() > 0) {

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

    // launch the task that moves serial data from the GPIO to the terminal and network
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

            data = 0;

            if (m_bDisplayBusy || m_ToDisplay.GetCount() > 0) {
                data |= VC_CTRL_BUSY;
            }

        break;

        case VC_PARAM:

            data = 0;

        break;

        case VC_DATA:

            data = m_pGraphics->MemRead();

        break;

        default:

            klog(LogWarning, "IO_READ Address: 0x%x", address);
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

            klog(LogError, "UART control write %u", data);
            m_bUartIntEnable = data & UART_INT_ENABLE;

        break;

        case UART_DATA:
            // write UART data register
            m_FromSerial.Add(data);

        break;

        case VC_CTRL:
        case VC_PARAM:
        case VC_DATA:

            // use AddSafe otherwise data may be lost from the ring buffer
            // lost data can result in commands with the wrong parameters
            // or missing commands
            m_ToDisplay.AddSafe((address << 8) | data);

        break;

        default:

            // DEBUGING, signal to scope we've seen an unexpected write
            // GPIOInterruptRaise();
            // GPIOInterruptRelease();

            klog(LogWarning, "IO_WRITE Address: 0x%x Data: 0x%x", address, data);
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
    write32(ARM_GPIO_GPSET0, 1 << 18);
}

inline void CKernel::GPIOInterruptRelease() {
    write32(ARM_GPIO_GPCLR0, 1 << 18);
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
