#include "global.h"

#define DisplayEntry_SetAnim sub_081D4990

struct DispInfo {
    u8 pad[0x1C];
    s8 f1C;
    s8 f1D;
    u8 f1E;
    u8 f1F;
};

struct DispA {
    u8 pad[0x10];
    u16 f10;
};

struct DispObj {
    u8 pad0[0xB4];
    u16 *fB4;
    u8 pad1[0x9A4 - 0xB8];
    u8 pad2[0x9AC - 0x9A4];
    u8 f9AC;
    u8 f9AD;
    u8 pad2b[0x9B4 - 0x9AE];
    u16 f9B4;
    u8 pad3[0x9C0 - 0x9B6];
    u16 f9C0;
    u16 f9C2;
    u16 f9C4;
    u8 pad4[0x9EE - 0x9C6];
    u16 f9EE;
    u8 pad5[0xA08 - 0x9F0];
    struct DispInfo *fA08;
};

void DisplayEntry_SetAnim(struct DispObj *p, s32 mode)
{
    switch (mode) {
        case 1:
            ((struct DispA *)((u8 *)p + 0x9A4))->f10 = 5;
            break;
        case 2:
            ((struct DispA *)((u8 *)p + 0x9A4))->f10 = 6;
            break;
        case 3:
            ((struct DispA *)((u8 *)p + 0x9A4))->f10 = 7;
            break;
    }
    p->f9C0 = p->fB4[0] + p->fA08->f1C;
    p->f9C2 = p->f9EE + p->fA08->f1D;
    p->f9C4 = p->fB4[2];
    p->f9AC = p->fA08->f1E;
    p->f9AD = p->fA08->f1F;
}
