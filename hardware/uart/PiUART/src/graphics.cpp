#include "graphics.h"
#include "vga_rgb565.h"

#define abs(a) ((a) > 0 ? (a) : -(a))

#define mk_u16(low, high) (u16)(((high) << 8) | (low))

// modified from Adafruit_GFX library
#ifndef _swap_s16
#define _swap_s16(a, b) \
    {                   \
        s16 t = a;      \
        a = b;          \
        b = t;          \
    }
#endif

LOGMODULE("graphics");

CGraphics::CGraphics() {

    m_pFrameBuffer = NULL;
    m_pFrameBufferBackup = NULL;

}

CGraphics::~CGraphics() {

    if (m_pFrameBuffer != NULL)
        delete m_pFrameBuffer;

    if (m_pFrameBufferBackup != NULL)
        delete m_pFrameBufferBackup;

}

bool CGraphics::Initialize() {

    m_pFrameBufferBackup = new u8[GRAPHICS_BUF_SIZE];

    if (m_pFrameBufferBackup == NULL) {
        klog(LogError, "Failed to create FrameBufferBackup");
        return false;
    }

    SetResolution(Res256x192);

    return true;

}

bool CGraphics::Activate(bool bLock) {

    if (m_pFrameBuffer == NULL) {

        if (bLock)
            m_Lock.Acquire();
        
        m_pFrameBuffer = new CBcmFrameBuffer(m_nFrameBufferWidth, m_nFrameBufferHeight, DEPTH);

        if (m_pFrameBuffer == NULL) {
            // framebuffer object is unusable, bomb out
            klog(LogError, "Failed to create frame buffer");
            return false;
        }

        // set the palette
        for (int i=0; i < 256; i++) {
            m_pFrameBuffer->SetPalette(i, VGAPalette[i]);
        }

        if (!m_pFrameBuffer->Initialize()) {
            // framebuffer object is unusable, bomb out
            klog(LogError, "Failed to init frame buffer");
            return false;
        } else {
            klog(LogNotice, "Frame buffer init %ux%u",
                m_pFrameBuffer->GetWidth(), m_pFrameBuffer->GetHeight());
        }

        m_pFrameBuffer->WaitForVerticalSync();

        memcpy((void *)m_pFrameBuffer->GetBuffer(), m_pFrameBufferBackup, m_pFrameBuffer->GetSize());

        if (bLock)
            m_Lock.Release();

        return true;

    }

    klog(LogError, "Activate failed, FrameBuffer not null");
    return false;

}

bool CGraphics::Deactivate(bool bLock) {

    klog(LogNotice, "Deactivate");

    if (m_pFrameBuffer != NULL) {

        if (bLock)
            m_Lock.Acquire();

        memcpy(m_pFrameBufferBackup, (void *)m_pFrameBuffer->GetBuffer(), m_pFrameBuffer->GetSize());

        delete m_pFrameBuffer;
        m_pFrameBuffer = NULL;

        if (bLock)
            m_Lock.Release();

        return true;

    }

    klog(LogError, "Deactivate failed, FrameBuffer is null");
    return false;

}

void CGraphics::MemWrite(u8 nColour) {

    unsigned nIndex = m_nMemWriteX + (m_nMemWriteY * m_nFrameBufferWidth);

    GetBuffer()[nIndex] = nColour;

    m_nMemWriteX++;
    m_nMemWriteY += m_nMemWriteX / m_nFrameBufferWidth;
    m_nMemWriteX %= m_nFrameBufferWidth;
    m_nMemWriteY %= m_nFrameBufferHeight;

}

u8 CGraphics::MemRead() {

    m_Lock.Acquire();

    unsigned nIndex = m_nMemReadX + (m_nMemReadY * m_nFrameBufferWidth);

    u8 retval = GetBuffer()[nIndex];

    m_nMemReadX++;
    m_nMemReadY += m_nMemReadX / m_nFrameBufferWidth;
    m_nMemReadX %= m_nFrameBufferWidth;
    m_nMemReadY %= m_nFrameBufferHeight;

    m_Lock.Release();

    return retval;

}

u8 *CGraphics::GetBuffer() {

    if (m_pFrameBuffer == NULL) {

        return m_pFrameBufferBackup;

    } else {

        return (u8 *)m_pFrameBuffer->GetBuffer();

    }

}

