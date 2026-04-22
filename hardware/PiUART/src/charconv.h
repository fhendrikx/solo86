#ifndef CHARCONV_H
#define CHARCONV_H

#include "common.h"
#include "ringbuf.h"

class CCharConv {
public:
    CCharConv();
    ~CCharConv();

    void SetOperatingMode(TOperatingMode nOperatingMode);

    // both network and keyboard data are 'char' (e.g. signed) so stick with that convention
    char Convert(char c);
    u16 ScanCode(char c);
    void AddSafe(CRingBuf<u16> *pRingBuf, u16 nScanCode);

private:
    inline const char *EscapeSequence(u16 nScanCode);
    
    TOperatingMode m_nOperatingMode;

};

#endif
