#include "global.h"
#include "render.h"
struct Entry {
    u8 bytes[4], a, b, flag, kind;
    u32 timer;
};
struct P {
    u8 pad[0x1c];
    u16 remaining;
    u8 pad2[2];
    u32 flags, timer;
    u8 state, entered;
    u8 pad3[2];
    u32 status;
    u8 pad4[7];
    u8 active;
    u8 scores[4];
    struct Entry entry;
};
struct Actor {
    u8 pad[0x424];
    u16 current, maximum;
};
struct Target {
    u8 pad[0x94];
    s16 current, maximum;
};
extern struct Actor *gUnk_02000580;
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void Hud_RegisterTimer(u16 *);
void WirelessGameMenu_SetStatusPanelState(struct P *, int);
void Sound_PlayTrackedSong(int);
void WirelessGameMenu_UpdateHealthIndicators(struct P *);
void WirelessGameMenu_SignalStatusPanel(struct P *);
int Mod(int, int);
int Div(int, int);
struct Target *SceneActors_GetBySlot(int);
void WirelessGame_AddWeightedScore(int, int);
int WirelessExchange_SendPacket(struct P *, int, const u32 *, int);
void Hud_ClearTimer(void);
void Sound_PlaySong(int);
static inline u32 test(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline void set(u32 mask)
{
    gEnginePauseFlags |= mask;
}
void WirelessGameMenu_UpdateActiveRound(struct P *input)
{
    struct P *p = input;
    u32 data;
    u8 *entered = &p->entered;
    if (*entered) {
        *entered = 0;
        {
            u32 value = 720;
            struct P *view;
            view = p;
            view->remaining = value;
        }
        Hud_RegisterTimer(&p->remaining);
        WirelessGameMenu_SetStatusPanelState(p, 4);
        Sound_PlayTrackedSong(22);
        p->active = 1;
    }
    WirelessGameMenu_UpdateHealthIndicators(p);
    if (((({
            struct P *view = p;
            view->timer;
        }))) == 30) {
        WirelessGameMenu_SignalStatusPanel(p);
    }
    if (Mod((({
                struct P *view = p;
                view->timer;
            })),
            60) == 0) {
        struct Actor *actor = gUnk_02000580;
        struct Target *target = SceneActors_GetBySlot(0);
        int a, b;
        struct Entry *e;
        u32 packed;
        int total;
        int i;
        int shift;
        u8 *q;
        if (actor && actor->maximum) {
            a = Div(100 * actor->current, actor->maximum);
        } else {
            a = 0;
        }
        if ((a == 0) && actor->current) {
            a = 1;
        }
        if (target && (target->maximum > 0)) {
            b = Div(100 * target->current, target->maximum);
        } else {
            b = 0;
        }
        do {
            if ((b == 0) && (target->current > 0)) {
                b = 1;
            }
            data = a | (b << 8);
            e = &p->entry;
            {
                u32 zero = 0;
                packed = zero;
                total = zero;
            }
            i = 0;
            {
                u32 zero = 0;
                shift = zero;
            }
            q = e->bytes;
        } while (0);
        do {
            int value;
            u32 flag = e->flag;
            if (flag) {
                value = *q;
            } else {
                int old = *q;
                u8 *score;
                int n;
                score = p->scores;
                score += i;
                n = *score;
                value = old + n;
                total += n;
                *score = (float)flag;
            }
            e->bytes[i] = 0;
            if (value > 15) {
                value = 15;
            }
            {
                int count = shift;
                u32 merged;
                value <<= count;
                merged = packed;
                merged |= value;
                packed = merged;
            }
            q++;
            i++;
            shift += 4;
        } while (i <= 3);
        if (((({
                int v = total;
                v;
            }))) > 0) {
            WirelessGame_AddWeightedScore(3, 1);
        }
        {
            u32 v = packed;
            u32 shifted;
            shifted = v << 16;
            data |= shifted;
        }
        WirelessExchange_SendPacket(p, 0, &data, 1);
    }
    {
        struct P *view = p;
        if ((view->status & 0xffff00) || test(&view->flags, 128)) {
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
            return;
        }
    }
    {
        struct P *view = p;
        u32 remaining;
        remaining = --view->remaining;
        if (!remaining) {
            u32 two;
            u8 *state;
            Hud_ClearTimer();
            Sound_PlaySong(0x372);
            two = 2;
            state = &p->state;
            *state = two;
            p->entered = 1;
            {
                struct P *view = p;
                view->timer = remaining;
            }
            set(4);
            p->active = 0;
        } else {
            struct P *view = p;
            view->timer++;
        }
    }
}
