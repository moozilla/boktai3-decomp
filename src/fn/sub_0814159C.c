#include "global.h"
#include "scene_facing.h"
#include "render.h"
#include "player.h"

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
    if (!p->cameraOffsetActive) {
        p->cameraOffsetX = gCamera.position.x - p->position.x;
        p->cameraOffsetY = gCamera.position.y - p->position.y - 150;
        p->cameraOffsetZ = gCamera.position.z - p->position.z;
        p->cameraOffsetActive = 1;
        p->cameraOffsetUpdateState = 1;
        Player_ClampCameraOffsets(p);
    }
    if ((s16)p->directionHistory[0] != -1) {
        u32 direction = (((s16)p->directionHistory[0] + 1) & 7) * 32;
        p->cameraOffsetX -= Scale(gUnk_086149C4[(direction + 64) & 255] * 30);
        p->cameraOffsetZ -= Scale(gUnk_086149C4[direction] * 30);
        p->cameraOffsetUpdateState = 0;
        Player_ClampCameraOffsets(p);
    }
    if ((p->cameraOffsetX || p->cameraOffsetZ) && p->cameraInputProcessed) {
        u16 angle = Math_Atan2(p->cameraOffsetX, p->cameraOffsetZ);
        p->facing = SceneFacing_EightWay(angle);
        if (p->actionMode == 3) {
            s16 quadrant = ((angle + 0x60) >> 6) & 3;
            if (quadrant > 1) {
                p->heading = 3 - quadrant;
                p->side = 1;
            } else {
                p->heading = quadrant;
                p->side = 0;
            }
        } else if (p->actionMode != 5 || p->movementMode46B == 5)
            Player_UpdateFacingSide(p);
    }
}
