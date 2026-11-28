#include "bosses/nidhoggr/nidhoggr_animation.h"
void AnimSprite_SelectGlyph(void *, void *, int);

void SceneAnimation_SetSlotFrameThirtyOne(struct Nidhoggr *boss, int i)
{
    if (!boss->rowInitFlag[i]) {
        boss->rows[i].flags &= ~1;
        AnimSprite_SelectGlyph(&boss->rows[i], &boss->animHdr, 31);
        boss->rowInitFlag[i]++;
        boss->rowDoneFlag[i] = 1;
    }
}
