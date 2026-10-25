#include "global.h"
#include "scene_actor_reset.h"

s32 Div(s32, s32);
s32 ActorTrail_RemoveLastSegment(struct SceneActorRegistryResetView *);
void SceneActorControl_OnIdle(void);
static inline void h10a_SetControl(struct SceneActorRegistryResetView *actor, void *control,
                                   u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}

#ifdef NONMATCHING
// fuzzy: 51.1%
// scope shape 123 (differences once low registers are set aside)
// confidence=low: per-frame chase of a tentacle body toward its target position, then idle
void sub_080A71A0(struct SceneActorRegistryResetView *a)
{
    u8 *trail = a->context;
    u32 flag = 0;
    s32 n;
    s32 vx, vz;
    s32 dx, dz;
    u32 *p2CC = &a->flags2CC;
    u32 *p2D0 = &a->flags2D0;
    u16 *p310 = (u16 *)((u8 *)a + 0x310);
    u16 *p318 = (u16 *)((u8 *)a + 0x318);
    u16 *p314 = (u16 *)((u8 *)a + 0x314);
    u16 *p31C = (u16 *)((u8 *)a + 0x31C);
    u16 *p64 = (u16 *)((u8 *)a + 0x64);
    u16 *p68 = (u16 *)((u8 *)a + 0x68);

    if (a->field2B2) {
        a->field2B2 = 0;
        flag = 1;
    }
    if (flag) {
        a->flags2D4 = (a->flags2D4 | 0x20) & ~1;
        n = Div((s16)a->health, trail[0x8EB]);
        if (n < trail[0x8EA]) {
            do {
                ActorTrail_RemoveLastSegment(a);
                n++;
            } while (n < trail[0x8EA]);
        }
    }
    *p2CC |= 0x2000;
    *p2D0 |= 0x10000;

    dx = (s16)*p310 - (s16)*p318;
    if (dx < 0) {
        vx = -((-dx) >> 2);
    } else {
        vx = dx >> 2;
    }
    if (vx > 0xC8) {
        vx = 0xC8;
    } else if (vx < -0xC8) {
        vx = -0xC8;
    }
    dz = (s16)*p314 - (s16)*p31C;
    if (dz < 0) {
        vz = -((-dz) >> 2);
    } else {
        vz = dz >> 2;
    }
    if (vz > 0xC8) {
        vz = 0xC8;
    } else if (vz < -0xC8) {
        vz = -0xC8;
    }
    *p64 += vx;
    *p68 += vz;
    *p318 += vx;
    *p31C += vz;

    if (vx == 0 && vz == 0) {
        u32 one = 1;
        u32 five = 5;
        a->field2B0 = one;
        a->flags2D4 &= ~0x20;
        h10a_SetControl(a, SceneActorControl_OnIdle, 0x10);
        a->state2A8 = 0;
        a->phase2A9 = 0;
        a->behavior2AA = five;
        a->counter2AC = 0;
        a->field2B1 = one;
        a->direction = a->fieldE9;
        a->field2C2 = 0x40;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080A71A0);
#endif
