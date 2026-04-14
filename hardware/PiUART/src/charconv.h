#ifndef CHARCONV_H
#define CHARCONV_H

#include "common.h"

class CCharConv {
public:
    CCharConv();
    ~CCharConv();

    void SetCRLF(bool b);
    void SetDelBS(bool b);
    void SetEsc(bool b);

    // both network and keyboard data are 'char' (e.g. signed) so stick with that convention
    char Convert(char c);
    u16 ScanCode(char c);

private:
    bool m_bCRLF;
    bool m_bDelBS;
    bool m_bEsc;

};

#endif
