#include "global.h"

#define RfuManager_StartChildConnect sub_082237A0

struct RfuMgr {
    u8 pad0[4];
    u8 f4;
    u8 f5;
    u8 pad6;
    u8 f7;
    u8 pad8[0xc];
    u16 f14;
    u8 pad16[4];
    u16 f1A;
    u8 pad1C[2];
    u16 f1E;
};
struct RfuLink {
    u8 pad0[8];
    u8 count;
    u8 pad9[11];
    u16 id;
    u8 pad16[10];
};
extern struct RfuMgr gUnk_03005390;
extern struct RfuLink *gUnk_03006A80;
u32 rfu_getMasterSlave(void);
void RfuManager_NotifyEvent(u32, u32);

u32 RfuManager_StartChildConnect(u16 id, u16 b)
{
    u8 i;
    u32 count;
    if (gUnk_03005390.f4 != 0 && (u8)(gUnk_03005390.f4 - 9) > 2) {
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
    *(&count) = gUnk_03006A80->count;
    if (i < count && gUnk_03006A80[0].id != id) {
        struct RfuLink *l = gUnk_03006A80;
        do {
            i++;
            if (i >= count)
                break;
        } while (((struct RfuLink *)((u8 *)l + (i << 5)))->id != id);
    }
    count = gUnk_03006A80->count;
    if (count == 0 || i == count) {
        gUnk_03005390.f14 = 3;
        RfuManager_NotifyEvent(0xf3, 1);
        return 3;
    }
    if (gUnk_03005390.f4 == 0 || gUnk_03005390.f4 == 9) {
        do {
            gUnk_03005390.f4 = 0xc;
            gUnk_03005390.f5 = 0xd;
        } while (0);
    } else {
        gUnk_03005390.f4 = 0xb;
        gUnk_03005390.f5 = 0xc;
    }
    gUnk_03005390.f1E = id;
    gUnk_03005390.f1A = b;
    if (gUnk_03005390.f7 != 0)
        gUnk_03005390.f7 = 7;
    return 0;
}
