#ifndef TERMINAL_H
#define TERMINAL_H

#include <circle/util.h>
#include "common.h"

#if DEPTH == 8

#define SSFN_CONSOLEBITMAP_PALETTE
//#define CONSOLE_FG 10  // GREEN
#define CONSOLE_FG 11  // CYAN

#elif DEPTH == 16

#define SSFN_CONSOLEBITMAP_HICOLOR
//#define CONSOLE_FG 0x57EA  // GREEN
#define CONSOLE_FG 0x57FF  // CYAN

#else
#error Bad DEPTH
#endif

#define CONSOLE_MAX_COLS 86
#define CONSOLE_MAX_ROWS 40

class CTerminal {
public:

    CTerminal();
    ~CTerminal();

    bool Initialize(unsigned nWidth, unsigned nHeight, unsigned nPitch);
    void UpdateDisplay(u8 *pBuffer);
    void Write(char c);
 
private:
    
    static void PeriodicTimerHandler();

    unsigned m_nScreenWidth;
    unsigned m_nScreenHeight;
    unsigned m_nScreenPitch;
    
    unsigned m_nFontWidth;
    unsigned m_nFontHeight;
    
    unsigned m_nBorderWidth;
    unsigned m_nBorderHeight;
    
    unsigned m_nCols;
    unsigned m_nRows;
    
    char *m_pTerminalBuffer;
    
    unsigned m_nCursorCol;
    unsigned m_nCursorRow;

    bool m_bBlink;

    static CTerminal *s_pThis;
    
};
  
#endif
