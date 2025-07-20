#ifndef KEYBOARDTASK_H
#define KEYBOARDTASK_H

#include <circle/sched/task.h>
#include <circle/usb/usbhcidevice.h>
#include <circle/usb/usbkeyboard.h>

#include "common.h"
#include "ringbuf.h"
#include "kernel.h"

class CKeyboardTask : public CTask {
public:
    CKeyboardTask(CUSBHCIDevice *pUSBHCI, CRingBuf<u8> *pKeyBuf, CKernel *pKernel);
    ~CKeyboardTask();
    void Run();
    
private:
    // Static functions
    static void KeyboardRemovedHandler(CDevice *pDevice, void *pContext);
    static void KeyPressedHandler(const char *pString);
    static void SelectConsoleHandler(unsigned nConsole);

    CUSBHCIDevice *m_pUSBHCI;
    CUSBKeyboardDevice * volatile m_pKeyboard;
    CRingBuf<u8> *m_pKeyBuf;
    CKernel *m_pKernel;

    static CKeyboardTask *s_pThis;
};

#endif

