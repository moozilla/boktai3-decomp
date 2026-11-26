#include "global.h"
#include "player/cues/player_cue_sequence.h"
#include "scene/actor/actor_attachment_state.h"
#include "effects/screen/background_controller.h"
#include "effects/particles/native_effect.h"
struct FixedPosition {
    s32 x, y, z;
};
struct MarkerSlot {
    u8 resource[0x1c];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    const struct FixedPosition *anchor;
    u32 active;
};
struct MarkerPool {
    u8 task[0x18];
    struct MarkerSlot slots[8];
};
s32 ActorMarkerPool_UpdateMarkers(struct MarkerPool *pool)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (pool->slots[i].active) {
            pool->slots[i].sprite.position.x = (pool->slots[i].anchor->x >> 12) - gSceneCamera->x;
            pool->slots[i].sprite.position.y =
                ((pool->slots[i].anchor->z + pool->slots[i].anchor->y) >> 12) + 0x5c;
            PlayerCue_StepSequence(&pool->slots[i].sprite, &pool->slots[i].sequence);
        }
    }
    return 0;
}
