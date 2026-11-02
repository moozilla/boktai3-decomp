#include "player.h"
extern const s16 gUnk_086149C4[];

static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

// confidence=low: initializes a fixed radial offset opposite the stored action heading.
#define Player_InitFacingDriftOffset sub_0823B5D8
void Player_InitFacingDriftOffset(struct Player *p)
{
    if (!p->cameraOffsetActive) {
        u32 angle;
        p->cameraTarget.x = p->position.x;
        p->cameraTarget.y = p->position.y + 150;
        p->cameraTarget.z = p->position.z;
        angle = ((p->coordinateAxis + 1) & 7) << 5;
        p->cameraOffsetX = -scale(gUnk_086149C4[(angle + 64) & 255] * 800);
        p->cameraOffsetY = 0;
        p->cameraOffsetZ = -scale(gUnk_086149C4[angle] * 800);
        p->cameraOffsetActive = 1;
    }
}
