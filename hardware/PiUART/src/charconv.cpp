#include "charconv.h"

const u16 scancodes[128] = {
    // 0x0300,     // ctrl-2
    0x0000,     // no key pressed
    0x1E01,     // ctrl-A
    0x3002,     // ctrl-B
    0x2E03,     // ctrl-C
    0x2004,     // ctrl-D
    0x1205,     // ctrl-E
    0x2106,     // ctrl-F
    0x2207,     // ctrl-G
    // 0x2308,     // ctrl-H
    0x0E08,     // backspace
    // 0x1709,     // ctrl-I
    0x0F09,     // tab
    // 0x240A,     // ctrl-J
    0x1C0A,     // ctrl-enter
    0x250B,     // ctrl-K
    0x260C,     // ctrl-L
    // 0x320D,     // ctrl-M
    0x1C0D,     // enter
    0x310E,     // ctrl-N
    0x180F,     // ctrl-O
    0x1910,     // ctrl-P
    0x1011,     // ctrl-Q
    0x1312,     // ctrl-R
    0x1F13,     // ctrl-S
    0x1414,     // ctrl-T
    0x1615,     // ctrl-U
    0x2F16,     // ctrl-V
    0x1117,     // ctrl-W
    0x2D18,     // ctrl-X
    0x1519,     // ctrl-Y
    0x2C1A,     // ctrl-Z
    // 0x1A1B,     // ctrl-[
    0x011B,     // esc
    0x2B1C,     // ctrl-backslash
    0x1B1D,     // ctrl-]
    0x071E,     // ctrl-6
    0x0C1F,     // ctrl--
    0x3920,     // space
    0x0221,     // !
    0x2822,     // "
    0x0423,     // #
    0x0524,     // $
    0x0625,     // %
    0x0826,     // &
    0x2827,     // '
    0x0A28,     // (
    0x0B29,     // )
    0x092A,     // *
    0x0D2B,     // +
    0x332C,     // ,
    0x0C2D,     // -
    0x342E,     // .
    0x352F,     // /
    0x0B30,     // 0
    0x0231,     // 1
    0x0332,     // 2
    0x0433,     // 3
    0x0534,     // 4
    0x0635,     // 5
    0x0736,     // 6
    0x0837,     // 7
    0x0938,     // 8
    0x0A39,     // 9
    0x273A,     // :
    0x273B,     // ;
    0x333C,     // <
    0x0D3D,     // =
    0x343E,     // >
    0x353F,     // ?
    0x0340,     // @
    0x1E41,     // A
    0x3042,     // B
    0x2E43,     // C
    0x2044,     // D
    0x1245,     // E
    0x2146,     // F
    0x2247,     // G
    0x2348,     // H
    0x1749,     // I
    0x244A,     // J
    0x254B,     // K
    0x264C,     // L
    0x324D,     // M
    0x314E,     // N
    0x184F,     // O
    0x1950,     // P
    0x1051,     // Q
    0x1352,     // R
    0x1F53,     // S
    0x1454,     // T
    0x1655,     // U
    0x2F56,     // V
    0x1157,     // W
    0x2D58,     // X
    0x1559,     // Y
    0x2C5A,     // Z
    0x1A5B,     // [
    0x2B5C,     // backslash
    0x1B5D,     // ]
    0x075E,     // ^
    0x0C5F,     // _
    0x2960,     // `
    0x1E61,     // a
    0x3062,     // b
    0x2E63,     // c
    0x2064,     // d
    0x1265,     // e
    0x2166,     // f
    0x2267,     // g
    0x2368,     // h
    0x1769,     // i
    0x246A,     // j
    0x256B,     // k
    0x266C,     // l
    0x326D,     // m
    0x316E,     // n
    0x186F,     // o
    0x1970,     // p
    0x1071,     // q
    0x1372,     // r
    0x1F73,     // s
    0x1474,     // t
    0x1675,     // u
    0x2F76,     // v
    0x1177,     // w
    0x2D78,     // x
    0x1579,     // y
    0x2C7A,     // z
    0x1A7B,     // {
    0x2B7C,     // |
    0x1B7D,     // }
    0x297E,     // ~
    0x0E7F      // ctrl-backspace
};

