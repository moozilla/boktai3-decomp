#include "global.h"
struct S {
    u8 pad0[2];
    volatile u8 f2;
    u8 pad3;
    u8 f4;
    u8 f5;
    u8 pad6[0xa];
    u8 f10;
    u8 pad11[3];
    u16 f14;
    u8 pad16[0xe];
    u8 f24;
    u8 pad25[3];
    u16 a28[4];
};
struct L {
    u8 pad[2];
    u8 f2;
    u8 f3;
};
extern struct S gUnk_03005390;
extern u16 *gUnk_03006A70[];
extern struct L *gUnk_03006A80;
void rfu_clearSlot(u32, u32);
void RfuManager_NotifyEvent(u32, u32);
void RfuManager_RequestMscCycleCompletion(void);
void RfuManager_DisconnectAndWait(u32);
// confidence=low: counts down the slot timeout and tears the link down when it expires.
void sub_082247CC(void)
{
    u16 ime = *(vu16 *)0x04000208;
    *(vu16 *)0x04000208 = 0;
    if (gUnk_03005390.f4 == 0xf) {
        if (--gUnk_03005390.a28[gUnk_03005390.f10] == 0 ||
            *gUnk_03006A70[gUnk_03005390.f10] == 0x27) {
            RfuManager_RequestMscCycleCompletion();
            gUnk_03005390.f4 = 0x18;
            rfu_clearSlot(4, gUnk_03005390.f10);
            gUnk_03005390.f24 &= ~(1 << gUnk_03005390.f10);
            gUnk_03005390.a28[gUnk_03005390.f10] = 0;
        }
    }
    *(vu16 *)0x04000208 = ime;
    if (gUnk_03005390.f4 == 0x18) {
        if (gUnk_03005390.f2 == 1)
            RfuManager_RequestMscCycleCompletion();
        if (gUnk_03005390.f2 == 0) {
            gUnk_03005390.f5 = 0;
            gUnk_03005390.f4 = 0;
            RfuManager_DisconnectAndWait(gUnk_03006A80->f2 | gUnk_03006A80->f3);
            gUnk_03005390.f14 = 0;
            RfuManager_NotifyEvent(0x25, 1);
        }
    }
}
