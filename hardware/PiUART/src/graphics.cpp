#include "graphics.h"
#include "vga_rgb565.h"

#define abs(a) ((a) > 0 ? (a) : -(a))

#define mk_u16(low, high) (u16)(((high) << 8) | (low))
#define mk_s16(low, high) (s16)(((high) << 8) | (low))

// modified from Adafruit_GFX library
#define _swap_s16(a, b) \
    {                   \
        s16 t = a;      \
        a = b;          \
        b = t;          \
    }

LOGMODULE("graphics");

CGraphics::CGraphics() {

    m_pFrameBuffer = NULL;
    m_pFrameBufferBackup = NULL;
    m_pFrameBufferActive = NULL;
    m_pFrameBufferStandby = NULL;
    m_pFrameBufferPtr = NULL;

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

    m_pFrameBufferPtr = m_pFrameBufferBackup;

    SetResolution(Res256x192);

    return true;

}

bool CGraphics::Activate() {

    return Activate(true);

}

bool CGraphics::Activate(bool bLock) {

    if (m_pFrameBuffer == NULL) {

        if (bLock)
            m_Lock.Acquire();

        m_pFrameBuffer = new CBcmFrameBuffer(m_nFrameBufferWidth, m_nFrameBufferHeight, DEPTH,
                                             0, 0, 0, m_bDoubleBuffered);

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
            klog(LogNotice, "Frame buffer init %ux%u DB=%d",
                 m_pFrameBuffer->GetWidth(), m_pFrameBuffer->GetHeight(), m_bDoubleBuffered);
        }

        m_pFrameBufferActive = (u8 *)m_pFrameBuffer->GetBuffer();
        m_pFrameBufferStandby = m_pFrameBufferActive + (m_nFrameBufferWidth * m_nFrameBufferHeight);

        m_pFrameBuffer->WaitForVerticalSync();

        memcpy(m_pFrameBufferActive, m_pFrameBufferBackup, m_nFrameBufferSize);

        if (not m_bDoubleBuffered)
            m_pFrameBufferPtr = m_pFrameBufferActive;

        if (bLock)
            m_Lock.Release();

        return true;

    }

    klog(LogError, "Activate failed, FrameBuffer not null");
    return false;

}

bool CGraphics::Deactivate() {

    return Deactivate(true);

}

bool CGraphics::Deactivate(bool bLock) {

    klog(LogNotice, "Deactivate");

    if (m_pFrameBuffer != NULL) {

        if (bLock)
            m_Lock.Acquire();

        if (not m_bDoubleBuffered)
            memcpy(m_pFrameBufferBackup, m_pFrameBufferActive, m_nFrameBufferSize);

        delete m_pFrameBuffer;
        m_pFrameBuffer = NULL;
        m_pFrameBufferActive = NULL;
        m_pFrameBufferStandby = NULL;

        m_pFrameBufferPtr = m_pFrameBufferBackup;

        if (bLock)
            m_Lock.Release();

        return true;

    }

    klog(LogError, "Deactivate failed, FrameBuffer is null");
    return false;

}

void CGraphics::MemWrite(u8 nColour) {

    unsigned nIndex = m_nMemWriteX + (m_nMemWriteY * m_nFrameBufferWidth);

    m_pFrameBufferPtr[nIndex] = nColour;

    m_nMemWriteX++;
    m_nMemWriteY += m_nMemWriteX / m_nFrameBufferWidth;
    m_nMemWriteX %= m_nFrameBufferWidth;
    m_nMemWriteY %= m_nFrameBufferHeight;

}

