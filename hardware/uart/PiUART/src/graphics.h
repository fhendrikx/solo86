#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <circle/bcmframebuffer.h>
#include <circle/util.h>

#include "common.h"

#define GRAPHICS_MAX_WIDTH 1920
#define GRAPHICS_MAX_HEIGHT 1080
#define GRAPHICS_BUF_SIZE GRAPHICS_MAX_WIDTH * GRAPHICS_MAX_HEIGHT

enum TResolution { Res256x192 = 0, Res512x384 = 1, Res1024x768 = 2, ResCustom = 3,
                   Res256x192DB = 4, Res512x384DB = 5, Res1024x768DB = 6, ResCustomDB = 7 };

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
    void SetResolution(TResolution Res, u16 width = 0, u16 height = 0);
    void Update();
    inline void DrawPixel(s16 x, s16 y, u8 c);
    void DrawLine(s16 x0, s16 y0, s16 x1, s16 y1, u8 c);

    CBcmFrameBuffer *m_pFrameBuffer;

    // store a copy of the FrameBuffer when not the live display or when double buffered
    u8 *m_pFrameBufferBackup;

    // the active FrameBuffer, writing here will appear on the display
    u8 *m_pFrameBufferActive;

    // the standby FrameBuffer (when double buffered), switch active/standby with Update()
    u8 *m_pFrameBufferStandby;

    CSpinLock m_Lock;

    enum TDrawMode m_nDrawMode;
    bool m_bDoubleBuffered;

    unsigned m_nFrameBufferWidth;
    unsigned m_nFrameBufferHeight;
    unsigned m_nFrameBufferSize;

    unsigned m_nMemReadX;
    unsigned m_nMemReadY;
    unsigned m_nMemWriteX;
    unsigned m_nMemWriteY;

};

#endif
