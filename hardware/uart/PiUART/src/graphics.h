#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <circle/bcmframebuffer.h>
#include <circle/util.h>

#include "common.h"

#define GRAPHICS_BUF_SIZE 1024 * 768

enum TResolution { Res256x192 = 1, Res512x384 = 2, Res1024x768 = 4 };
enum TDrawMode { ClippingMode = 0, WrapMode = 1 };

class CGraphics {

    public:

    CGraphics();
    ~CGraphics();

    bool Initialize();

    bool Activate(bool bLock = true);
    bool Deactivate(bool bLock = true);

    void MemWrite(u8 nColour);
    u8 MemRead();

    void Command(u8 nCmd, u8 *pParamBuffer, unsigned nParamBufferLength);

    private:

    inline u8 *GetBuffer();
    void SetResolution(TResolution Res);
    inline void DrawPixel(s16 x, s16 y, u8 c);
    void DrawLine(s16 x0, s16 y0, s16 x1, s16 y1, u8 c);

    CBcmFrameBuffer *m_pFrameBuffer;
    u8 *m_pFrameBufferBackup;

    CSpinLock m_Lock;

    enum TDrawMode m_nDrawMode;

    unsigned m_nFrameBufferWidth;
    unsigned m_nFrameBufferHeight;

    unsigned m_nMemReadX;
    unsigned m_nMemReadY;
    unsigned m_nMemWriteX;
    unsigned m_nMemWriteY;

};

#endif