u8 CGraphics::MemRead() {

    m_Lock.Acquire();

    unsigned nIndex = m_nMemReadX + (m_nMemReadY * m_nFrameBufferWidth);

    u8 retval = m_pFrameBufferPtr[nIndex];

    m_nMemReadX++;
    m_nMemReadY += m_nMemReadX / m_nFrameBufferWidth;
    m_nMemReadX %= m_nFrameBufferWidth;
    m_nMemReadY %= m_nFrameBufferHeight;

    m_Lock.Release();

    return retval;

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

        case 0x43:
            SetResolution(ResCustom, mk_u16(pParamBuffer[0], pParamBuffer[1]),
                                     mk_u16(pParamBuffer[2], pParamBuffer[3]));
        break;

        case 0x44:
            SetResolution(Res256x192DB);
        break;

        case 0x45:
            SetResolution(Res512x384DB);
        break;

        case 0x46:
            SetResolution(Res1024x768DB);
        break;

        case 0x47:
            SetResolution(ResCustomDB, mk_u16(pParamBuffer[0], pParamBuffer[1]),
                                       mk_u16(pParamBuffer[2], pParamBuffer[3]));
        break;

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

        // misc commands
        case 0x50:
            Update();
        break;

        case 0x51: // clear screen
            memset(m_pFrameBufferPtr, 0, m_nFrameBufferSize);
        break;

        case 0x52: // fill screen
            memset(m_pFrameBufferPtr, pParamBuffer[0], m_nFrameBufferSize);
        break;

        // 0x53 - 0x5F

        // drawing commands, 8 bit
        case 0x60:
            DrawPixel(pParamBuffer[0], pParamBuffer[1], pParamBuffer[2]);
        break;

        case 0x61:
            DrawLine(pParamBuffer[0], pParamBuffer[1],
                     pParamBuffer[2], pParamBuffer[3], pParamBuffer[4]);
        break;

        case 0x62:
            DrawRect(pParamBuffer[0], pParamBuffer[1],
                     pParamBuffer[2], pParamBuffer[3], pParamBuffer[4]);
        break;

        case 0x63:
            FillRect(pParamBuffer[0], pParamBuffer[1],
                     pParamBuffer[2], pParamBuffer[3], pParamBuffer[4]);
        break;

        // drawing commands, 16 bit
        case 0x80:
            DrawPixel(mk_s16(pParamBuffer[0], pParamBuffer[1]),
                      mk_s16(pParamBuffer[2], pParamBuffer[3]), pParamBuffer[4]);
        break;

        case 0x81:
            DrawLine(mk_s16(pParamBuffer[0], pParamBuffer[1]),
                     mk_s16(pParamBuffer[2], pParamBuffer[3]),
                     mk_s16(pParamBuffer[4], pParamBuffer[5]),
                     mk_s16(pParamBuffer[6], pParamBuffer[7]), pParamBuffer[8]);
        break;

        case 0x82:
            DrawRect(mk_s16(pParamBuffer[0], pParamBuffer[1]),
                     mk_s16(pParamBuffer[2], pParamBuffer[3]),
                     mk_s16(pParamBuffer[4], pParamBuffer[5]),
                     mk_s16(pParamBuffer[6], pParamBuffer[7]), pParamBuffer[8]);
        break;

        case 0x83:
            FillRect(mk_s16(pParamBuffer[0], pParamBuffer[1]),
                     mk_s16(pParamBuffer[2], pParamBuffer[3]),
                     mk_s16(pParamBuffer[4], pParamBuffer[5]),
                     mk_s16(pParamBuffer[6], pParamBuffer[7]), pParamBuffer[8]);
        break;

    }

}

void CGraphics::SetResolution(TResolution Res, u16 width, u16 height) {

    m_Lock.Acquire();

    switch(Res) {

        case Res256x192:
            m_nFrameBufferWidth = 256;
            m_nFrameBufferHeight = 192;
            m_bDoubleBuffered = false;
        break;

        case Res256x192DB:
            m_nFrameBufferWidth = 256;
            m_nFrameBufferHeight = 192;
            m_bDoubleBuffered = true;
        break;

        case Res512x384:
            m_nFrameBufferWidth = 512;
            m_nFrameBufferHeight = 384;
            m_bDoubleBuffered = false;
        break;

        case Res512x384DB:
            m_nFrameBufferWidth = 512;
            m_nFrameBufferHeight = 384;
            m_bDoubleBuffered = true;
        break;

        case Res1024x768:
            m_nFrameBufferWidth = 1024;
            m_nFrameBufferHeight = 768;
            m_bDoubleBuffered = false;
        break;

        case Res1024x768DB:
            m_nFrameBufferWidth = 1024;
            m_nFrameBufferHeight = 768;
            m_bDoubleBuffered = true;
        break;

        case ResCustom:
        case ResCustomDB:

            if (width >= 64 && width <= GRAPHICS_MAX_WIDTH && width % 4 == 0 &&
                height >= 64 && height <= GRAPHICS_MAX_HEIGHT && height % 4 == 0) {
                m_nFrameBufferWidth = width;
                m_nFrameBufferHeight = height;
            } else {
                m_Lock.Release();
                klog(LogError, "Invalid resolution %ux%u", width, height);
                return;
            }

            if (Res == ResCustomDB)
                m_bDoubleBuffered = true;

        break;

    }

    klog(LogNotice, "SetResolution %ux%u", m_nFrameBufferWidth, m_nFrameBufferHeight);

    m_nFrameBufferSize = m_nFrameBufferWidth * m_nFrameBufferHeight;

    m_nMemReadX = 0;
    m_nMemReadY = 0;
    m_nMemWriteX = 0;
    m_nMemWriteY = 0;

    if (m_pFrameBuffer != NULL) {

        if (!Deactivate(false)) {
            CMultiCoreSupport::HaltAll();
        }

        memset(m_pFrameBufferBackup, 0, GRAPHICS_BUF_SIZE);

        if (!Activate(false)) {
            CMultiCoreSupport::HaltAll();
        }

    } else {

        memset(m_pFrameBufferBackup, 0, GRAPHICS_BUF_SIZE);

    }

    m_Lock.Release();

}

