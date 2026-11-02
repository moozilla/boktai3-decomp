#include "display_animation.h"
#include "display_sprite_playback.h"
s32 DisplayAnimation_UpdatePlayback(struct DisplayAnimation *display)
{
    u32 kind = display->kind;
    if (kind == 0)
        return -1;
    if (kind == 1) {
        if (!display->resource.sequence)
            goto done;
        display->finished = PlayerCue_StepSequenceEndedWithFirstFlipMask(
            (struct ContactSwarmSprite *)((u8 *)display + 0x28),
            (struct DisplaySequenceState *)((u8 *)display + 0x54), kind);
    } else {
        void *resource = &display->resource;
        display->finished = Anim_Step((u8 *)display + 0x2C, resource);
    }
done:
    return display->finished;
}
