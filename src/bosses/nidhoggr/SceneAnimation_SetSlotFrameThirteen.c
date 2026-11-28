#include "bosses/nidhoggr/nidhoggr_animation.h"
void AnimSprite_SelectGlyph(void *, void *, int);

void SceneAnimation_SetSlotFrameThirteen(struct Nidhoggr *boss, int i)
{
    if (!boss->rowInitFlag[i]) {
        boss->rows[i].flags &= ~1;
        AnimSprite_SelectGlyph(&boss->rows[i], &boss->animHdr, 13);
        boss->rowInitFlag[i]++;
        boss->rowDoneFlag[i] = 1;
    }
}
