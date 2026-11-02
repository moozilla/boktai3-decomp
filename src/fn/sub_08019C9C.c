#include "global.h"
#include "ram.h"
#include "display_sprite_playback.h"
struct SpritePoolSlot {
    u32 id;
    u8 pad4[8];
    struct ContactSwarmSprite sprite;
    u8 pad38[0x1c];
    struct DisplaySequenceState anim;
};
int SpritePool_AdvanceAnimation(struct SpritePoolSlot *slot)
{
    if (!gSpritePoolManager)
        return 0;
    return PlayerCue_StepSequenceEnded(&slot->sprite, &slot->anim);
}
