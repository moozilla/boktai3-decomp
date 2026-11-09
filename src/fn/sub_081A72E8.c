#include "global.h"
#include "display_sequence_sprite.h"
struct VanargandAnimatedPart {
    struct DisplaySequenceSprite sprite;
    u8 graphics[0x1C];
    struct DisplaySequenceState sequence;
    u8 unknown58[4];
    u32 sequenceId;
    u8 unknown60[0xA0];
    u32 flags;
    u16 animation;
};
void Vanargand_UpdatePartCollision(void *boss, void *part);
void sub_081A7204(void *part);
void SceneAnimation_SelectSequence(void *, s32, s32, s32);
void Vanargand_UpdatePartAnimation(void *boss, struct VanargandAnimatedPart *part)
{
    if (!(part->sprite.flags & 1)) {
        part->flags = 0;
        sub_081A7204(part);
        if (part->animation) {
            if (DisplaySequence_StepSprite(&part->sprite, &part->sequence)) {
                part->animation = 0;
                if (part->sequenceId == 2)
                    SceneAnimation_SelectSequence(part, 3, 0, 0);
            }
            part->flags |= 1;
        }
        Vanargand_UpdatePartCollision(boss, part);
    }
}
