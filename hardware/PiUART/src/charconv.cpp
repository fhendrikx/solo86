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