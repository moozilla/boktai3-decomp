#include "global.h"
#define gRfuLinkStatus gUnk_03006A80
#include "rfu_compat.h"
extern struct Manager gUnk_03005390;
#define G (&gUnk_03005390)
struct Manager;
struct Manager {
    u8 f0, f1;
    volatile u8 f2;
    u8 f3, f4, f5, f6, f7, f8, f9, fa, fb, fc, fd, fe, ff;
    u8 f10, f11, f12, p13;
    u16 f14, f16, f18, f1a, f1c, f1e;
    const u16 *allowed;
    u8 f24, p25;
    u16 f26, a28[4];
    u8 f30, p31;
    u16 f32;
    u16 a34[4];
};
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_DisconnectAndWait(u8);

u8 sub_08224BC4(u8 enable, u16 period)
{
    u16 ime;
    if (G->f9 && enable == 0 && G->f30)
        return 5;
    ime = REG_IME;
    REG_IME = 0;
    G->f9 = enable;
    G->f32 = period;
    REG_IME = ime;
    return 0;
}
