#include "global.h"

struct RfuMgr {
    u8 pad0[4];
    u8 f4;
    u8 pad5;
    u8 f6;
    u8 f7;
    u8 pad8[3];
    u8 fB;
    u8 padC[8];
    u16 f14;
    u8 pad16[4];
    u16 f1A;
    u8 pad1C[4];
    u32 f20;
    u16 pad24;
    u16 f26;
};
extern struct RfuMgr gUnk_03005390;
u32 rfu_getMasterSlave(void);
void RfuManager_NotifyEvent(u32, u32);

u32 RfuManager_StartConnect(u8 a, u16 b, u16 c, u16 *list)
{
    u8 i;
    u16 *p;
    if (gUnk_03005390.f4 != 0 && (gUnk_03005390.f4 != 8 || a != 1)) {
        gUnk_03005390.f14 = 1;
        RfuManager_NotifyEvent(0xf3, 1);
        return 1;
    }
    if ((u8)rfu_getMasterSlave() == 0) {
        gUnk_03005390.f14 = 2;
        RfuManager_NotifyEvent(0xf3, 1);
        return 2;
    }
    i = 0;
    p = list;
    if (*p++ != 0xFFFF) {
        do {
            i++;
            if (i > 0xf)
                break;
        } while (*p++ != 0xFFFF);
    }
    if (i == 0x10) {
        gUnk_03005390.f14 = 4;
        RfuManager_NotifyEvent(0xf3, 1);
        return 4;
    }
    if (a > 1) {
        gUnk_03005390.f7 = 1;
        a = 1;
        b = 0;
    } else {
        gUnk_03005390.f7 = 0;
    }
    if (a != 0) {
        gUnk_03005390.f4 = 5;
    } else {
        gUnk_03005390.f4 = 9;
        if (gUnk_03005390.fB != 0)
            gUnk_03005390.fB = 2;
    }
    gUnk_03005390.f6 = a;
    gUnk_03005390.f1A = b;
    gUnk_03005390.f26 = c;
    gUnk_03005390.f20 = (u32)list;
    return 0;
}
