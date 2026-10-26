#include "global.h"
#include "render.h"
struct P {
    u8 pad[0x20];
    u32 flags, timer;
    u8 state, entered;
    u8 pad2[0x18];
    u8 flag;
};
void PaletteLevels_Reset(void);
int WirelessExchange_SendPacket(struct P *, int, const u32 *, int);
void WirelessGameMenu_SetStatusPanelState(struct P *, int);
void WirelessGameMenu_SignalStatusPanel(struct P *);
void WirelessGameItems_StartOutgoingItems(void);
void Hud_ClearTimer(void);
static inline void set(u32 mask)
{
    gEnginePauseFlags |= mask;
}
void WirelessGameMenu_UpdateRoundHandshake(struct P *p)
{
    u32 data;
    u32 bits;
    u32 flags;
    if (p->entered) {
        u32 zero = 0;
        p->entered = zero;
        PaletteLevels_Reset();
        data = zero;
        WirelessExchange_SendPacket(p, 1, &data, 1);
        set(4);
        WirelessGameMenu_SetStatusPanelState(p, 1);
        p->flag = zero;
        WirelessGameItems_StartOutgoingItems();
    }
    bits = 128;
    flags = p->flags;
    bits &= flags;
    if (bits) {
        u32 eight;
        u8 *state;
        u32 zero;
        Hud_ClearTimer();
        eight = 8;
        state = &p->state;
        zero = 0;
        *state = eight;
        p->entered = 1;
        p->timer = zero;
    } else {
        u32 three = 3;
        three &= flags;
        if (three != 3)
            goto tick;
        p->flags = flags & ~3;
        p->state = three;
        p->entered = 1;
        p->timer = bits;
    }
    WirelessGameMenu_SignalStatusPanel(p);
    return;
tick:
    p->timer++;
}
