#include "global.h"

#include "scene_actor_reset.h"
#include "scene_actor_animation.h"

// confidence=low: steps an actor's directional animation toward its requested facing and restarts the matching sequence.

// confidence=low: steps an actor's directional animation toward its requested facing and restarts the matching sequence.

// confidence=low: steps an actor's directional animation toward its requested facing and restarts the matching sequence.

// confidence=low: steps an actor's directional animation toward its requested facing and restarts the matching sequence.

// confidence=low: steps an actor's directional animation toward its requested facing and restarts the matching sequence.

// confidence=low: steps an actor's directional animation toward its requested facing and restarts the matching sequence.
u32 Route_GetCurrentPointFacing(void *);

static inline u8 BAE04_Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline void BAE04_Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = 0;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = 0;
}

static inline void BAE04_Select(struct SceneActorAnimationView *actor, u32 selection, u32 variant,
                                u32 flags)
{
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable = actor->drawable;
        actor->selection = selection;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        actor->restart = 0;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        selection += variant;
        if (!(actor->selection == selection && !actor->restart &&
              ((flags == 1 && (tile->flags & 4)) || (flags == 0 && !(tile->flags & 4))))) {
            actor->selection = selection;
            Anim_SetKeyframes(tile, actor->drawable, (u16)selection, 2, (flags & 4) ? 6 : 4);
            if (flags & 1)
                tile->flags |= 4;
            else
                tile->flags &= ~4;
            actor->restart = 0;
        }
    }
}

static inline void BAE04_Begin(struct SceneActorRegistryResetView *a)
{
    a->state2A8 = 3;
    a->phase2A9 = 1;
    a->behavior2AA = 0;
    a->counter2AC = 0;
    a->field2B1 = 1;
    BAE04_Control(a, *(void **)((u8 *)a + 0x5F0), 0x1B);
}

#ifdef NONMATCHING
// fuzzy: 25.6%
// scope shape 617 (differences once low registers are set aside)
// confidence=low: steps an actor's directional animation toward its requested facing and restarts the matching sequence.
void sub_080BAE04(struct SceneActorRegistryResetView *a)
{
    struct SceneActorAnimationView *anim = (struct SceneActorAnimationView *)a;
    u32 quadrant;
    u8 nd;
    u8 entry;
    u16 *sl;
    u8 *dir;

    sl = &a->flags2D4;
    dir = &a->direction;
    if (BAE04_Take(&a->field2B2)) {
        *dir = (u8)(((((*dir + 0x60) >> 6) & 3) << 6) - 0x40);
        quadrant = ((*dir + 0x60) >> 6) & 3;
        if (a->behavior2AA != 6 && a->behavior2AA != 0x14 && !(*sl & 0x4480)) {
            s32 facing = (u16)Route_GetCurrentPointFacing((u8 *)a + 0x448);
            nd = ((facing + 0x70) >> 5) & 7;
            anim->nextDirection = nd;
        } else {
            nd = anim->nextDirection;
        }
        if (quadrant == (nd >> 1)) {
            a->field2B0 = 1;
            if (*sl & 0x400)
                BAE04_Begin(a);
            return;
        }
        entry = *(u8 *)SceneActorProfiles_GetEntryAddress(a->profileId, a->profileMode, 2);
        switch (quadrant) {
            case 0:
                if ((u32)(nd - 2) <= 3) {
                    BAE04_Select(anim, entry, 1, 0);
                    a->flags2CC |= 0x10000;
                } else {
                    BAE04_Select(anim, entry, 0, 4);
                    a->flags2CC &= ~0x10000;
                }
                break;
            case 1:
                if ((u32)(nd - 4) <= 3) {
                    BAE04_Select(anim, entry, 2, 0);
                    a->flags2CC |= 0x10000;
                } else {
                    BAE04_Select(anim, entry, 1, 4);
                    a->flags2CC &= ~0x10000;
                }
                break;
            case 2:
                if ((u32)(nd - 1) <= 4) {
                    BAE04_Select(anim, entry, 2, 4);
                    a->flags2CC &= ~0x10000;
                } else {
                    BAE04_Select(anim, entry, 1, 5);
                    a->flags2CC |= 0x10000;
                }
                break;
            case 3:
                if (nd <= 3) {
                    BAE04_Select(anim, entry, 0, 0);
                    a->flags2CC |= 0x10000;
                } else {
                    BAE04_Select(anim, entry, 1, 1);
                    a->flags2CC &= ~0x10000;
                }
                break;
            default:
                break;
        }
    }

    if (*(u16 *)((u8 *)a->drawable + 0x2C) == 0 &&
        (u16)(*(u16 *)((u8 *)a->drawable + 0x34) - 1) <= 1) {
        u32 want = (((anim->nextDirection + 5) & 7) << 5);
        s32 diff = (s32)*dir - (s32)want;
        if (diff < 0)
            diff = -diff;
        if (diff <= 0x20)
            *dir = (u8)want;
        else if (a->flags2CC & 0x10000)
            *dir += 0x20;
        else
            *dir -= 0x20;
    }

    if ((*sl & 0xA02F) || !a->field2C2)
        a->field2C2 = 0x40;
    if (a->drawableKind == 0)
        Anim_SetDurationScale((u8 *)a->drawable + 0x48, a->field2C2);
    else
        Anim_SetFrameDurationScale((u8 *)a->drawable + 0x20, a->field2C2);

    if (!a->field2B3)
        return;
    a->field2B0 = 1;
    {
        s32 cur = (((*dir + 0x70) >> 5) | -8);
        s32 k = ((s32)anim->nextDirection - cur) & 7;
        if (k > 4)
            k = 8 - k;
        if (k != 0) {
            BAE04_Control(a, a->entryTarget224, 2);
            return;
        }
    }
    *dir = (u8)((anim->nextDirection + 5) << 5);
    SceneActorAnimation_SelectDirectionalEntry(anim, 0);
    if (*sl & 0x400)
        BAE04_Begin(a);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080BAE04);
#endif
