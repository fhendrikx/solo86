#ifndef CHARCONV_H
#define CHARCONV_H

#include "common.h"

class CCharConv {
public:
    CCharConv();
    ~CCharConv();

    void SetCRLF(bool b);
    void SetDelBS(bool b);

    char Convert(char c);

private:
    bool m_bCRLF;
    bool m_bDelBS;

};

#endif
