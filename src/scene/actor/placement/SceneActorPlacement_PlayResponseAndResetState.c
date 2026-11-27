#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene/actor/scene_actor_placement.h"

static inline void SetFlags(struct RoomActor *s, u16 mask)
{
    s->motionFlags = mask | s->motionFlags;
}
static inline u32 MaskFlags(u16 *p, u32 mask)
{
    mask &= *p;
    *p = mask;
    return mask;
}
static inline u16 TestFlags(u32 value, u32 mask)
{
    return mask & value;
}
s32 SceneActorPlacement_PlayResponseAndResetState(struct RoomActor *s)
{
    u32 flags = MaskFlags(&s->motionFlags, 0xFFFFFEFF);
    switch (s->category) {
        case 0x12:
            if (!s->reactionSoundVariant)
                goto fallback;
            Sound_PlaySong(0x108);
            break;
        case 0x13:
            if (!s->reactionSoundVariant)
                goto fallback;
            Sound_PlaySong(0x109);
            break;
        case 0x14:
            if (!s->reactionSoundVariant)
                goto fallback;
            Sound_PlaySong(0x13E);
            break;
        case 0x17:
            if (!s->reactionSoundVariant) {
            fallback:
                Sound_PlaySong(0x211);
            } else if ((u8)TestFlags(flags, 8)) {
                Sound_PlaySong(0x13E);
                Sound_PlaySong(0x12A);
            } else {
                Sound_PlaySong(0x13E);
            }
            break;
    }
    {
        SceneActorTerrain_InstallControl((struct ChaseActor *)s, s->onReaction, 0x12);
        SceneActorTerrain_EnterState((struct ChaseActor *)s, 3, 0, 0);
        SceneActorPlacement_ClearFlags(&s->reactionFlags, 0xFFFFF636);
        SceneActorChase_AndWordFlags(&s->statusFlags, 0xFEFFFFFF);
    }
    return 1;
}
