#include "global.h"
#include "scene/terrain/terrain_sampling.h"
#include "engine/math/math_shift.h"
#include "npc/manager/scene_actor_lifecycle.h"

extern const s16 gUnk_086149C4[];

struct RiseTag {
    u8 unknown0[6];
    u16 field6;
};

// confidence=low: state actor rising to terrain height plus 0x80, with a sine bob on entry.
void StoryCharacter_UpdateRiseToTerrain(void *context, struct SceneActor *actor, s32 tick)
{
    struct RiseTag *tag = (struct RiseTag *)actor->unknown20;
    struct ContactPosition *pos = &actor->body.position;
    s32 height = Terrain_SampleHeight(pos);
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        pos->y = height + 0x80 +
                 Math_ShiftTowardZero(gUnk_086149C4[(actor->stateFrame << 2) & 0xFF] << 4, 12);
        ((struct ContactPosition *)actor->drawable.sequence.position)->y =
            height + 0x80 +
            Math_ShiftTowardZero(gUnk_086149C4[(actor->stateFrame << 2) & 0xFF] << 4, 12);
        actor->animationPhase = 0;
        tag->field6 = 0;
    }
    switch (actor->animationPhase) {
        case 0:
            SceneActor_SetFlags(actor, 0x400);
            if (pos->y >= height) {
                pos->y--;
            } else {
                pos->y = height;
                actor->animationPhase++;
                actor->stateFrame = 0;
            }
            break;
        case 1:
            if (tick > 0x7f)
                actor->animationPhase = 2;
            break;
        case 2:
            actor->finished = 1;
            break;
    }
}
