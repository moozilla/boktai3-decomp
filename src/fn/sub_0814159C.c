#define Player_UpdateCameraOffset sub_0814159C
#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fuzzy: 83.7%
// Camera input and angle allocation, mask scheduling and literal derivation remain.
extern struct SceneVec gUnk_030042A0;
extern const s16 gUnk_086149C4[];
void Player_ClampCameraOffsets(struct Player *);
void Player_UpdateFacingSide(struct Player *);
u16 Math_Atan2(s32, s32);
static inline s32 Scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void Player_UpdateCameraOffset(struct Player *p)
{
    s32 d;
    if (!p->cameraOffsetActive) {
        p->cameraOffsetX = gUnk_030042A0.x - p->position.x;
        p->cameraOffsetY = gUnk_030042A0.y - p->position.y - 150;
        p->cameraOffsetZ = gUnk_030042A0.z - p->position.z;
        p->cameraOffsetActive = 1;
        p->cameraOffsetUpdateState = 1;
        Player_ClampCameraOffsets(p);
    }
    d = (s16)p->directionHistory[0];
    if (d != -1) {
        d = (((s16)p->directionHistory[0] + 1) & 7) << 5;
        p->cameraOffsetX -= Scale(gUnk_086149C4[(d + 64) & 255] * 30);
        p->cameraOffsetZ -= Scale(gUnk_086149C4[d] * 30);
        p->cameraOffsetUpdateState = 0;
        Player_ClampCameraOffsets(p);
    }
    if ((p->cameraOffsetX || p->cameraOffsetZ) && p->cameraInputProcessed) {
        u16 angle = Math_Atan2(p->cameraOffsetX, p->cameraOffsetZ);
        p->facing = 7 & ((angle + 0x70) >> 5);
        if (p->actionMode == 3) {
            s32 q = ((angle + 0x60) >> 6) & 3;
            if (q > 1) {
                p->heading = 3 - q;
                p->side = 1;
            } else {
                p->heading = q;
                p->side = 0;
            }
        } else if (p->actionMode != 5 || p->unk466[5] == 5)
            Player_UpdateFacingSide(p);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Player_UpdateCameraOffset);
#endif
