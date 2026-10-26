// confidence=low: existing Golem state interpretation retained.
// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: neighboring Golem controls establish the state role.
// confidence=low: Golem_BeginSlideState installs control kind 0x29 with speed 0x200.
#ifdef NONMATCHING
// fuzzy: 90.5%
// fix.py best (sweep after the inlined-insn fix)
// Push along the stored heading, emit dust, and resolve an impact into completion or defeat.
// Remaining mismatch: motion-pointer copies, RNG allocation and completion/control-reset offsets.
#include "scene_object.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"

struct SceneActorPlacementView;
void SceneActorPlacement_PlayVariantEventSound(struct SceneActorPlacementView *, u32);
u32 ActorMode_SelectOddStateVariant(struct SceneActorRegistryResetView *);
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);

struct GolemSlideBody {
    u8 unknown0[9];
    u8 direction;
    u8 unknownA[2];
    struct ContactPosition position;
    s16 motionX, motionY, motionZ;
};

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}
static inline u32 NextRandom(void)
{
    register const u16 *table asm("r6") = (const u16 *)RANDOM_TABLE_ADDR;
    asm("" : "+r"(table));
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
static inline s32 Quarter(s16 n)
{
    if (n >= 0)
        return n >> 2;
    return -((-n) >> 2);
}

void Golem_UpdateSlideImpact(struct SceneActorRegistryResetView *actor, u32 frame)
{
    u8 *context = actor->context;
    u8 *group = (u8 *)(&actor->group);
    struct GolemSlideBody *body = (struct GolemSlideBody *)(((u8 *)actor) + 0x50);
    if (TakeFlag(&actor->field2B2))
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 4);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0xF);
    {
        u32 mask = 0x20;
        actor->flags2D6 |= mask;
    }
    {
        u32 mask = 0x100;
        actor->flags2D4 |= mask;
    }
    {
        u32 mask = 0x2000;
        actor->flags2CC |= mask;
    }
    {
        s16 *motionX = &actor->motion.x;
        s16 *speed = (s16 *)(context + 0x682);
        *motionX += ((*speed) * gUnk_086149C4[(body->direction + 0x40) & 0xFF]) >> 16;
        actor->motion.z += ((*speed) * gUnk_086149C4[body->direction]) >> 16;
    }
    if ((frame & 3) == 0) {
        struct SceneExtent spread;
        struct SceneExtent size;
        spread.x = -body->motionX;
        spread.y = 0;
        spread.z = -body->motionZ;
        size.x = 0x20;
        size.y = 0;
        size.z = 0x20;
        SceneParticleBurst_CreateWithPause((void *)(&body->position), (s32)(&spread), &size, 1, 3,
                                           4);
    }
    if ((*(((u8 *)actor) + 0x40)) & 1) {
        struct SceneExtent velocity;
        struct SceneExtent extent;
        u32 count;
        *((u16 *)(context + 0x6AC)) = 0x1E;
        *((u16 *)(context + 0x6AE)) = 0x14;
        velocity.x = Quarter(body->motionZ);
        {
            register u32 unused asm("r4") = 0x0403B714;
            asm("" : : "r"(unused));
            velocity.y = NextRandom() & 7;
        }
        velocity.z = Quarter(body->motionX);
        {
            extent.x = (NextRandom() & 3) + 15;
            extent.y = (NextRandom() & 7) + 15;
            extent.z = (NextRandom() & 3) + 15;
        }
        count = (NextRandom() & 3) + 4;
        sub_08014634(count, ActorMode_SelectOddStateVariant(actor), (void *)(&body->position),
                     &velocity, &extent, 45, 31);
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 5);
        if (*((u16 *)(context + 0x634))) {
            actor->field2B0 = 1;
        } else {
            {
                u16 delay = 0xB4;
                actor->fieldAA = delay;
                {
                    u32 mask = 1;
                    actor->flags2D0 |= mask;
                }
            }
            actor->state2A8 = 3;
            actor->phase2A9 = 0;
            actor->behavior2AA = 0;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
            {
                void *target = *((void **)(group + 0x208));
                actor->field2B2 = 1;
                actor->field2B0 = 0;
                actor->field2B5 = 7;
                actor->target = target;
            }
            actor->field2C8 = 0;
            actor->flags114 &= ~1;
            actor->word104 = 0;
            {
                u16 k1 = 0x100;
                actor->colliderA.flags &= ~k1;
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Golem_UpdateSlideImpact);
#endif
