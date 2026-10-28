#include "global.h"
#include "area_effect_link.h"
#define Link_GetState Registry1745_GetSessionState
struct LinkPanel {
    u8 pad0[0xF50];
    s32 peerIndex;
};
s32 Registry1745_GetSessionState(void);
static inline u8 connected(void)
{
    if ((*(u8 **)&gLinkSession) && (*(u8 **)&gLinkSession)[0x1D])
        return 1;
    return 0;
}
static inline s32 peerStatus(s32 i)
{
    if (!(*(u8 **)&gLinkSession))
        return 0;
    {
        u8 *statuses = (*(u8 **)&gLinkSession) + 0x64;
        return *(statuses + i);
    }
}
s32 LinkPanel_GetConnectionStatus(struct LinkPanel *p)
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
