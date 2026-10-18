#include "global.h"
#define LinkPanel_GetConnectionStatus sub_0810C398
#define Link_GetState Registry1745_GetSessionState
#define gLinkSession                  gUnk_020004B4
struct LinkPanel {
    u8 pad0[0xF50];
    s32 peerIndex;
};
extern u8 *gUnk_020004B4;
s32 Registry1745_GetSessionState(void);
static inline u8 connected(void)
{
    if (gUnk_020004B4 && gUnk_020004B4[0x1D])
        return 1;
    return 0;
}
static inline s32 peerStatus(s32 i)
{
    if (!gUnk_020004B4)
        return 0;
    {
        u8 *statuses = gUnk_020004B4 + 0x64;
        return *(statuses + i);
    }
}
s32 sub_0810C398(struct LinkPanel *p)
{
    s32 state, result, status;
    if (p->peerIndex < 0) {
    two:
        return 2;
    }
    state = Registry1745_GetSessionState();
    if ((u32)(state - 5) <= 1)
        return 4;
    if (connected() && state == 8)
        return 6;
    status = peerStatus(p->peerIndex);
    if (status == 2)
        goto two;
    if (status <= 2)
        goto lower;
    if (status == 3)
        goto three;
    goto bad;
lower:
    if (status < 0)
        goto bad;
    result = 1;
    goto end;
three:
    result = 3;
    goto end;
bad:
    result = -1;
end:
    return result;
}
