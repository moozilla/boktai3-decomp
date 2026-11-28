#include "global.h"
#include "player/player.h"

/* tentative: drifts an offset along an 8-way heading and sets facing from it */

s32 Math_Atan2(s32, s32);
extern const s16 gUnk_086149C4[];

void LinkPlayer_ClampCameraOffsets(struct Player *);

struct PlayerFacingByte {
    u8 index;
} __attribute__((packed));

static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

void ActorMotion_ApplyHeadingDrift(struct Player *p)
{
    if (p->cameraOffsetActive == 0) {
        p->cameraTarget.x = p->position.x;
        p->cameraTarget.y = p->position.y + 150;
        p->cameraTarget.z = p->position.z;
        p->cameraOffsetX = 0;
        p->cameraOffsetY = 0;
        p->cameraOffsetZ = 0;
        p->cameraOffsetActive = 1;
        LinkPlayer_ClampCameraOffsets(p);
    }
    if ((s16)p->directionHistory[0] != -1) {
        u32 a = (((s16)p->directionHistory[0] + 1) & 7) * 32;
        p->cameraOffsetX -= scale(gUnk_086149C4[(a + 64) & 255] * 30);
        p->cameraOffsetZ -= scale(gUnk_086149C4[a] * 30);
        LinkPlayer_ClampCameraOffsets(p);
    }
    if ((p->cameraOffsetX != 0 || p->cameraOffsetZ != 0) && p->cameraInputProcessed) {
        s32 angle = ((u16)Math_Atan2(p->cameraOffsetX, p->cameraOffsetZ) + 0x70) >> 5;
        u32 direction = 7;
        s32 signedDirection;
        struct PlayerFacingByte *facing;
        direction &= angle;
        facing = (struct PlayerFacingByte *)&p->facing;
        signedDirection = (s8)direction;
        facing->index = direction;
        if ((u16)signedDirection > 4) {
            p->heading = 8 - signedDirection;
            p->side = 1;
        } else {
            p->heading = direction;
            p->side = 0;
        }
    }
}
