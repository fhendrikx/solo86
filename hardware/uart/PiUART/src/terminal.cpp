#include "terminal.h"

// there is a typo in the c++ class wrapper and a dependany on strings.h
// which doesn't exist so undef __cplusplus rather than edit ssfn.h
#undef __cplusplus
#include "scalable-font2/ssfn.h"

#include "fonts/u_vga16.h" // font_small
#include "fonts/TerminusBold24x12.h" // font_medium
#include "fonts/TerminusBold36x16.h" // font_large

LOGMODULE("terminal");

CTerminal *CTerminal::s_pThis = NULL;

CTerminal::CTerminal() {

    m_pTerminalBuffer = NULL;
    
    m_nCursorCol = 0;
    m_nCursorRow = 0;

    m_bBlink = true;

    s_pThis = this;

}

CTerminal::~CTerminal() {

    if (m_pTerminalBuffer != NULL)
	delete[] m_pTerminalBuffer;

    s_pThis = NULL;
    
}

bool CTerminal::Initialize(unsigned nWidth, unsigned nHeight, unsigned nPitch) {
  
    m_nScreenWidth = nWidth;
    m_nScreenHeight = nHeight;
    m_nScreenPitch = nPitch;
    
    if (m_nScreenHeight <= 600)
	// 640x480, 800x600
	ssfn_src = (ssfn_font_t *) font_small;
    else if (m_nScreenHeight <= 900)
	// 1024x768
	ssfn_src = (ssfn_font_t *) font_medium;
    else
	// everything larger
	ssfn_src = (ssfn_font_t *) font_large;
    
    ssfn_dst.w = m_nScreenWidth;
    ssfn_dst.h = m_nScreenHeight;
    ssfn_dst.p = m_nScreenPitch;
    ssfn_dst.fg = CONSOLE_FG;

    m_nFontWidth = ssfn_src->width;
    m_nFontHeight = ssfn_src->height;
    
    klog(LogNotice, "m_nFontWidth %u", m_nFontWidth);
    klog(LogNotice, "m_nFontHeight %u", m_nFontHeight);
    
    m_nCols = m_nScreenWidth / m_nFontWidth;
    m_nRows = m_nScreenHeight / m_nFontHeight;

    m_nCols = m_nCols > CONSOLE_MAX_COLS ? CONSOLE_MAX_COLS : m_nCols;
    m_nRows = m_nRows > CONSOLE_MAX_ROWS ? CONSOLE_MAX_ROWS : m_nRows;

    klog(LogNotice, "m_nCols %u", m_nCols);
    klog(LogNotice, "m_nRows %u", m_nRows);
    
    unsigned nTerminalBufferSize = m_nCols * m_nRows;
    m_pTerminalBuffer = new char[nTerminalBufferSize];
    memset(m_pTerminalBuffer, 0, nTerminalBufferSize);

    m_nBorderWidth = (m_nScreenWidth - (m_nCols * m_nFontWidth)) / 2;
    m_nBorderHeight = (m_nScreenHeight - (m_nRows * m_nFontHeight)) / 2;

    klog(LogNotice, "m_nBorderWidth %u", m_nBorderWidth);
    klog(LogNotice, "m_nBorderHeight %u", m_nBorderHeight);

    CTimer::Get()->RegisterPeriodicHandler(PeriodicTimerHandler);

    return true;
}

void CTerminal::UpdateDisplay(u8 *pBuffer) {

    ssfn_dst.ptr = pBuffer;
    
    // clear the buffer
    memset(pBuffer, 0, m_nScreenHeight * m_nScreenPitch);
    unsigned nIndex = 0;
    
    for (unsigned nRow = 0; nRow < m_nRows; nRow++) {
	
	ssfn_dst.y = m_nBorderHeight + (nRow * m_nFontHeight);
	unsigned nXPos = m_nBorderWidth;
	
	for (unsigned nCol = 0; nCol < m_nCols; nCol++) {
	    
	    /*
	      updating .x ourselves as the sfnconv tool is adding an extra pixel by
	      setting the horizontal advance to width +1
	    */
	    
	    u8 c = m_pTerminalBuffer[nIndex++];
	    
	    // only draw printable chars, skip space (0x20) and del (0x7f)
	    if (c > 0x20 and c < 0x7f) {
		ssfn_dst.x = nXPos;
		ssfn_putc(c);
	    }
	    
	    nXPos += m_nFontWidth;
	    
	}
    }
    
    // draw the cursor
    if (m_bBlink && (m_nCursorCol < m_nCols)) {
	// use the font engine to draw the cursor
	ssfn_dst.x = m_nBorderWidth + (m_nFontWidth * m_nCursorCol);
	ssfn_dst.y = m_nBorderHeight + (m_nFontHeight * m_nCursorRow);
	ssfn_putc('_');
	
    }
    
}

void CTerminal::Write(char c) {

    switch(c) {
    case '\r':
	break;
	
    case 8: // backspace
	if (m_nCursorCol > 0)
	    m_nCursorCol--;
	
	break;
	
    case '\n':
	if (m_nCursorRow == (m_nRows - 1)) {
	    memmove(m_pTerminalBuffer, &m_pTerminalBuffer[m_nCols], m_nCols * (m_nRows - 1));
	    memset(&m_pTerminalBuffer[m_nCols * (m_nRows - 1)], 0, m_nCols);
	} else {
	    m_nCursorRow++;
	}
	m_nCursorCol = 0;
	
	break;
	
    default:
	if (c >= 0x20) {
	    if (m_nCursorCol < m_nCols) {
		m_pTerminalBuffer[(m_nCursorRow * m_nCols) + m_nCursorCol] = c;
		m_nCursorCol++;
	    }
	}
    }
    
}

// Called HZ times per second
void CTerminal::PeriodicTimerHandler() {

    if (s_pThis != NULL) {
  
	// twice per second, blink cursor
	if ((CTimer::Get()->GetTicks() % (HZ/2)) == 0) {
	    s_pThis->m_bBlink = !s_pThis->m_bBlink;
	}

    }
    
}