// for scan codes 0x??00 translate into escape sequences
// taken from https://stanislavs.org/helppc/scan_codes.html
// only filling in the handful that we need for ELKS
// this would probably have been simpler as a switch statement in hindsight
const char *esc[256] = {
    NULL,        // 0x00
    NULL,        // 0x01  Alt Esc
    NULL,        // 0x02
    NULL,        // 0x03  Ctrl 2 (NUL)
    NULL,        // 0x04
    NULL,        // 0x05
    NULL,        // 0x06
    NULL,        // 0x07
    NULL,        // 0x08
    NULL,        // 0x09
    NULL,        // 0x0A
    NULL,        // 0x0B
    NULL,        // 0x0C
    NULL,        // 0x0D
    NULL,        // 0x0E  Alt BackSpace
    NULL,        // 0x0F  Shift Tab
    NULL,        // 0x10  Alt q
    NULL,        // 0x11  Alt w
    NULL,        // 0x12  Alt e
    NULL,        // 0x13  Alt r
    NULL,        // 0x14  Alt t
    NULL,        // 0x15  Alt y
    NULL,        // 0x16  Alt u
    NULL,        // 0x17  Alt i
    NULL,        // 0x18  Alt o
    NULL,        // 0x19  Alt p
    NULL,        // 0x1A  Alt [
    NULL,        // 0x1B  Alt ]
    NULL,        // 0x1C
    NULL,        // 0x1D
    NULL,        // 0x1E  Alt a
    NULL,        // 0x1F  Alt s
    NULL,        // 0x20  Alt d
    NULL,        // 0x21  Alt f
    NULL,        // 0x22  Alt g
    NULL,        // 0x23  Alt h
    NULL,        // 0x24  Alt j
    NULL,        // 0x25  Alt k
    NULL,        // 0x26  Alt l
    NULL,        // 0x27  Alt ;
    NULL,        // 0x28
    NULL,        // 0x29
    NULL,        // 0x2A
    NULL,        // 0x2B
    NULL,        // 0x2C  Alt z
    NULL,        // 0x2D  Alt x
    NULL,        // 0x2E  Alt c
    NULL,        // 0x2F  Alt v
    NULL,        // 0x30  Alt b
    NULL,        // 0x31  Alt n
    NULL,        // 0x32  Alt m
    NULL,        // 0x33
    NULL,        // 0x34
    NULL,        // 0x35
    NULL,        // 0x36
    NULL,        // 0x37  Alt Keypad *
    NULL,        // 0x38
    NULL,        // 0x39
    NULL,        // 0x3A
    "\x1b[a",    // 0x3B  F1
    "\x1b[b",    // 0x3C  F2
    "\x1b[c",    // 0x3D  F3
    "\x1b[d",    // 0x3E  F4
    "\x1b[e",    // 0x3F  F5
    "\x1b[f",    // 0x40  F6
    "\x1b[g",    // 0x41  F7
    "\x1b[h",    // 0x42  F8
    "\x1b[i",    // 0x43  F9
    "\x1b[j",    // 0x44  F10
    NULL,        // 0x45
    NULL,        // 0x46
    "\x1b[H",    // 0x47  Home
    "\x1b[A",    // 0x48  Up
    "\x1b[5~",   // 0x49  PgUp
    NULL,        // 0x4A  Alt Keypad -
    "\x1b[D",    // 0x4B  Left
    NULL,        // 0x4C
    "\x1b[C",    // 0x4D  Right
    NULL,        // 0x4E  Alt Keypad +
    "\x1b[F",    // 0x4F  End
    "\x1b[B",    // 0x50  Down
    "\x1b[6~",   // 0x51  PgDn
    NULL,        // 0x52  Ins
    "\x1b[3~",   // 0x53  Del
    NULL,        // 0x54  Shift F1
    NULL,        // 0x55  Shift F2
    NULL,        // 0x56  Shift F3
    NULL,        // 0x57  Shift F4
    NULL,        // 0x58  Shift F5
    NULL,        // 0x59  Shift F6
    NULL,        // 0x5A  Shift F7
    NULL,        // 0x5B  Shift F8
    NULL,        // 0x5C  Shift F9
    NULL,        // 0x5D  Shift F10
    NULL,        // 0x5E  Ctrl F1
    NULL,        // 0x5F  Ctrl F2
    NULL,        // 0x60  Ctrl F3
    NULL,        // 0x61  Ctrl F4
    NULL,        // 0x62  Ctrl F5
    NULL,        // 0x63  Ctrl F6
    NULL,        // 0x64  Ctrl F7
    NULL,        // 0x65  Ctrl F8
    NULL,        // 0x66  Ctrl F9
    NULL,        // 0x67  Ctrl F10
    NULL,        // 0x68  Alt F1
    NULL,        // 0x69  Alt F2
    NULL,        // 0x6A  Alt F3
    NULL,        // 0x6B  Alt F4
    NULL,        // 0x6C  Alt F5
    NULL,        // 0x6D  Alt F6
    NULL,        // 0x6E  Alt F7
    NULL,        // 0x6F  Alt F8
    NULL,        // 0x70  Alt F9
    NULL,        // 0x71  Alt F10
    NULL,        // 0x72  Ctrl PrtSc
    NULL,        // 0x73  Ctrl Left
    NULL,        // 0x74  Ctrl Right
    NULL,        // 0x75  Ctrl End
    NULL,        // 0x76  Ctrl PgDn
    NULL,        // 0x77  Ctrl Home
    NULL,        // 0x78  Alt 1
    NULL,        // 0x79  Alt 2
    NULL,        // 0x7A  Alt 3
    NULL,        // 0x7B  Alt 4
    NULL,        // 0x7C  Alt 5
    NULL,        // 0x7D  Alt 6
    NULL,        // 0x7E  Alt 7
    NULL,        // 0x7F  Alt 8
    NULL,        // 0x80  Alt 9
    NULL,        // 0x81  Alt 0
    NULL,        // 0x82  Alt -
    NULL,        // 0x83  Alt =
    NULL,        // 0x84  Ctrl PgUp
    "\x1b[k",    // 0x85  F11
    "\x1b[l",    // 0x86  F12
    NULL,        // 0x87  Shift F11
    NULL,        // 0x88  Shift F12
    NULL,        // 0x89  Ctrl F11
    NULL,        // 0x8A  Ctrl F12
    NULL,        // 0x8B  Alt F11
    NULL,        // 0x8C  Alt F12
    NULL,        // 0x8D  Ctrl Up
    NULL,        // 0x8E  Ctrl Keypad -
    NULL,        // 0x8F  Ctrl Keypad 5
    NULL,        // 0x90
    NULL,        // 0x91  Ctrl Down
    NULL,        // 0x92  Ctrl Ins
    NULL,        // 0x93  Ctrl Del
    NULL,        // 0x94  Ctrl Tab
    NULL,        // 0x95  Ctrl Keypad /
    NULL,        // 0x96  Ctrl Keypad *
    NULL,        // 0x97  Alt Home
    NULL,        // 0x98  Alt Up
    NULL,        // 0x99  Alt PgDn
    NULL,        // 0x9A
    NULL,        // 0x9B  Alt Left
    NULL,        // 0x9C
    NULL,        // 0x9D  Alt Right
    NULL,        // 0x9E
    NULL,        // 0x9F  Alt End
    NULL,        // 0xA0  Alt Down
    NULL,        // 0xA1  Alt PgDn
    NULL,        // 0xA2  Alt Ins
    NULL,        // 0xA3  Alt Del
    NULL,        // 0xA4  Alt Keypad /
    NULL,        // 0xA5  Alt Tab
    NULL,        // 0xA6  Alt Enter
    NULL,        // 0xA7
    NULL,        // 0xA8
    NULL,        // 0xA9
    NULL,        // 0xAA
    NULL,        // 0xAB
    NULL,        // 0xAC
    NULL,        // 0xAD
    NULL,        // 0xAE
    NULL,        // 0xAF
    NULL,        // 0xB0
    NULL,        // 0xB1
    NULL,        // 0xB2
    NULL,        // 0xB3
    NULL,        // 0xB4
    NULL,        // 0xB5
    NULL,        // 0xB6
    NULL,        // 0xB7
    NULL,        // 0xB8
    NULL,        // 0xB9
    NULL,        // 0xBA
    NULL,        // 0xBB
    NULL,        // 0xBC
    NULL,        // 0xBD
    NULL,        // 0xBE
    NULL,        // 0xBF
    NULL,        // 0xC0
    NULL,        // 0xC1
    NULL,        // 0xC2
    NULL,        // 0xC3
    NULL,        // 0xC4
    NULL,        // 0xC5
    NULL,        // 0xC6
    NULL,        // 0xC7
    NULL,        // 0xC8
    NULL,        // 0xC9
    NULL,        // 0xCA
    NULL,        // 0xCB
    NULL,        // 0xCC
    NULL,        // 0xCD
    NULL,        // 0xCE
    NULL,        // 0xCF
    NULL,        // 0xD0
    NULL,        // 0xD1
    NULL,        // 0xD2
    NULL,        // 0xD3
    NULL,        // 0xD4
    NULL,        // 0xD5
    NULL,        // 0xD6
    NULL,        // 0xD7
    NULL,        // 0xD8
    NULL,        // 0xD9
    NULL,        // 0xDA
    NULL,        // 0xDB
    NULL,        // 0xDC
    NULL,        // 0xDD
    NULL,        // 0xDE
    NULL,        // 0xDF
    NULL,        // 0xE0
    NULL,        // 0xE1
    NULL,        // 0xE2
    NULL,        // 0xE3
    NULL,        // 0xE4
    NULL,        // 0xE5
    NULL,        // 0xE6
    NULL,        // 0xE7
    NULL,        // 0xE8
    NULL,        // 0xE9
    NULL,        // 0xEA
    NULL,        // 0xEB
    NULL,        // 0xEC
    NULL,        // 0xED
    NULL,        // 0xEE
    NULL,        // 0xEF
    NULL,        // 0xF0
    NULL,        // 0xF1
    NULL,        // 0xF2
    NULL,        // 0xF3
    NULL,        // 0xF4
    NULL,        // 0xF5
    NULL,        // 0xF6
    NULL,        // 0xF7
    NULL,        // 0xF8
    NULL,        // 0xF9
    NULL,        // 0xFA
    NULL,        // 0xFB
    NULL,        // 0xFC
    NULL,        // 0xFD
    NULL,        // 0xFE
    NULL         // 0xFF
};

