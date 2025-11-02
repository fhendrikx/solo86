#include "keyboardtask.h"

#include <circle/devicenameservice.h>
#include <circle/sched/scheduler.h>

LOGMODULE("keyboardtask");

CKeyboardTask *CKeyboardTask::s_pThis = NULL;

CKeyboardTask::CKeyboardTask(CUSBHCIDevice *pUSBHCI, CRingBuf<u8> *pKeyBuf, CKernel *pKernel) {

    SetName("keyboardtask");

    m_pUSBHCI = pUSBHCI;
    m_pKeyboard = NULL;
    m_pKeyBuf = pKeyBuf;
    m_pKernel = pKernel;
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

    while (*pString) {

        char c = *pString;

        if (c >= 0x20 and c < 0x7f) {
            klog(LogDebug, "KeyPress: %02x '%c'", c, c);
        } else {
            klog(LogDebug, "KeyPress: %02x", c);
        }

        // ELKS seems to require \r
        if (c == '\n') {
            c = '\r';
        }

        s_pThis->m_pKeyBuf->AddSafe(c);

        pString++;
    }

}

void CKeyboardTask::SelectConsoleHandler(unsigned nConsole) {

    assert(s_pThis != NULL);

    klog(LogDebug, "SelectConsoleHandler %d", nConsole);

    s_pThis->m_pKernel->SetConsole(nConsole);

}

