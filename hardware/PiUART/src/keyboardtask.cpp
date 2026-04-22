#include "keyboardtask.h"

#include <circle/devicenameservice.h>
#include <circle/sched/scheduler.h>

LOGMODULE("keyboardtask");

CKeyboardTask *CKeyboardTask::s_pThis = NULL;

CKeyboardTask::CKeyboardTask(CUSBHCIDevice *pUSBHCI, CRingBuf<u16> *pKeyBuf, CKernel *pKernel, CCharConv *pCharConv) {

    SetName("keyboardtask");

    m_pUSBHCI = pUSBHCI;
    m_pKeyboard = NULL;
    m_pKeyBuf = pKeyBuf;
    m_pKernel = pKernel;
    m_pCharConv = pCharConv;
    s_pThis = this;
}

CKeyboardTask::~CKeyboardTask() {}

void CKeyboardTask::Run() {

    klog(LogNotice, "Starting Keyboard Task");

    while(true) {

        bool bUpdated = m_pUSBHCI->UpdatePlugAndPlay();

        if (bUpdated and m_pKeyboard == NULL) {

            m_pKeyboard = (CUSBKeyboardDevice *) CDeviceNameService::Get()->GetDevice("ukbd1", FALSE);

            if (m_pKeyboard != NULL) {

                klog(LogNotice, "Keyboard updated");

                m_pKeyboard->RegisterRemovedHandler(KeyboardRemovedHandler);
                m_pKeyboard->RegisterKeyPressedHandler(KeyPressedHandler);
                m_pKeyboard->RegisterSelectConsoleHandler(SelectConsoleHandler);
                m_pKeyboard->RegisterKeyStatusHandlerRaw(KeyRawHandler, true);

            }

        }

        if (m_pKeyboard != NULL)
            m_pKeyboard->UpdateLEDs();

        CScheduler::Get()->MsSleep(100);

    }

}

//
//  Static functions
//

void CKeyboardTask::KeyboardRemovedHandler (CDevice *pDevice, void *pContext) {

    assert(s_pThis != NULL);

    klog(LogNotice, "Keyboard removed");
    s_pThis->m_pKeyboard = NULL;

}

void CKeyboardTask::KeyPressedHandler (const char *pString) {

    assert(s_pThis != NULL);

    char c = *pString;

    // only process plain chars (e.g. skip escaped strings)
    // the complex keystrokes will be done by the raw handler so we can
    // control the sequences generated
    if (strlen(pString) == 1 and (c & 0x80) == 0x00) {

        if (c >= 0x20 and c < 0x7f) {
            klog(LogNotice, "KeyPress: %02x '%c'", c, c);
        } else {
            klog(LogNotice, "KeyPress: %02x", c);
        }

        // keyboard sends \n for enter
        // keyboard sends DEL for backspace
        // convert CR/LF, and DEL/^H, etc
        c = s_pThis->m_pCharConv->Convert(c);

        klog(LogDebug, "Translated byte: 0x%x", c);

        u16 s = s_pThis->m_pCharConv->ScanCode(c);

        s_pThis->m_pCharConv->AddSafe(s_pThis->m_pKeyBuf, s);

    }

}

// Modifiers (include/circle/usb/usbhid.h)
// #define LCTRL           (1 << 0)
// #define LSHIFT          (1 << 1)
// #define ALT             (1 << 2)
// #define LWIN            (1 << 3)
// #define RCTRL           (1 << 4)
// #define RSHIFT          (1 << 5)
// #define ALTGR           (1 << 6)
// #define RWIN            (1 << 7)

u8 CKeyboardTask::MakeKeyboardFlags(unsigned char ucModifiers) {

    u8 nKeyboardFlags = 0;

    if (ucModifiers & LSHIFT) {
        nKeyboardFlags |= KEYB_FLAGS_LEFT_SHIFT;
    } 

    if (ucModifiers & RSHIFT) {
        nKeyboardFlags |= KEYB_FLAGS_RIGHT_SHIFT;
    }

    if ((ucModifiers & LCTRL) or (ucModifiers & RCTRL)) {
        nKeyboardFlags |= KEYB_FLAGS_CTRL;
    }

    if ((ucModifiers & ALT) or (ucModifiers & ALTGR)) {
        nKeyboardFlags |= KEYB_FLAGS_ALT;
    }

    return nKeyboardFlags;

}