LOGMODULE("charconv");

CCharConv::CCharConv() {

    m_nOperatingMode = Monitor;

}

CCharConv::~CCharConv() {

}

void CCharConv::SetOperatingMode(TOperatingMode nOperatingMode) {

    m_nOperatingMode = nOperatingMode;

}

char CCharConv::Convert(char c) {

    // convert between \r and \n
    switch(m_nOperatingMode) {
        case Monitor: // MONITOR converts \r -> \n itself
        case ELKS:
        case DOS:
        case CPM:
            // replace new line with carriage return
            if (c == '\n') {
                c = '\r';
            }
        break;

        case TBasic:
            // replace carriage return with new line
            if (c == '\r') {
                c = '\n';
            }
        break;

        case RAW:
            // do nothing
        break;
    }

    // convert between DEL and BS (^H)
    switch(m_nOperatingMode) {
        case Monitor:
        case ELKS:   // ELKS seems happy with DEL or BS
        case TBasic:
            // replace BS with DEL
            if (c == 0x8) {
                c = 0x7f;
            }
        break;

        case DOS:
        case CPM:
            // replace DEL with BS
            if (c == 0x7f) {
                c = 0x8;
            }
        break;

        case RAW:
            // do nothing
        break;
    }

    return c;

}

u16 CCharConv::ScanCode(char c) {

    u16 s = 0;

    if ((c >= 0) and (c <= 127)) {
        s = scancodes[(u8) c];
    }

    return s;

}

const char *CCharConv::EscapeSequence(u16 nScanCode) {

    u8 sc = (nScanCode & 0xFF00) >> 8;
    const char *s = esc[sc];

    return s;

}

void CCharConv::AddSafe(CRingBuf<u16> *pRingBuf, u16 nScanCode) {

    if (nScanCode == 0) {
        return;
    }

    const char *e;

    switch(m_nOperatingMode) {
        case Monitor:
        case TBasic:
        case CPM:
        case RAW:
            // only interested in the ascii value
            // skip special chars (e.g. ascii == 0)
            nScanCode &= 0x00FF;
            if (nScanCode) {
                pRingBuf->AddSafe(nScanCode);
            }
            break;

        case ELKS:
            if (nScanCode & 0x00FF) {
                // regular ascii value
                pRingBuf->AddSafe(nScanCode & 0x00FF);
            } else {
                // special char, convert to escape sequence
                e = EscapeSequence(nScanCode);
                while (e and *e) {
                    pRingBuf->AddSafe(*e);
                    e++;
                }
            }
        break;

        case DOS:
            // keep the ascii value and scan code for DOS
            pRingBuf->AddSafe(nScanCode);
        break;
    }

}