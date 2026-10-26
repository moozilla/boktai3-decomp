#include "global.h"
#include "render.h"
struct P {
    u8 pad[0x1e];
    u16 rounds;
    u8 pad2[4];
    u32 timer;
    u8 state, entered;
    u8 pad3[0xc2];
    u8 mode;
};
struct Save {
    u8 pad[0x52];
    s16 selected;
};
extern struct Save *gUnk_02000710;
void WirelessGameMenu_SetStatusPanelState(struct P *, int);
void WirelessGameMenu_SignalStatusPanel(struct P *);
void Player_EnterStateTen(void);
void Player_EnterStateEleven(void);
void WirelessGame_GetSelectedEquipmentEffect(int);
void Player_SetTransitionMode(void);
static inline int item(void)
{
    struct Save *s = gUnk_02000710;
    return *(s16 *)((u8 *)s + s->selected * 2 + 0x88);
}
void WirelessGameMenu_UpdateRoundPreparation(struct P *p)
{
    if (p->entered) {
        p->entered = 0;
        p->rounds++;
        if (p->rounds > 98)
            p->rounds = 99;
        WirelessGameMenu_SetStatusPanelState(p, 3);
    }
    if (p->timer > 89) {
        u32 seven = 7;
        u8 *state = &p->state;
        u32 zero = 0;
        u32 one;
        u8 *entered;
        u8 *mode;
        *state = seven;
        entered = &p->entered;
        one = 1;
        *entered = one;
        p->timer = zero;
        {
            u32 *global = &gEnginePauseFlags;
            u32 mask = ~4;
            *global &= mask;
        }
        WirelessGameMenu_SignalStatusPanel(p);
        mode = &p->mode;
        switch (*mode) {
            case 0: {
                int n = item();
                if (n == 1)
                    goto mode2;
                if (n != 2)
                    goto done;
                goto mode1;
            }
            case 2:
                if (item() == 2) {
                mode1:
                    *mode = one;
                    Player_EnterStateTen();
                } else {
                    *mode = zero;
                    Player_EnterStateEleven();
                }
                break;
            case 1:
                if (item() == 1) {
                mode2:
                    *mode = 2;
                    Player_EnterStateEleven();
                } else {
                    *mode = zero;
                    Player_EnterStateTen();
                }
                break;
        }
    done:
        WirelessGame_GetSelectedEquipmentEffect(item());
        Player_SetTransitionMode();
    } else
        p->timer++;
}
