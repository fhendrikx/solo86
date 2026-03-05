#include "terminalwrapper.h"
#include <circle/windowdisplay.h>

LOGMODULE("termwrap");

CTerminalWrapper::CTerminalWrapper(const char *pName, unsigned nWidth, unsigned nHeight,
    unsigned nCols, unsigned nRows, unsigned nBorderColour, const TFont &rFont) :
    CDisplay (CDisplay::I8),
    m_pName(pName),
    m_TerminalFont(rFont) {

    m_pTerminal = NULL;
    m_pFrameBuffer = NULL;

    m_nFrameBufferWidth = nWidth;
    m_nFrameBufferHeight = nHeight;
    m_nFrameBufferSize = nWidth * nHeight;

    m_nCols = nCols;
    m_nRows = nRows;

    if (nBorderColour <= 31) {
        m_nBorderColour = nBorderColour;
    } else {
        m_nBorderColour = 0;
        klog(LogWarning, "[%s] Invalid Border Colour %u", m_pName, nBorderColour);
    }

    m_pFrameBufferBackup = NULL;
}

CTerminalWrapper::~CTerminalWrapper() {

    if (m_pTerminal != NULL)
        delete m_pTerminal;

    if (m_pFrameBuffer != NULL)
        delete m_pFrameBuffer;

    if (m_pFrameBufferBackup != NULL)
        delete m_pFrameBufferBackup;

}

bool CTerminalWrapper::Initialize() {

    // initialise the framebuffer backup
    m_pFrameBufferBackup = new u8[m_nFrameBufferSize];

    if (m_pFrameBufferBackup == NULL) {
        klog(LogError, "[%s] Failed to create FrameBufferBackup", m_pName);
        return false;
    }

    // fill with the border colour
    memset(m_pFrameBufferBackup, 255, m_nFrameBufferSize);

    // setup the display
    CDisplay *pWindow = this;

    unsigned nMaxCols = m_nFrameBufferWidth / m_TerminalFont.width;
    unsigned nMaxRows = m_nFrameBufferHeight / (m_TerminalFont.height + m_TerminalFont.extra_height);

    if (m_nCols > 0 and m_nRows > 0 and m_nCols < nMaxCols and m_nRows < nMaxRows) {

        klog(LogNotice, "[%s] Forcing terminal size %ux%u", m_pName, m_nCols, m_nRows);

        CDisplay::TArea Area;

        unsigned nBorderWidth = (m_nFrameBufferWidth - (m_TerminalFont.width * m_nCols)) / 2;
        unsigned nBorderHeight = (m_nFrameBufferHeight - ((m_TerminalFont.height + m_TerminalFont.extra_height) * m_nRows)) / 2;

        Area.x1 = nBorderWidth;
        Area.x2 = m_nFrameBufferWidth - nBorderWidth - 1;
        Area.y1 = nBorderHeight;
        Area.y2 = m_nFrameBufferHeight - nBorderHeight - 1;

        klog(LogNotice, "[%s] Window (%u,%u) (%u,%u)", m_pName, Area.x1, Area.y1, Area.x2, Area.y2);

        pWindow = new CWindowDisplay (this, Area);
        if (pWindow == NULL) {
            klog(LogError, "[%s] CWindowDisplay creation failed", m_pName);
            return false;
        }

        // fill the text area with black
        for (unsigned y = Area.y1; y <= Area.y2; y++) {
            for (unsigned x = Area.x1; x <= Area.x2; x++) {
                m_pFrameBufferBackup[x + y * m_nFrameBufferWidth] = 0;
            }
        }

    }

    // setup the terminal device
    m_pTerminal = new CTerminalDevice(pWindow, 0, m_TerminalFont);

    if (m_pTerminal == NULL || !m_pTerminal->Initialize()) {
        klog(LogError, "[%s] TerminalDevice init failed", m_pName);
        return false;
    }

    m_pTerminal->Update(0);
    m_pTerminal->SetCursorBlock(true);

    return true;

}

bool CTerminalWrapper::Activate() {

    if (m_pFrameBuffer == NULL) {

        m_pFrameBuffer = new CBcmFrameBuffer(m_nFrameBufferWidth, m_nFrameBufferHeight, DEPTH);

        if (m_pFrameBuffer == NULL) {
            // framebuffer object is unusable, bomb out
            klog(LogError, "[%s] Failed to create frame buffer", m_pName);
            return false;
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

        // border colour
        m_pFrameBuffer->SetPalette(255, COLOR16(m_nBorderColour, m_nBorderColour, m_nBorderColour));

        if (!m_pFrameBuffer->Initialize()) {
            // framebuffer object is unusable, bomb out
            klog(LogError, "[%s] Failed to init frame buffer", m_pName);
            return false;
        } else {
            klog(LogNotice, "[%s] Frame buffer init %ux%u", m_pName,
                m_pFrameBuffer->GetWidth(), m_pFrameBuffer->GetHeight());
        }

        m_pFrameBuffer->WaitForVerticalSync();

        // TODO better safety tests
        if (m_nFrameBufferSize == m_pFrameBuffer->GetSize()) {
            memcpy((void *)m_pFrameBuffer->GetBuffer(), m_pFrameBufferBackup, m_nFrameBufferSize);
        }

        m_pTerminal->Update();

        return true;

    }

    klog(LogError, "[%s] Activate failed, FrameBuffer not null", m_pName);
    return false;

}

bool CTerminalWrapper::Deactivate() {

    klog(LogNotice, "[%s] Deactivate", m_pName);

    if (m_pFrameBuffer != NULL) {

        if (m_nFrameBufferSize == m_pFrameBuffer->GetSize()) {
            memcpy(m_pFrameBufferBackup, (void *)m_pFrameBuffer->GetBuffer(), m_nFrameBufferSize);
        }

        delete m_pFrameBuffer;
        m_pFrameBuffer = NULL;

        return true;

    }

    klog(LogError, "[%s] Deactivate failed, FrameBuffer is null", m_pName);
    return false;

}

int CTerminalWrapper::Write(const void *pBuffer, size_t nCount) {

    int retval = m_pTerminal->Write(pBuffer, nCount);

    if (m_pFrameBuffer != NULL)
        m_pTerminal->Update();

    return retval;

}

unsigned CTerminalWrapper::GetWidth() const {

    return m_nFrameBufferWidth;

}

unsigned CTerminalWrapper::GetHeight() const {

    return m_nFrameBufferHeight;

}

unsigned CTerminalWrapper::GetDepth() const {

    return DEPTH;

}

void CTerminalWrapper::SetPixel(unsigned nPosX, unsigned nPosY, TRawColor nColor) {

    if (m_pFrameBuffer != NULL) {

        m_pFrameBuffer->SetPixel(nPosX, nPosY, nColor);

    }

}

void CTerminalWrapper::SetArea(const TArea &rArea, const void *pPixels,
                    TAreaCompletionRoutine *pRoutine, void *pParam) {

    if (m_pFrameBuffer != NULL) {

        m_pFrameBuffer->SetArea(rArea, pPixels, pRoutine, pParam);

    }

}

CDisplay *CTerminalWrapper::GetParent() const {

    return NULL;

}

unsigned CTerminalWrapper::GetOffsetX() const {

    return 0;

}

unsigned CTerminalWrapper::GetOffsetY() const {

    return 0;

}
