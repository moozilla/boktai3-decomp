#include "bosses/nidhoggr/nidhoggr_animation.h"

void SceneAnimation_UpdateSlotSequenceFourHide(struct Nidhoggr *boss, int i)
{
    if (!boss->rowInitFlag[i]) {
        boss->rows[i].flags &= ~1;
        Anim_SetKeyframes(&boss->rows[i], &boss->animHdr, 4, 2, 6);
        boss->rowInitFlag[i]++;
        boss->rowDoneFlag[i] = 0;
        Sound_PlaySongChecked(0x3ff);
    }
    if (Anim_Step(&boss->rows[i], &boss->animHdr)) {
        boss->rowInitFlag[i] = 0xff;
        boss->rowDoneFlag[i] = 1;
        Nidhoggr_DisablePartCollider(&boss->colliders[i]);
        Nidhoggr_SetPartExtraFlags(&boss->partExtra[i], 1);
    }
}
