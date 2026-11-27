#include "global.h"
#include "engine/video/display_animation.h"
#include "engine/video/display_sequence_sprite.h"
#include "engine/video/display_sprite_playback.h"
struct PlayerSlotDisplaySlot {
    u8 resource[0x1C];
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState sequence;
    const struct ContactPosition *source;
    u16 stopOnEnd, active;
};
struct PlayerSlotDisplay {
    u8 task[0x18];
    struct PlayerSlotDisplaySlot slots[4];
    void *sequenceResource;
};
int PositionEffectDisplay_UpdateSources(struct PlayerSlotDisplay *m)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        if (m->slots[i].active) {
            struct ContactPosition *position = &m->slots[i].sprite.position;
            *position = *m->slots[i].source;
            if (PlayerCue_StepSequenceEnded((void *)&m->slots[i].sprite, &m->slots[i].sequence) &&
                m->slots[i].stopOnEnd) {
                m->slots[i].sprite.flags |= 1;
                m->slots[i].stopOnEnd = 0;
                m->slots[i].active = 0;
            }
        }
    }
    return 0;
}
