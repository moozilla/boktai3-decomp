#include "global.h"

struct P08195C20 {
    u8 f0[0x18];
    u8 queue[0x5404 - 0x18];
    s16 x;
    s16 pad;
    s16 y;
    u8 f1[0x7246 - 0x540a];
    u16 timer;
    u8 active;
    u8 f2;
    u8 state;
    u8 f3[0x7278 - 0x724b];
    s16 hp[9];
    u8 f4[0x72a8 - 0x728a];
    s32 a, b, c, d, e, f;
};
void Nidhoggr_SetBackgroundMode(void *, u32);
void Nidhoggr_InitBobbingMotion(void *, s32, s32);
void Nidhoggr_UpdateAnchoredSway(void *, s32, s32);
void Nidhoggr_ActivateRandomPart(void *);
void BossAttack_ActivateCollider(void *);
void Nidhoggr_CreateLowerSweepBurst(void *);
void Nidhoggr_SetBehavior(void *, u32);
void Sound_PlaySongChecked(u32);

void Nidhoggr_UpdateFirstPartPhase(struct P08195C20 *t)
{
    s32 allDead, i;
    s16 *hp;
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 0);
            Nidhoggr_InitBobbingMotion(t, 0x200, 0xf80);
            if (t->a < -0x20000)
                t->state = 1;
            else {
                t->state = 2;
                t->timer = 0;
            }
            break;
        case 1:
            t->a += t->d;
            if (t->a > -0x20000) {
                t->state = 2;
                t->timer = 0;
            }
            t->d += 6;
            if (t->d > 0x800)
                t->d = 0x800;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0xf80);
            break;
        case 2:
            t->state++;
            Sound_PlaySongChecked(0x403);
        case 3:
            Nidhoggr_ActivateRandomPart(t);
            t->a += t->d;
            if (t->a < -0x20000) {
                t->a = -0x20000;
                t->d = 0;
            } else if (t->a > 0) {
                t->a = 0;
                t->d = 0;
            }
            t->d += 6;
            if (t->d > 0x30)
                t->d = 0x30;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0xf80);
            if (t->x == 0x200) {
                t->state = 4;
                t->d = -0x600;
                BossAttack_ActivateCollider(t);
                Nidhoggr_CreateLowerSweepBurst(&t->x);
            }
            break;
        case 4:
            t->state++;
            Sound_PlaySongChecked(0x403);
        case 5:
            t->a += t->d;
            if (t->a < -0x20000) {
                t->a = -0x20000;
                t->d = 0;
            }
            t->d += 6;
            if (t->d > -0x30)
                t->d = -0x30;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0xf80);
            if (t->x < 0) {
                t->state = 2;
                t->d = 0x30;
            }
            break;
    }
    t->timer++;
    allDead = 1;
    hp = t->hp;
    for (i = 8; i >= 0; hp++, i--) {
        if (*hp > 0)
            allDead = 0;
    }
    if (allDead)
        Nidhoggr_SetBehavior(t, 3);
}