void CGraphics::Command(u8 nCmd, u8 *pParamBuffer, unsigned nParamBufferLength) {

    // klog(LogNotice, "Command 0x%x len %u", nCmd, nParamBufferLength);

    switch(nCmd) {

        // set video properties
        case 0x40:
            SetResolution(Res256x192);
        break;

        case 0x41:
            SetResolution(Res512x384);
        break;

        case 0x42:
            SetResolution(Res1024x768);
        break;

        // 0x43 - 0x47

        // set video memory read/write position
        // 8 bit
        case 0x48:
            m_nMemWriteX = pParamBuffer[0];
        break;

        case 0x49:
            m_nMemWriteY = pParamBuffer[0] % m_nFrameBufferHeight;
        break;
        
        case 0x4A:
            m_nMemReadX = pParamBuffer[0];
        break;

        case 0x4B:
            m_nMemReadY = pParamBuffer[0] % m_nFrameBufferHeight;
        break;

        // 16 bit
        case 0x4C:
            m_nMemWriteX = mk_u16(pParamBuffer[0], pParamBuffer[1]) % m_nFrameBufferWidth;
        break;

        case 0x4D:
            m_nMemWriteY = mk_u16(pParamBuffer[0], pParamBuffer[1]) % m_nFrameBufferHeight;
        break;
        
        case 0x4E:
            m_nMemReadX = mk_u16(pParamBuffer[0], pParamBuffer[1]) % m_nFrameBufferWidth;
        break;

        case 0x4F:
            m_nMemReadY = mk_u16(pParamBuffer[0], pParamBuffer[1]) % m_nFrameBufferHeight;
        break;

        // Set Draw Mode
        case 0x50:
            m_nDrawMode = ClippingMode;
        break;

        case 0x51:
            m_nDrawMode = WrapMode;
        break;

        // 0x52 - 0x5F

        // drawing commands, 8 bit
        case 0x60:
            DrawPixel(pParamBuffer[0], pParamBuffer[1], pParamBuffer[2]);
        break;

        case 0x61:
            DrawLine(pParamBuffer[0], pParamBuffer[1],
                     pParamBuffer[2], pParamBuffer[3], pParamBuffer[4]);
        break;

        // drawing commands, 16 bit
        case 0x80:
            DrawPixel(mk_u16(pParamBuffer[0], pParamBuffer[1]),
                     mk_u16(pParamBuffer[2], pParamBuffer[3]), pParamBuffer[4]);
        break;

        case 0x81:
            DrawLine(mk_u16(pParamBuffer[0], pParamBuffer[1]),
                     mk_u16(pParamBuffer[2], pParamBuffer[3]),
                     mk_u16(pParamBuffer[4], pParamBuffer[5]),
                     mk_u16(pParamBuffer[6], pParamBuffer[7]), pParamBuffer[8]);
        break;

    }

}

void CGraphics::SetResolution(TResolution Res) {

    m_Lock.Acquire();

    m_nDrawMode = ClippingMode;

    m_nFrameBufferWidth = Res * 256;
    m_nFrameBufferHeight = Res * 192;

    m_nMemReadX = 0;
    m_nMemReadY = 0;
    m_nMemWriteX = 0;
    m_nMemWriteY = 0;

    if (m_pFrameBuffer != NULL) {

        Deactivate(false);
        memset(m_pFrameBufferBackup, 0, GRAPHICS_BUF_SIZE);
        Activate(false);

    } else {

        memset(m_pFrameBufferBackup, 0, GRAPHICS_BUF_SIZE);

    }

    m_Lock.Release();

}

void CGraphics::DrawPixel(s16 x, s16 y, u8 c) {

    switch(m_nDrawMode) {

        case ClippingMode:

            if (x < 0 || x >= (s16)m_nFrameBufferWidth)
                return;

            if (y < 0 || y >= (s16)m_nFrameBufferHeight)
                return;

        break;

        case WrapMode:

            x %= m_nFrameBufferWidth;
            y %= m_nFrameBufferHeight;

        break;
    }

    GetBuffer()[x + (y * m_nFrameBufferWidth)] = c;
}

// modified from Adafruit_GFX library
void CGraphics::DrawLine(s16 x0, s16 y0, s16 x1, s16 y1, u8 c) {

    s16 steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_s16(x0, y0);
        _swap_s16(x1, y1);
    }

    if (x0 > x1) {
        _swap_s16(x0, x1);
        _swap_s16(y0, y1);
    }

    s16 dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    s16 err = dx / 2;
    s16 ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            DrawPixel(y0, x0, c);
        } else {
            DrawPixel(x0, y0, c);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }

}