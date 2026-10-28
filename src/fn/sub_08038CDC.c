#include "global.h"
#include "player.h"
struct Entry {
    u8 bytes[4], a, b, flag, kind;
    u16 timer, pad;
};
struct P {
    u8 pad[0x20];
    u32 flags, timer;
    u8 state, entered;
    u8 pad2[0x12];
    struct Entry entry;
};
extern u32 gUnk_02000560;
void PaletteLevels_Reset(void);
void WirelessExchange_CaptureLocalResult(struct P *);
int WirelessExchange_SendPacket(struct P *, int, const u32 *, int);
void WirelessGameMenu_SetStatusPanelState(struct P *, int);
void Sound_FadeOutChannel10(int);
void PlayerCue_Stand(void *, int);
static inline u32 test(u32 *flags, u32 mask)
{
    return *flags & mask;
}
void WirelessGameMenu_UpdateRoundResultExchange(struct P *p)
{
    u32 data;
    if (p->entered) {
        p->entered = 0;
        PaletteLevels_Reset();
        WirelessExchange_CaptureLocalResult(p);
        {
            struct Entry *e = &p->entry;
            data = e->timer | ((e->kind & 3) << 16);
        }
        WirelessExchange_SendPacket(p, 4, &data, 1);
        WirelessGameMenu_SetStatusPanelState(p, 5);
        Sound_FadeOutChannel10(4);
        {
            u32 mask = 4;
            if (!(gUnk_02000560 & mask))
                PlayerCue_Stand((*(void **)gPlayers), -1);
        }
    }
    if (p->timer > 29 && test(&p->flags, 0xc0) == 0xc0) {
        u32 nine;
        u8 *state;
        u32 zero;
        p->flags &= ~0xc0;
        nine = 9;
        state = &p->state;
        zero = 0;
        *state = nine;
        p->entered = 1;
        p->timer = zero;
    } else
        p->timer++;
}
