#define gShadowActorActionVectors         gUnk_0824F60C
#define gShadowActorMirroredActionVectors gUnk_0824F624
#include "global.h"

struct ShadowActionVector {
    u32 xy;
    u32 z;
};
struct ShadowActionRegion {
    u8 unknown0[6];
    s16 flags;
    u8 unknown8[0xC];
    struct ShadowActionVector vector;
    u8 unknown1C[0x44];
};
/* Same tentative actor layout as ShadowActor_UpdateTransition. */
struct ShadowActor {
    u8 unknown0[8];
    u8 animationData[0x20];
    u8 animation[8];
    u32 animationFlags;
    u8 unknown34[0xC0];
    struct ShadowActionRegion region;
    u8 entered;
    u8 phase;
    u8 animationEnded;
    u8 state;
};
extern const struct ShadowActionVector gShadowActorActionVectors[];
extern const struct ShadowActionVector gShadowActorMirroredActionVectors[];
void Anim_SetKeyframes(void *, void *, u32, u32, u32);

static inline void SetRegionFlag(struct ShadowActionRegion *region, u32 bit)
{
    region->flags = bit | (u16)region->flags;
}

static inline void ChangeState(struct ShadowActor *actor, u32 state)
{
    actor->state = state;
    actor->entered = 0;
    actor->phase = 0;
}

void ShadowActor_UpdateThreeStageAction(struct ShadowActor *actor)
{
    struct ShadowActionRegion *region;
    const struct ShadowActionVector *vector;
    u8 *counter;
    switch ((s32)actor->entered) {
        case 1:
        case 2:
            region = &actor->region;
            goto configure;
        case 0:
            Anim_SetKeyframes(actor->animation, actor->animationData, 1, 2, 0);
            region = &actor->region;
            {
                s32 mask = -5;
                region->flags &= mask;
            }
        configure:
            if (actor->animationFlags & 4)
                vector = &gShadowActorMirroredActionVectors[actor->entered];
            else
                vector = &gShadowActorActionVectors[actor->entered];
            region->vector = *vector;
            counter = &actor->entered;
            break;
        default: {
            struct ShadowActionRegion *region2 = &actor->region;
            SetRegionFlag(region2, 4);
            if (!actor->animationEnded)
                return;
            {
                u32 next = actor->phase;
                if (next > 4) {
                    next = 2;
                    ChangeState(actor, next);
                    return;
                }
                actor->entered = 0;
                counter = &actor->phase;
            }
        } break;
    }
    (*counter)++;
}
