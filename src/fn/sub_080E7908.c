// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"
#include "player.h"
#include "engine.h"
u32 SceneActor_GetDirectionFlags(u32);
u8 SceneActorTarget_TrySelectNearestParticipant(struct SceneActorRegistryResetView *);
void SceneActor_ResetCollisionReaction(struct SceneActorRegistryResetView *);
void SceneActorAnimation_SelectEntryFlags(struct SceneActorAnimationView *, s32, u32);
static inline s32 Quarter(s32 v)
{
    return v >= 0 ? v >> 2 : -(-v >> 2);
}
static inline s32 Clamp(s32 v)
{
    if (v > 200)
        return 200;
    if (v < -200)
        return -200;
    return v;
}
static inline u8 AtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

void Mimic_ConvergeAndTurn(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    s32 x;
    s32 z;
    s16 *priorX;
    s16 *priorZ;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectEntryFlags((void *)actor, 3,
                                             SceneActor_GetDirectionFlags(actor->body.direction));
        {
            register u32 mask asm("r0") = 0x10;
            u16 *flags = &actor->flags2D6;
            *flags = mask | *flags;
        }
    }
    {
        u32 offset = 0x310;
        s32 current = *(s16 *)((u8 *)actor + offset);
        u32 priorOffset;
        s16 *field;
        s32 difference;
        priorOffset = 0x318;
        field = (void *)((u8 *)actor + priorOffset);
        difference = current - *field;
        priorX = field;
        x = Quarter(difference);
    }
    {
        register u32 offset asm("r4") = 0x314;
        s32 current = *(s16 *)((u8 *)actor + offset);
        register s16 *field asm("r1");
        s32 difference;
        offset += 8;
        asm("" : "+r"(offset));
        field = (void *)((u8 *)actor + offset);
        difference = current - *field;
        priorZ = field;
        z = Quarter(difference);
    }
    x = Clamp(x);
    z = Clamp(z);
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (AtRest(x, z) && actor->field2B3) {
        u16 *flags2D4;
        register u32 *p asm("r6");
        u16 *f = &actor->flags2D4;
        u8 t;
        {
            u32 mask;
            u32 value;
            mask = ~0x20;
            value = *f;
            mask &= value;
            *f = mask;
        }
        {
            u32 gv = gEngineFlags;
            u32 bits = 0x4800;
            u32 hit = gv & bits;
            flags2D4 = f;
            t = Test(hit, hit);
        }
        if (t) {
            if (!SceneActorTarget_TrySelectNearestParticipant(actor)) {
                SceneActor_ResetCollisionReaction(actor);
                return;
            }
        } else {
            actor->tracking.target = (u32)gPlayers[0] + 0x24;
        }
        p = &actor->tracking.target;
        {
            u32 q = *p;
            if (q) {
                struct ContactPosition *target;
                struct ContactPosition d;
                s16 dx;
                u16 *fp;
                u32 one;
                u32 z4;
                register u32 z6 asm("r9");
                struct ContactPosition *out = &d;
                struct ContactPosition *from = &actor->body.position;
                target = (void *)(q + 0xC);
                dx = target->x - from->x;
                z4 = 0;
                asm("" : "+r"(z4));
                z6 = 0;
                out->x = dx;
                out->y = target->y - from->y;
                out->z = target->z - from->z;
                actor->body.direction = actor->tracking.heading = Math_Atan2(d.x, d.z);
                {
                    u16 *fl = &actor->group.common.gate.flags;
                    register u32 mask asm("r0") = 4;
                    *fl = mask | *fl;
                }
                {
                    register u32 mask asm("r0") = 0x80;
                    fp = flags2D4;
                    *fp = mask | *fp;
                }
                actor->group.common.gate.counter = actor->group.common.gate.duration;
                {
                    u16 *mode = (u16 *)((u8 *)actor->context + 0x65C);
                    u32 mask;
                    one = 1;
                    mask = ~one;
                    asm("" : "+r"(mask));
                    *mode = mask & *mode;
                }
                {
                    register u32 mask asm("r0") = 8;
                    ((u8 *)p)[15] = mask | ((u8 *)p)[15];
                }
                {
                    u32 two = 2;
                    register u32 o asm("r2") = 0x2A8;
                    register u32 o2 asm("r4");
                    *((u8 *)actor + o) = z4;
                    o2 = 0x2A9;
                    *((u8 *)actor + o2) = z4;
                    o += 2;
                    *((u8 *)actor + o) = two;
                    o2 += 3;
                    {
                        register u32 *w asm("r0") = (u32 *)((u8 *)actor + o2);
                        *w = z6;
                    }
                    o += 7;
                    {
                        register u8 *b asm("r0") = (u8 *)actor + o;
                        *b = one;
                    }
                }
            } else {
                u32 zero = q;
                register u32 o asm("r3") = 0x2A8;
                register u32 o2 asm("r4");
                u32 o3 = 0x2AA;
                *((u8 *)actor + o) = zero;
                o2 = 0x2A9;
                *((u8 *)actor + o2) = zero;
                *((u8 *)actor + o3) = zero;
                o += 4;
                asm("" : "+r"(o));
                {
                    register u32 *w asm("r0") = (u32 *)((u8 *)actor + o);
                    *w = zero;
                }
                o2 += 8;
                {
                    register u8 *b asm("r1") = (u8 *)actor + o2;
                    *b = 1;
                }
            }
        }
    }
    {
        u32 mask = 0x2000;
        register u32 off asm("r0") = 0x2CC;
        register u32 *flags asm("r1");
        asm("" : "+r"(off));
        flags = (u32 *)((u8 *)actor + off);
        *flags |= mask;
    }
}
