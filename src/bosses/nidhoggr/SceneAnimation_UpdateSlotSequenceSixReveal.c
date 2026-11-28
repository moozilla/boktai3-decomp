#include "bosses/nidhoggr/nidhoggr_animation.h"

void SceneAnimation_UpdateSlotSequenceSixReveal(struct Nidhoggr *boss, int i)
{
    if (!boss->rowInitFlag[i]) {
        boss->rows[i].flags &= ~1;
        Anim_SetKeyframes(&boss->rows[i], &boss->animHdr, 6, 2, 4);
        boss->rowInitFlag[i]++;
        boss->rowDoneFlag[i] = 0;
        {
            struct ContactCollider *collider =
                (struct ContactCollider *)(i * sizeof(struct ContactCollider) + (u32)boss + 0x66d8);
            u32 mask = ~4;

            collider->flags &= mask;
        }
        Sound_PlaySongChecked(0x3fe);
    }
    if (Anim_Step(&boss->rows[i], &boss->animHdr)) {
        boss->rowInitFlag[i] = 0xff;
        boss->rowDoneFlag[i] = 1;
    }
}
