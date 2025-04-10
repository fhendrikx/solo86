#ifndef KERNEL_H
#define KERNEL_H

#include <circle/machineinfo.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/bcmframebuffer.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/types.h>
#include <circle/multicore.h>
#include <circle/memory.h>
#include <circle/util.h>
#include <circle/memio.h>
#include <circle/usb/usbhcidevice.h>
#include <circle/sched/scheduler.h>
#include <circle/net/netsubsystem.h>
#include <display/ssd1306device.h>
#include <SDCard/emmc.h>
#include <fatfs/ff.h>
#include <wlan/bcm4343.h>
#include <wlan/hostap/wpa_supplicant/wpasupplicant.h>

#include "common.h"
#include "i2clogger.h"
#include "ringbuf.h"
#include "terminal.h"


/*
  GPIO 0 PWAIT (output)
  GPIO 1 PEVENT
  GPIO 2 SDA
  GPIO 3 SCL
  GPIO 18 Interrupt
  GPIO 19 PRDWR
*/

#define PWAIT 0x1
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

#define MODE_CON 0
#define MODE_256x192 1
#define MODE_256x192_DB 2

// UART registers
#define UART_CTRL 0     // 0x20
#define UART_DATA 1     // 0x22
// RESERVED 2              0x24
// RESERVED 3              0x26

// Video Control registers
#define VC_CTRL 4       // 0x28
#define VC_HIGH_ADDR 5  // 0x2A
#define VC_LOW_ADDR 6   // 0x2C
#define VC_DATA 7       // 0x2E

// UART bitmaps
#define UART_INT_ENABLE 0x1

/*
  VC_CTRL WRITE:

  bit 7
  bit 6
  bit 5 : auto increment on read
  bit 4 : auto increment on write
  bit 3-0: video mode, console, 256x192, etc

  VC_CTRL READ:

  bit 7 : ready for swap (double buffer)
  bit 6 :
  bit 5 : auto increment on read
  bit 4 : auto increment on write
  bit 3-0: video mode, console, 256x192, etc

*/

#define VC_CTRL_MODE_MASK 0x0F
#define VC_CTRL_AUTO_INCREMENT_WRITE 0x10
#define VC_CTRL_AUTO_INCREMENT_READ 0x20
#define VC_CTRL_READY_FOR_SWAP 0x80

// OLED I2C display
#define LCD_HEIGHT 32
#define LCD_WIDTH 128
#define LCD_I2C_ADDR 0x3C

// multicore stuff
#define CORE_MAIN 0
#define CORE_DISPLAY 1
#define CORE_GPIO 2

#define RAM_SIZE 49152 // 256 x 192

#define RING_BUF_SIZE 262144

#define NETWORK_DELAY_US 30000 // 30 ms
#define NETWORK_DELAY_BYTES 132 // one xmodem packet

class CKernel : public CMultiCoreSupport {
public:
    CKernel(CMemorySystem *pMemorySystem);
    ~CKernel();

    bool Initialize();
    void Run(unsigned nCore);

private:
    // Cores
    void GPIO();
    void Display();
    void Main();

    // Helper functions
    bool DeferredInitialize();
    void UpdateMode256x192(u8 *pRam);
    bool InitFB(unsigned nWidth, unsigned nHeight);
    bool ResizeFB(unsigned nWidth, unsigned nHeight, unsigned nTargetWidth, unsigned nTargetHeight);
    void UpdateFB();

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

    // Command line options ("cmdline.txt")
    CKernelOptions m_CmdLine;

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
    CBcmFrameBuffer *m_pFrameBuffer;
    TPixel *m_pBuffer;
    TPixel *m_pBuffer0;
    TPixel *m_pBuffer1;
    bool m_bBufferSwapped;

    // Terminal emulator
    CTerminal m_Terminal;

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

    // ring buffers
    CRingBuf m_ToSerial; // data for the UART to output
    CRingBuf m_ToTerminal; // data for the terminal to display
    CRingBuf m_ToNetwork; // data for the network to send

    unsigned m_nLogLevel;
    u8 m_pRam[RAM_SIZE * 2];

    u8 *m_pBusRam;
    u8 *m_pDisRam;
    volatile u8 m_nMode;
    u8 m_nPrevMode;
    volatile u32 m_nBusRamPtr;
    volatile bool m_bReadyForSwap;
    volatile bool m_bAutoIncrementWrite;
    volatile bool m_bAutoIncrementRead;

    bool m_bUartIntEnable;
    bool m_bUartIntActive;

    unsigned m_nScreenWidth;
    unsigned m_nScreenHeight;
    unsigned m_nScreenPitch;

    u8 m_nTestPort;

};

#endif
