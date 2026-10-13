#include "global.h"
#include "game_state.h"

struct P081963B8 {
    u8 f0[0x71be];
    u8 b71be;
    u8 f1[0x7244 - 0x71bf];
    s16 timer;
    u8 f2[0x724a - 0x7246];
    u8 state;
    u8 f3[2];
    u8 b724d;
    u8 b724e;
    u8 f4[0x7276 - 0x724f];
    s16 h7276;
    u8 f5[0x728a - 0x7278];
    s16 h728a;
    s16 h728c;
};
extern u32 gUnk_030053F4, gUnk_030053F0;
extern u32 gUnk_02000560;
void sub_081945AC(void *);
s32 sub_08194624(void *);
void sub_082260A4(u32);
void sub_081936F8(void *);
void sub_08195A14(void *);
void sub_081959F0(void *);
void sub_081945FC(void *);
void sub_081937F8(void *);
void sub_0819382C(void *);
void sub_081945D0(void *);
s32 sub_08194640(void *);
void sub_08195A4C(void *, u32);
void sub_08190670(void *);
void Sound_PlaySongChecked(u32);
void Sound_StopSong(u32);

static inline u32 Tst2(u32 mask, u32 *a, u32 *b)
{
    return (*a | *b) & mask;
}

static inline u32 Tst32(u32 mask, u32 *p)
{
    return *p & mask;
}

static inline void Track(struct P081963B8 *t)
{
    if (gUnk_02000710->positionX > 0xeff)
        sub_08195A14(t);
    else
        sub_081959F0(t);
}

void sub_081963B8(struct P081963B8 *t)
{
    switch (t->state) {
    case 0:
        sub_081945AC(t);
        t->state++;
        break;
    case 1:
        if (sub_08194624(t)) {
            t->state++;
            t->timer = 0;
        }
        break;
    case 2:
        if (t->timer <= 9) {
            sub_082260A4(0x14);
            break;
        }
        sub_081936F8(t);
        t->timer = 0;
        t->state++;
        t->b71be = 1;
        break;
    case 3:
        Track(t);
        if (t->timer > 0x1a3) {
            sub_081945FC(t);
            Sound_PlaySongChecked(0x1c1);
            t->state++;
        }
        break;
    case 4:
        Track(t);
        if (!(t->timer & 7))
            t->b724e |= 3;
        if (t->timer > 0x257) {
            t->b71be = 0;
            if (gUnk_02000710->positionX > 0xeff)
                sub_081937F8(t);
            else
                sub_0819382C(t);
            sub_081945D0(t);
            Sound_StopSong(0x1c1);
            t->state++;
        }
        break;
    case 5:
        Track(t);
        t->b724e |= 3;
        if (sub_08194640(t))
            t->state++;
        break;
    case 6:
        if (gUnk_02000710->positionX > 0xeff) {
            if ((t->h728a <= 0 && t->h728c <= 0) || t->h7276 <= 0)
                sub_08195A4C(t, 12);
            else
                sub_08195A4C(t, 10);
        } else {
            sub_08190670(t);
            t->state++;
        }
        break;
    case 7:
        if (!t->b724d) {
            if (!Tst2(0x400, &gUnk_030053F4, &gUnk_030053F0) && !Tst32(4, &gUnk_02000560))
                sub_08190670(t);
            else {
                sub_081937F8(t);
                t->state++;
            }
        }
        break;
    case 8:
        break;
    }
}