void CKeyboardTask::KeyRawHandler (unsigned char ucModifiers, const unsigned char RawKeys[6]) {

    klog(LogNotice, "KeyRaw: %02x [%02x %02x %02x %02x %02x %02x]", ucModifiers,
        RawKeys[0], RawKeys[1], RawKeys[2], RawKeys[3], RawKeys[4], RawKeys[5]);

    u8 nPrevKeyboardFlags = s_pThis->m_pKernel->GetKeyboardFlags();
    u8 nKeyboardFlags = MakeKeyboardFlags(ucModifiers);

    if (nKeyboardFlags != nPrevKeyboardFlags) {
        s_pThis->m_pKernel->SetKeyboardFlags(nKeyboardFlags);
        s_pThis->m_pKernel->RequestKeyboardInterrupt();
    }

    u8 c = RawKeys[0];

    // only deal with key presses, ignore key releases
    // only consider single key presses, no simultaneous key presses
    if (c != 0) {

        u16 s = 0;

        // see lib/input/keymap_us.h
        switch(ucModifiers) {
            case 0:             // plain key press, no modifiers
                switch(c) {
                    case 0x3A:  // F1
                        s = 0x3B00;
                    break;
                    case 0x3B:  // F2
                        s = 0x3C00;
                    break;
                    case 0x3C:  // F3
                        s = 0x3D00;
                    break;
                    case 0x3D:  // F4
                        s = 0x3E00;
                    break;
                    case 0x3E:  // F5
                        s = 0x3F00;
                    break;
                    case 0x3F:  // F6
                        s = 0x4000;
                    break;
                    case 0x40:  // F7
                        s = 0x4100;
                    break;
                    case 0x41:  // F8
                        s = 0x4200;
                    break;
                    case 0x42:  // F9
                        s = 0x4300;
                    break;
                    case 0x43:  // F10
                        s = 0x4400;
                    break;
                    case 0x44:  // F11
                        s = 0x8500;
                    break;
                    case 0x45:  // F12
                        s = 0x8600;
                    break;
                    case 0x4A:  // Home
                        s = 0x4700;
                    break;
                    case 0x4B:  // PageUp
                        s = 0x4900;
                    break;
                    case 0x4C:  // Del
                        s = 0x5300;
                    break;
                    case 0x4D:  // End
                        s = 0x4F00;
                    break;
                    case 0x4E:  // PageDown
                        s = 0x5100;
                    break;
                    case 0x4F:  // Right
                        s = 0x4D00;
                    break;
                    case 0x50:  // Left
                        s = 0x4B00;
                    break;
                    case 0x51:  // Down
                        s = 0x5000;
                    break;
                    case 0x52:  // Up
                        s = 0x4800;
                    break;
                }
            break;

            case LSHIFT:
            case RSHIFT:
                switch(c) {
                    case 0x3A:  // F1
                        s = 0x5400;
                    break;
                    case 0x3B:  // F2
                        s = 0x5500;
                    break;
                    case 0x3C:  // F3
                        s = 0x5600;
                    break;
                    case 0x3D:  // F4
                        s = 0x5700;
                    break;
                    case 0x3E:  // F5
                        s = 0x5800;
                    break;
                    case 0x3F:  // F6
                        s = 0x5900;
                    break;
                    case 0x40:  // F7
                        s = 0x5A00;
                    break;
                    case 0x41:  // F8
                        s = 0x5B00;
                    break;
                    case 0x42:  // F9
                        s = 0x5C00;
                    break;
                    case 0x43:  // F10
                        s = 0x5D00;
                    break;
                    case 0x44:  // F11
                        s = 0x8700;
                    break;
                    case 0x45:  // F12
                        s = 0x8800;
                    break;
                }
            break;

            case LCTRL:
            case RCTRL:
                if (c == 0x48) {    // Ctrl-Break
                    s = 0xFF00;     // made up scan code
                }
            break;

            case ALT:
            case ALTGR:
                // we're only interested in ALT-A -> ALT-Z
                if (c >= 0x04 and c <= 0x1D) {
                    c += 0x3D; // convert to ascii value
                    s = s_pThis->m_pCharConv->ScanCode(c);
                    s &= 0xFF00; // remove the ascii value, leave the scan code
                }
            break;

        }

        if (s) {
            s_pThis->m_pCharConv->AddSafe(s_pThis->m_pKeyBuf, s);
        }

    }

}

void CKeyboardTask::SelectConsoleHandler(unsigned nConsole) {

    assert(s_pThis != NULL);

    klog(LogDebug, "SelectConsoleHandler %d", nConsole);

    s_pThis->m_pKernel->SetConsole(nConsole);

}

