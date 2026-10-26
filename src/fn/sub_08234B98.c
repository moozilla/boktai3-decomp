#include "global.h"

struct RestoreOwner {
    u8 unk_00[0x424];
    u16 life, maxLife;
};
struct RestoreParticle {
    u32 flags;
    u8 unk_04[0x14];
    u16 x, y, z;
    u8 unk_1E[10];
    u16 age, active, dx, dy, dz;
    u16 unk_32;
    void *data;
    void (*update)(struct RestoreParticle *);
};
struct HealthRestoreEffect {
    u8 unk_00[0x18];
    struct RestoreOwner *owner;
    u8 unk_1C[0x20];
    u16 x, y;
    u8 unk_40[0x3c];
    u16 targetX, targetY;
    u8 unk_80[0xc];
    struct RestoreParticle particles[8];
    u16 frame, nextParticle, timer;
    u16 unk_272;
    void (*state)(struct HealthRestoreEffect *);
};
extern u32 gRandomIndex;
extern u16 gUnk_0203B400[];
s32 Div(s32, s32);
s32 Mod(s32, s32);
void Sound_StopSong(u32);
void HealthRestoreEffect_InitRisingParticle(struct RestoreParticle *);
void HealthRestoreEffect_InitSparkle(struct RestoreParticle *);
void HealthRestoreEffect_UpdateCompletion(struct HealthRestoreEffect *);
static inline s32 rnd(u16 *table)
{
    s32 v;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    v = table[gRandomIndex];
    return v >> 3;
}

void HealthRestoreEffect_UpdateRestoration(struct HealthRestoreEffect *p)
{
    s32 done, step;
    u16 *table;
    if (p->timer == 0) {
        p->owner->life = 0;
        p->timer++;
    }
    done = 0;
    step = Div(p->owner->maxLife, 150);
    if (step == 0)
        step = 1;
    if ((p->owner->life += step) >= p->owner->maxLife) {
        p->owner->life = p->owner->maxLife;
        Sound_StopSong(0x259);
        done = 1;
    }
    table = gUnk_0203B400;
    p->x = p->targetX + Mod(rnd(table), 3) - 1;
    p->y = p->targetY + Mod(rnd(table), 3) - 1;
    if ((p->frame & 7) == 0) {
        HealthRestoreEffect_InitRisingParticle(&p->particles[p->nextParticle]);
        if (++p->nextParticle > 7)
            p->nextParticle = 0;
    }
    if ((p->frame & 15) == 0) {
        HealthRestoreEffect_InitSparkle(&p->particles[p->nextParticle]);
        if (++p->nextParticle > 7)
            p->nextParticle = 0;
    }
    p->frame++;
    if (done) {
        p->state = HealthRestoreEffect_UpdateCompletion;
        p->timer = 0;
    }
}
