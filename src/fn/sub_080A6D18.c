#include "scene_actor_control_phase.h"

static inline void TentacleBody_SelectAnimation(struct SceneActorAnimationView *actor,
                                                u32 selection, u32 variant, u32 flags)
{
    u32 force;
    u8 *restart;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->selection = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        savedSelection = &actor->selection;
        restart = &actor->restart;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            flags &= 1;
            if (flags)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}

struct TentacleAttackBody {
    u8 unknown0[0x8D0];
    u16 attackFlags;
};

void TentacleBody_GetDirectionFrame(u8, u32 *, u32 *);
void TentacleBody_RefreshDirectionalAnimations(void *, s32);
void TentacleBody_UpdateAttackRecovery(void);
void SceneActor_AimArcAtPlayer(void *);
void ActorTrail_ApplyAngularOffsets(void *, s32, s32, s32);
static inline u8 TentacleBody_TakeEntryFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

void TentacleBody_BeginDirectedAttack(struct SceneActorRegistryResetView *actor)
{
    u8 *entry = (void *)SceneActorProfiles_GetEntryAddress(
        ((struct SceneActorAnimationView *)actor)->profileId,
        ((struct SceneActorAnimationView *)actor)->profileMode, 0);
    struct TentacleAttackBody *body = actor->context;
    u32 variant;
    u32 flags;
    u32 entered = TentacleBody_TakeEntryFlag(&actor->field2B2);
    u8 *direction = &actor->direction;
    u32 *attackFlags = &actor->flags2D0;
    if (entered) {
        SceneActor_AimArcAtPlayer(actor);
        *direction = Math_Atan2(actor->secondaryMotion.x, actor->secondaryMotion.z);
        {
            u32 mask = 256;
            actor->flags2D4 |= mask;
        }
        {
            u32 mask = ~0x100000;
            *attackFlags &= mask;
        }
        {
            u16 *bodyFlags = &body->attackFlags;
            s32 mask = ~2;
            u32 value = *bodyFlags;
            mask &= value;
            mask |= 4;
            body->attackFlags = mask;
        }
        Sound_PlaySong(982);
    }
    TentacleBody_GetDirectionFrame(*direction, &variant, &flags);
    TentacleBody_SelectAnimation((void *)actor, entry[0x38], variant, flags);
    ActorTrail_ApplyAngularOffsets(actor, 18, 6, 0);
    TentacleBody_RefreshDirectionalAnimations(actor->context, 15);
    {
        u32 mask = 16;
        if ((*attackFlags) & mask) {
            {
                u16 *field = &actor->flags2D4;
                s32 mask = ~256;
                actor->flags2D4 = *field & mask;
            }
            {
                void *callback = (void *)TentacleBody_UpdateAttackRecovery;
                u32 kind = 37;
                actor->field2B2 = 1;
                actor->field2B0 = 0;
                actor->field2B5 = kind;
                actor->target = callback;
                actor->field2C8 = 0;
                {
                    u32 *flags = &actor->flags114;
                    u32 mask = ~1;
                    *flags &= mask;
                }
                actor->word104 = 0;
            }
        } else {
            u32 mask = 8192;
            actor->flags2CC |= mask;
        }
    }
}
