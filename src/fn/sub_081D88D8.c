#define SpriteSequence_SetCallback  Projectile_SetUpdateCallback
#define SpriteGroup_SelectAnimation Task4E01_SetTrailSpriteResources
#include "global.h"
struct SpriteSequenceState {
    u8 unknown[0x4C];
    u32 stage;
};
struct SpriteSequenceSprite {
    u32 flags;
    u8 unknown4[0xC];
    u16 frame;
};
struct SpriteSequence {
    u8 unknown0[0x18];
    struct SpriteSequenceState state;
    u8 unknown68[0x8C - 0x68];
    struct SpriteSequenceSprite sprite;
    u8 unknownA0[0x1A4 - 0xA0];
    s16 timer;
};
void *ResourceRegistry_GetResource(u16);
void Projectile_SetUpdateCallback(void *, void *);
void Task4E01_SetTrailSpriteResources(void *, u32);
void Task4E01_UpdateTrailImpact(void);
extern const u32 gUnk_08613684[];
s32 SpriteSequence_BeginStageAnimation(struct SpriteSequence *p)
{
    struct SpriteSequenceState *state = &p->state;
    void *group = ResourceRegistry_GetResource(0x4E01);
    s32 result;
    if (group) {
        p->timer = 1;
        {
            struct SpriteSequenceSprite *sprite = &p->sprite;
            sprite->frame = 0x1A;
        }
        Projectile_SetUpdateCallback(state, Task4E01_UpdateTrailImpact);
        Task4E01_SetTrailSpriteResources(group, gUnk_08613684[state->stage]);
        result = 0;
    } else {
        result = -1;
    }
    return result;
}
