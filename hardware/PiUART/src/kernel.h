#ifndef KERNEL_H
#define KERNEL_H

#include <circle/2dgraphics.h>
#include <circle/devicenameservice.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/koptions.h>
#include <circle/logger.h>
#include <circle/machineinfo.h>
#include <circle/memio.h>
#include <circle/memory.h>
#include <circle/screen.h>
#include <circle/timer.h>
#include <circle/types.h>
#include <circle/util.h>
#include <circle/bcmframebuffer.h>
#include <circle/usb/usbhcidevice.h>
#include <circle/sched/scheduler.h>
#include <circle/net/netsubsystem.h>
#include <display/ssd1306device.h>
#include <fatfs/ff.h>
#include <SDCard/emmc.h>
#include <wlan/bcm4343.h>
#include <wlan/hostap/wpa_supplicant/wpasupplicant.h>

#include "charconv.h"
#include "common.h"
#include "cp437.h"
#include "fonts.h"
#include "graphics.h"
#include "i2clogger.h"
#include "ringbuf.h"
#include "terminalwrapper.h"

#if DEPTH != 8
#error Bad DEPTH
#endif

/*
  GPIO 0 PWAIT (output)
  GPIO 1 PEVENT
  GPIO 2 SDA
  GPIO 3 SCL
  GPIO 18 Interrupt
  GPIO 19 PRDWR
*/

#define PEVENT 0x2
#define PRDWR 0x80000

#define DATA_MASK 0x0ff00000

// see include/circle/bcm2835.h
// GPIO Function Select
// 3 bits per GPIO, 000 = input, 001 = output
// GPIO 0-9
#define ARM_GPIO_GPFSEL0        (ARM_GPIO_BASE + 0x00)
// GPIO 10-19
#define ARM_GPIO_GPFSEL1        (ARM_GPIO_BASE + 0x04)
// GPIO 20-29
#define ARM_GPIO_GPFSEL2        (ARM_GPIO_BASE + 0x08)

// UART registers
#define UART1_CTRL 0    // 0x20
#define UART1_DATA 1    // 0x22
#define UART2_CTRL 2    // 0x24
#define UART2_DATA 3    // 0x26

// Video Control registers
#define VC_CTRL   4     // 0x28
#define VC_PARAM  5     // 0x2A
#define VC_DATA   6     // 0x2C
// RESERVED 7           // 0x2E

// UART bitmaps
#define UART_INT_ENABLE 0x1
#define UART_CRLF       0x2   // 0 == CR, 1 == LF
#define UART_DEL_BS     0x4   // 0 == DEL, 1 == BS
#define UART_DONT_USE   0x20  // MSDOS writes an EOI to the PiUART ctrl port, so don't use bit 5

/*
  VC_CTRL READ:

  bit 7 : busy
  bit 6-0 : reserved

*/

#define VC_CTRL_BUSY 0x80

// OLED I2C display
#define LCD_HEIGHT 32
#define LCD_WIDTH 128
#define LCD_I2C_ADDR 0x3C

// multicore stuff
#define CORE_MAIN 0
#define CORE_DISPLAY 1
#define CORE_GPIO 2

#define RING_BUF_SIZE 262144
#define TERM_BUF_SIZE 256
#define PARAM_BUF_SIZE 32

#define NETWORK_DELAY_US 30000 // 30 ms
#define NETWORK_DELAY_BYTES 132 // one xmodem packet

#define NUM_DISPLAY_MODES 3

enum TDisplayMode { Uninitialised = -1, TerminalMode = 0, GraphicsMode = 1, DebugLogMode = 2 };

class CKernel : public CMultiCoreSupport {
public:
    CKernel(CMemorySystem *pMemorySystem);
    ~CKernel();

    bool Initialize();
    void Run(unsigned nCore);
    void SetConsole(unsigned nConsoleMode);

private:
    // Cores
    void GPIO();
    void Display();
    void Main();

    // Helper functions
    bool DeferredInitialize();

    inline u32 BusIORead(u32 address);
    inline void BusIOWrite(u32 address, u8 data);

    void GPIOInit();
    inline u32 GPIORead();
    inline void GPIOPWaitReady();
    inline void GPIOPWaitBusy();
    inline void GPIOInterruptRaise();
    inline void GPIOInterruptRelease();
    inline void GPIOBreakReset();
    inline void GPIODataOutput(u32 data);
    inline void GPIODataInput();

    // Kernel Options, gives access to command line options ("cmdline.txt")
    CKernelOptions m_KernelOptions;

    // needed by various Cirle internal functions
    CDeviceNameService m_DeviceNameService;

    // allow CPU to run at full speed with thermal throttling
    CCPUThrottle m_CPUThrottle;

    #ifndef NDEBUG
    // handle exceptions more elegantly
    CExceptionHandler m_ExceptionHandler;
    #endif

    // make interrupts work
    CInterruptSystem m_InterruptSystem;

    // system timer
    CTimer m_Timer;

    #ifndef NDEBUG
    // system logger
    CLogger m_Logger;
    #endif

    // I2C master
    CI2CMaster m_I2C;

    #ifndef NDEBUG
    // I2C Logger
    CI2CLogger m_I2CLogger;
    #endif

    // Simple multi-tasking that runs on Core 0
    CScheduler m_Scheduler;

    // HDMI display
    unsigned m_nScreenWidth;
    unsigned m_nScreenHeight;

    CRingBuf<u16> m_ToDisplay; // commands and data for the display

    enum TDisplayMode m_nDisplayMode;
    volatile TDisplayMode m_nNextDisplayMode;

    CTerminalWrapper *m_pTerminal;
    CTerminalWrapper *m_pDebugLog;
    CGraphics *m_pGraphics;

    volatile bool m_bDisplayBusy;

    // OLED/LCD display
    CSSD1306Device m_LCD;

    // USB magic
    CUSBHCIDevice m_USBHCI;

    // Bits to read the WPA Supplicant config file from the SD card
    CEMMCDevice m_EMMC;
    FATFS m_FileSystem;

    // WLAN/Networking bits
    CBcm4343Device m_WLAN;
    CNetSubSystem m_Net;
    CWPASupplicant m_WPASupplicant;

    // UART
    bool m_bUartIntEnable;
    bool m_bUartIntActive;

    CCharConv m_CharConv;

    // ring buffers
    CRingBuf<u8> m_ToSerial_UART1; // data for the UART to output
    CRingBuf<u8> m_FromSerial_UART1; // data received by the UART
    CRingBuf<u8> m_ToTerminal; // data for the terminal to display
    CRingBuf<u8> m_ToNetwork; // data for the network to send

    CRingBuf<u8> m_ToSerial_UART2; // data for the UART to output
    CRingBuf<u8> m_FromSerial_UART2; // data received by the UART
    u8 m_nTestPort;

};

#endif
