#ifndef CHARCONV_H
#define CHARCONV_H

#include "common.h"

class CCharConv {
public:
    CCharConv();
    ~CCharConv();

    void SetOperatingMode(TOperatingMode nOperatingMode);

    // both network and keyboard data are 'char' (e.g. signed) so stick with that convention
    char Convert(char c);
    u16 ScanCode(char c);

private:
    TOperatingMode m_nOperatingMode;

};

#endif
