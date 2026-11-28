#include "bosses/nidhoggr/nidhoggr_animation.h"
#include "objects/scene_object_slot.h"
extern const struct Pair gUnk_0824F354;
void Nidhoggr_CreateSeekingAttackEffect(void *, void *, void *, void *);

void SceneAnimation_UpdateSlotSequenceSevenEffect(struct Nidhoggr *boss, int i)
{
    switch (boss->rowInitFlag[i]) {
        case 0:
            boss->rows[i].flags &= ~1;
            Anim_SetKeyframes(&boss->rows[i], &boss->animHdr, 7, 2, 4);
            boss->rowInitFlag[i]++;
            boss->rowDoneFlag[i] = 0;
            break;
        case 1:
            if (boss->rowCounter[i] > 23) {
                struct Pair v;

                boss->rowInitFlag[i] = 2;
                v = gUnk_0824F354;
                Nidhoggr_CreateSeekingAttackEffect(&boss->effectPool, &boss->rows[i].position, &v,
                                                   boss);
            }
            break;
    }
    if (Anim_Step(&boss->rows[i], &boss->animHdr)) {
        boss->rowInitFlag[i] = 0xff;
        boss->rowDoneFlag[i] = 1;
    }
}
