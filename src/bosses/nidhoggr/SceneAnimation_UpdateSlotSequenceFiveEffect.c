#include "bosses/nidhoggr/nidhoggr_animation.h"
#include "objects/scene_object_slot.h"
extern const struct Pair gUnk_0824F34C;
void Nidhoggr_CreateAttachedAttackEffect(void *, void *, void *, void *);

void SceneAnimation_UpdateSlotSequenceFiveEffect(struct Nidhoggr *boss, int i)
{
    struct Pair v;

    if (!boss->rowInitFlag[i]) {
        boss->rows[i].flags &= ~1;
        Anim_SetKeyframes(&boss->rows[i], &boss->animHdr, 5, 2, 4);
        boss->rowInitFlag[i]++;
        boss->rowDoneFlag[i] = 0;
        v = gUnk_0824F34C;
        Nidhoggr_CreateAttachedAttackEffect(&boss->effectPool, &boss->rows[i].position, &v, boss);
    }
    if (Anim_Step(&boss->rows[i], &boss->animHdr)) {
        boss->rowInitFlag[i] = 0xff;
        boss->rowDoneFlag[i] = 1;
    }
}
