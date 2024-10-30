#include "keyboardtask.h"

#include <circle/devicenameservice.h>
#include <circle/sched/scheduler.h>

LOGMODULE("keyboardtask");

CKeyboardTask *CKeyboardTask::s_pThis = NULL;

CKeyboardTask::CKeyboardTask(CUSBHCIDevice *pUSBHCI, CRingBuf *pKeyBuf) {

    SetName("keyboardtask");

    m_pUSBHCI = pUSBHCI;
    m_pKeyboard = NULL;
    m_pKeyBuf = pKeyBuf;
    s_pThis = this;
}

CKeyboardTask::~CKeyboardTask() {}

void CKeyboardTask::Run() {

    klog(LogNotice, "Starting Keyboard Task");

    while(true) {

        boolean bUpdated = m_pUSBHCI->UpdatePlugAndPlay();

        if (bUpdated and m_pKeyboard == NULL) {

            m_pKeyboard = (CUSBKeyboardDevice *) CDeviceNameService::Get()->GetDevice("ukbd1", FALSE);

            if (m_pKeyboard != NULL) {

                klog(LogNotice, "Keyboard updated");

                m_pKeyboard->RegisterRemovedHandler(KeyboardRemovedHandler);
                m_pKeyboard->RegisterKeyPressedHandler(KeyPressedHandler);

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

#ifndef NDEBUG
    const char *pStringCopy = pString;

    while (*pStringCopy) {

        if (*pStringCopy >= 0x20 and *pStringCopy < 0x7f) {
            klog(LogNotice, "KeyPress: %02x '%c'", *pStringCopy, *pStringCopy);
        } else {
            klog(LogNotice, "KeyPress: %02x", *pStringCopy);
        }

        pStringCopy++;

    }
#endif

    char c = pString[0];

    // ignore escape encoded keys, we only want 7-bit clean ascii
    if (c != 0x1b) {

        // fix backspace
        if (c == 0x7f)
            c = 0x8; // ascii backspace

        s_pThis->m_pKeyBuf->AddCharSafe(c);

    }

}

