// MATCHING_CONCESSION: register-pinning
#include "global.h"
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
    s32 input;
    register s16 *history asm("r2");
    if (!p->cameraOffsetActive) {
        p->cameraOffsetX = gCamera.position.x - p->position.x;
        p->cameraOffsetY = gCamera.position.y - p->position.y - 150;
        p->cameraOffsetZ = gCamera.position.z - p->position.z;
        p->cameraOffsetActive = 1;
        p->cameraOffsetUpdateState = 1;
        Player_ClampCameraOffsets(p);
    }
    history = (s16 *)p->directionHistory;
    input = *history;
    if (input != -1) {
        s32 sum = input;
        s32 d;
        asm("" : "+r"(sum));
        sum += 1;
        sum &= 7;
        d = sum << 5;
        p->cameraOffsetX -= Scale(gUnk_086149C4[(d + 64) & 255] * 30);
        p->cameraOffsetZ -= Scale(gUnk_086149C4[d] * 30);
        p->cameraOffsetUpdateState = 0;
        Player_ClampCameraOffsets(p);
    }
    if ((p->cameraOffsetX || p->cameraOffsetZ) && p->cameraInputProcessed) {
        s32 angle = (u16)Math_Atan2(p->cameraOffsetX, p->cameraOffsetZ);
        s32 direction = (angle + 0x70) >> 5;
        s32 facing;
        register u32 mode asm("r0");
        facing = 7;
        facing &= direction;
        p->facing = facing;
        mode = p->actionMode;
        if (mode == 3) {
            s32 sum = angle + 0x60;
            s32 q;
            q = sum >> 6;
            q &= 3;
            if (q > 1) {
                p->heading = 3 - q;
                p->side = 1;
            } else {
                p->heading = q;
                p->side = 0;
            }
        } else if (mode != 5 || p->unk466[5] == 5)
            Player_UpdateFacingSide(p);
    }
}
