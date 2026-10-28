#include "global.h"
#include "ram.h"
struct P {
    u8 pad[0x1c];
    u8 b1c;
    u8 b1d;
    u8 pad1[0x14];
    u8 b32;
    u8 b33;
    u8 pad2[4];
    u16 h38;
    u8 pad3[10];
    u32 w44;
    u32 w48;
    u32 w4c;
    u8 pad5[0x338];
    u32 w388;
};
s32 Wireless_InitRfuApi(void);
void WirelessController_ResetSession(struct P *);
u32 Script_GetKeywordValue(u32, u32);
s32 Script_SeekToKeyword(s32);
u32 Script_GetTextSourcePc(void);
s32 Wireless_InitControllerFromScript(struct P *p)
{
    u32 z;
    u8 *f = &p->b33;
    z = 0;
    *f = z;
    gWirelessLastStatus = z;
    if (Wireless_InitRfuApi() < 0)
        goto fail;
    p->b1d = z;
    WirelessController_ResetSession(p);
    {
        u32 one = 1;
        p->b1c = z;
        p->h38 = one;
        p->w44 = z;
        p->b32 = 1;
    }
    p->w4c = Script_GetKeywordValue(0x70, 0);
    if (!Script_SeekToKeyword(0x61))
        goto fail;
    {
        u32 v = Script_GetTextSourcePc();
        p->w388 = v;
        if (v == 0) {
        fail:
            return -1;
        }
    }
    return 0;
}
