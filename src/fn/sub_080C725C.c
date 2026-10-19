#include "global.h"
#ifdef NONMATCHING
// fuzzy: 67.2%
// Slide-and-shed state: pushes along the context heading, bursts dust every 4 frames and, when
// the actor's +0x40 bit 0 is set, sprays particles and then either finishes or hands control to
// group+0x208 (kind 7). About 30 differing instructions ignoring registers: the ROM reloads the
// random table for the extent draws (mine reuses the velocity draw's), and allocation.
#include "scene_object.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"

struct SceneActorPlacementView;
void SceneActorPlacement_PlayVariantEventSound(struct SceneActorPlacementView *, u32);
u32 ActorMode_SelectOddStateVariant(struct SceneActorRegistryResetView *);
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);

struct Unk080C725CBody {
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
static inline u16 NextRandom(void)
{
    const u16 *table = gUnk_0203B400;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return table[gUnk_03005308];
}
static inline s32 Quarter(s16 n)
{
    if (n >= 0)
        return n >> 2;
    return -((-n) >> 2);
}

void sub_080C725C(struct SceneActorRegistryResetView *actor, u32 frame)
{
    u8 *context = actor->context;
    u8 *group = (u8 *)&actor->group;
    struct Unk080C725CBody *body = (struct Unk080C725CBody *)((u8 *)actor + 0x50);

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
        *motionX += (*speed * gUnk_086149C4[(body->direction + 0x40) & 0xFF]) >> 16;
        actor->motion.z += (*speed * gUnk_086149C4[body->direction]) >> 16;
    }
    if ((frame & 3) == 0) {
        struct SceneExtent spread, size;
        spread.x = -body->motionX;
        spread.y = 0;
        spread.z = -body->motionZ;
        size.x = 0x20;
        size.y = 0;
        size.z = 0x20;
        SceneParticleBurst_CreateWithPause((void *)&body->position, (s32)&spread, &size, 1, 3, 4);
    }
    if (*((u8 *)actor + 0x40) & 1) {
        struct SceneExtent velocity, extent;
        u32 count;
        *(u16 *)(context + 0x6AC) = 0x1E;
        *(u16 *)(context + 0x6AE) = 0x14;
        velocity.x = Quarter(body->motionZ);
        velocity.y = NextRandom() & 7;
        velocity.z = Quarter(body->motionX);
        {
            const u16 *table = gUnk_0203B400;
            u32 i = gUnk_03005308;
            i = (i + 1) & 1023;
            extent.x = (table[i] & 3) + 15;
            i = (i + 1) & 1023;
            extent.y = (table[i] & 7) + 15;
            i = (i + 1) & 1023;
            gUnk_03005308 = i;
            extent.z = (table[i] & 3) + 15;
        }
        count = (NextRandom() & 3) + 4;
        sub_08014634(count, ActorMode_SelectOddStateVariant(actor), (void *)&body->position,
                     &velocity, &extent, 45, 31);
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 5);
        if (*(u16 *)(context + 0x634)) {
            actor->field2B0 = 1;
        } else {
            {
                u16 delay = 0xB4;
                u32 mask = 1;
                actor->fieldAA = delay;
                actor->flags2D0 |= mask;
            }
            actor->state2A8 = 3;
            actor->phase2A9 = 0;
            actor->behavior2AA = 0;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
            {
                void *target = *(void **)(group + 0x208);
                actor->field2B2 = 1;
                actor->field2B0 = 0;
                actor->field2B5 = 7;
                actor->target = target;
            }
            actor->field2C8 = 0;
            actor->flags114 &= ~1;
            actor->word104 = 0;
            *(u16 *)((u8 *)&actor->colliderA + 6) &= ~0x100;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C725C);
#endif
