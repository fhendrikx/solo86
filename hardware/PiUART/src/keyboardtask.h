#ifndef KEYBOARDTASK_H
#define KEYBOARDTASK_H

#include <circle/sched/task.h>
#include <circle/usb/usbhcidevice.h>
#include <circle/usb/usbkeyboard.h>

#include "common.h"
#include "ringbuf.h"
#include "kernel.h"

// https://stanislavs.org/helppc/int_16-2.html
// BIOS int 16,2
// AL = BIOS keyboard flags (located in BIOS Data Area 40:17)

// 	|7|6|5|4|3|2|1|0|  AL or BIOS Data Area 40:17
// 	 | | | | | | | `---- right shift key depressed
// 	 | | | | | | `----- left shift key depressed
// 	 | | | | | `------ CTRL key depressed
// 	 | | | | `------- ALT key depressed
// 	 | | | `-------- scroll-lock is active
// 	 | | `--------- num-lock is active
// 	 | `---------- caps-lock is active
// 	 `----------- insert is active

#define KEYB_FLAGS_INS          0x80
#define KEYB_FLAGS_CAPS         0x40
#define KEYB_FLAGS_NUM          0x20
#define KEYB_FLAGS_SCRL         0x10
#define KEYB_FLAGS_ALT          0x08
#define KEYB_FLAGS_CTRL         0x04
#define KEYB_FLAGS_LEFT_SHIFT   0x02
#define KEYB_FLAGS_RIGHT_SHIFT  0x01

class CKeyboardTask : public CTask {
public:
    CKeyboardTask(CUSBHCIDevice *pUSBHCI, CRingBuf<u16> *pKeyBuf, CKernel *pKernel, CCharConv *pCharConv);
    ~CKeyboardTask();
    void Run();
    
private:
    // Static functions
    static u8 MakeKeyboardFlags(unsigned char ucModifiers);
    static void KeyboardRemovedHandler(CDevice *pDevice, void *pContext);
    static void KeyPressedHandler(const char *pString);
    static void KeyRawHandler(unsigned char ucModifiers, const unsigned char RawKeys[6]);
    static void SelectConsoleHandler(unsigned nConsole);

    CUSBHCIDevice *m_pUSBHCI;
    CUSBKeyboardDevice * volatile m_pKeyboard;
    CRingBuf<u16> *m_pKeyBuf;
    CKernel *m_pKernel;
    CCharConv *m_pCharConv;

    static CKeyboardTask *s_pThis;
};

#endif