void CGraphics::Update() {

    if (m_bDoubleBuffered && m_pFrameBuffer != NULL) {

        memcpy(m_pFrameBufferStandby, m_pFrameBufferBackup, m_nFrameBufferSize);

        WaitForVerticalSync();

        if (m_pFrameBufferStandby > m_pFrameBufferActive)
            m_pFrameBuffer->SetVirtualOffset(0, m_nFrameBufferHeight);
        else
            m_pFrameBuffer->SetVirtualOffset(0, 0);

        u8 *tmp = m_pFrameBufferActive;
        m_pFrameBufferActive = m_pFrameBufferStandby;
        m_pFrameBufferStandby = tmp;

    } else {

        // this graphics object doesn't currently own the frame buffer or
        // we're not doing double buffering.
        // however we still wait for vsync so the Update() routine
        // provides consistent timing.

        WaitForVerticalSync();

    }

}

void CGraphics::WaitForVerticalSync() {

    // we don't always own the framebuffer so go direct to the graphics hardware
    // and wait for vsync on the underlying display instead

    // only works on Pi <= 3 (e.g. not Pi 4 or 5 which have multiple displays)

    // copied from bcmframebuffer.cpp WaitForVerticalSync()
    CBcmPropertyTags Tags;
    TPropertyTagSimple Dummy;
    Tags.GetTag (PROPTAG_WAIT_FOR_VSYNC, &Dummy, sizeof Dummy);

}

void CGraphics::DrawPixel(s16 x, s16 y, u8 c) {

    // if (x < 0 || (unsigned) x >= m_nFrameBufferWidth)
    //     return;

    // if (y < 0 || (unsigned) y >= m_nFrameBufferHeight)
    //     return;

    if ((unsigned) x >= m_nFrameBufferWidth)
        return;

    if ((unsigned) y >= m_nFrameBufferHeight)
        return;

    m_pFrameBufferPtr[(unsigned)x + ((unsigned)y * m_nFrameBufferWidth)] = c;

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

void CGraphics::DrawHLine(s16 x, s16 y, s16 len, u8 c) {

    if (len <= 0)
        return;

    // ignore lines above or below the screen
    if (y < 0 || y >= (s16)m_nFrameBufferHeight)
        return;

    s16 x_end = x + len;

    // truncate the left side if it's off the screen
    if (x < 0)
        x = 0;

    // truncate the right side if it's off the screen
    if (x_end > (s16)m_nFrameBufferWidth)
        x_end = (s16)m_nFrameBufferWidth;

    // draw the line
    for (s16 i = x; i < x_end; i++) {
        DrawPixel(i, y, c);
    }

}

void CGraphics::DrawVLine(s16 x, s16 y, s16 len, u8 c) {

    if (len <= 0)
        return;

    // ignore lines off the sides of the screen
    if (x < 0 || x >= (s16)m_nFrameBufferWidth)
        return;

    s16 y_end = y + len;

    // truncate the top if it's off the screen
    if (y < 0)
        y = 0;

    // truncate the bottom if it's off the screen
    if (y_end > (s16)m_nFrameBufferHeight)
        y_end = (s16)m_nFrameBufferHeight;

    // daw the line
    for (s16 i = y; i < y_end; i++) {
        DrawPixel(x, i, c);
    }

}

void CGraphics::DrawRect(s16 x, s16 y, s16 w, s16 h, u8 c) {

    if (w <= 0)
        return;

    if (h <= 0)
        return;

    DrawHLine(x, y, w, c);
    DrawHLine(x, y + h - 1, w, c);
    DrawVLine(x, y, h, c);
    DrawVLine(x + w - 1, y, h, c);

}

void CGraphics::FillRect(s16 x, s16 y, s16 w, s16 h, u8 c) {

    if (w <= 0)
        return;

    if (h <= 0)
        return;

    for (s16 i = y; i < y + h; i++) {
        DrawHLine(x, i, w, c);
    }

}
