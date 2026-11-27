#include "engine/video/display_sequence_sprite.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_UpdatePartCollision(struct VanargandPairAttack *, struct VanargandDamagePart *);
void SceneAnimation_SelectSequence(void *, s32, s32, s32);

void Vanargand_UpdatePartAnimation(struct VanargandPairAttack *boss,
                                 struct VanargandDamagePart *part)
{
    if (!(part->sprite.flags & 1)) {
        part->flags = 0;
        Vanargand_ApplyPartDamage(part);
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
