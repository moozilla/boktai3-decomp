#include "global.h"
#include "display_sequence_sprite.h"
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
    int i;
    for (i = 0; i <= 3; i++) {
        if (m->slots[i].active) {
            m->slots[i].sprite.position = *m->slots[i].source;
            if (DisplaySequence_StepSprite(&m->slots[i].sprite, &m->slots[i].sequence)
                && m->slots[i].stopOnEnd) {
                m->slots[i].sprite.flags |= 1;
                m->slots[i].stopOnEnd = 0;
                m->slots[i].active = 0;
            }
        }
    }
    return 0;
}
