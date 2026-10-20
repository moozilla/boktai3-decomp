#include "global.h"

struct RfuMgr {
    u8 pad0[4];
    u8 f4;
    u8 pad5;
    u8 f6;
    u8 f7;
    u8 pad8[0x12];
    u16 f1A;
    u16 f1C;
};
extern struct RfuMgr gUnk_03005390;
u32 sub_08249498(u32, u32);

void RfuManager_StepSlotTimer(u32 a)
{
    if (gUnk_03005390.f7 == 5) {
        gUnk_03005390.f6 = 1;
        gUnk_03005390.f4 = 5;
        gUnk_03005390.f1A = gUnk_03005390.f1C;
        if (gUnk_03005390.f1A != 0) {
            gUnk_03005390.f7 = 6;
        } else {
            gUnk_03005390.f7 = 1;
        }
    }
    if (gUnk_03005390.f7 == 1) {
        gUnk_03005390.f6 = 1;
        gUnk_03005390.f4 = 5;
        gUnk_03005390.f1A = sub_08249498(a, 0x8c);
        gUnk_03005390.f1C = 0x8c - gUnk_03005390.f1A;
        if (gUnk_03005390.f1A != 0) {
            gUnk_03005390.f7 = 2;
        } else {
            gUnk_03005390.f7 = 3;
        }
    }
    if (gUnk_03005390.f7 == 3) {
        gUnk_03005390.f6 = 0;
        gUnk_03005390.f1A = 0x28;
        gUnk_03005390.f7 = 4;
        gUnk_03005390.f4 = 9;
    }
}
