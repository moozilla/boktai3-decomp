#include "global.h"

// Sets the character-base block (BGxCNT bits 2-3) of background `bg`.
void sub_08215AC4(s32 bg, u32 charBase)
{
    switch (bg) {
        case 0: {
            u16 v = (*(vu16 *)0x04000008 & 0xFFF3) | (charBase << 2);
            *(vu16 *)0x04000008 = v;
        } break;
        case 1: {
            u16 v = (*(vu16 *)0x0400000A & 0xFFF3) | (charBase << 2);
            *(vu16 *)0x0400000A = v;
        } break;
        case 2: {
            u16 v = (*(vu16 *)0x0400000C & 0xFFF3) | (charBase << 2);
            *(vu16 *)0x0400000C = v;
        } break;
        case 3: {
            u16 v = (*(vu16 *)0x0400000E & 0xFFF3) | (charBase << 2);
            *(vu16 *)0x0400000E = v;
        } break;
    }
}
