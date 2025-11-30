#ifndef TERMINAL_WRAPPER_H
#define TERMINAL_WRAPPER_H

#include <circle/bcmframebuffer.h>
#include <circle/display.h>
#include <circle/screen.h>
#include <circle/terminal.h>
#include <circle/util.h>

#include "common.h"

class CTerminalWrapper : public CDisplay {

    public:

    CTerminalWrapper(const char *pName, unsigned nWidth, unsigned nHeight, const TFont &rFont = DEFAULT_FONT);
    ~CTerminalWrapper();

    bool Initialize();

    bool Activate();
    bool Deactivate();

    // Terminal functions

    int Write(const void *pBuffer, size_t nCount);

    // overrides for CDisplay

    unsigned GetWidth() const;
    unsigned GetHeight() const;
    unsigned GetDepth() const;

    void SetPixel(unsigned nPosX, unsigned nPosY, TRawColor nColor);
    void SetArea(const TArea &rArea, const void *pPixels,
                    TAreaCompletionRoutine *pRoutine = nullptr,
                    void *pParam = nullptr);

    CDisplay *GetParent() const;
    unsigned GetOffsetX() const;
    unsigned GetOffsetY() const;

    private:

    const char *m_pName;

    CTerminalDevice *m_pTerminal;
    CBcmFrameBuffer *m_pFrameBuffer;

    unsigned m_nFrameBufferWidth;
    unsigned m_nFrameBufferHeight;
    unsigned m_nFrameBufferSize;
    
    const TFont m_TerminalFont;

    u8 *m_pFrameBufferBackup;

};

#endif
