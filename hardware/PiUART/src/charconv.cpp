#include "charconv.h"

LOGMODULE("charconv");

CCharConv::CCharConv() {

    m_bCRLF = false;
    m_bDelBS = false;

}

CCharConv::~CCharConv() {

}

void CCharConv::SetCRLF(bool b) {

    m_bCRLF = b;

}

void CCharConv::SetDelBS(bool b) {

    m_bDelBS = b;

}

char CCharConv::Convert(char c) {

    // ELKS requires \r
    // CP/M requires ???
    // MONITOR converts \r -> \n
    // Tiny Basic requires \n

    // false == \r, true == \n
    if (m_bCRLF) {

        if (c == '\r') {
            c = '\n';
        }

    } else {

        if (c == '\n') {
            c = '\r';
        }

    }

    // ELKS seems happy with DEL or BS
    // CP/M requires BS (^H)
    // MONITOR requires DEL

    // false == DEL, true == BS
    if (m_bDelBS) {

        if (c == 0x7f) {
            c = 0x8;
        }

    } else {

        if (c == 0x8) {
            c = 0x7f;
        }
 
    }

    return c;

}