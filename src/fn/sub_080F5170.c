#include "global.h"

// confidence=low: slows the sword component and selects its return path when it stops.
struct SwordReturn {
    u8 pad0[6];
    u8 rotation;
    u8 pad7[0x15];
    u16 x, y, z;
    u8 pad22[0x30];
    u16 bodyRadius;
    u8 pad54[0x52];
    u16 attackRadius;
    u8 padA8[0x58];
    s16 velocityX;
    u16 velocityY;
    s16 velocityZ;
    u8 pad106[0x2e];
    u16 endX, endY, endZ;
    u8 pad13A[2];
    u16 startX, startY, startZ;
    u8 pad142[2];
    s16 deltaX, deltaY, deltaZ;
    u8 pad14A[0x16];
    u8 *owner;
    u8 pad164[0x10];
    void (*callback)(void);
    u8 pad178[0x12];
    u8 direction;
    u8 pad18B[2];
    u8 turnRate;
};
void Sword_UpdateHorizontalWaveReturn(void);
void Sword_UpdateDepthWaveReturn(void);
static inline s32 Half(s32 x)
{
    if (x >= 0)
        return x >> 1;
    return -(-x >> 1);
}
void Sword_DecelerateReturn(struct SwordReturn *input)
{
    struct SwordReturn *p = input;
    u8 *owner = p->owner;
    p->rotation += 20;
    p->x += p->velocityX;
    p->z += p->velocityZ;
    if (!(*(u32 *)(owner + 0xd80) & 7)) {
        if (!p->velocityX && !p->velocityZ) {
            s32 x, z;
            p->attackRadius = 16;
            p->bodyRadius = 16;
            p->endX = p->x;
            p->endY = p->y;
            p->endZ = p->z;
            p->deltaX = p->endX - p->startX;
            p->deltaY = p->endY - p->startY;
            p->deltaZ = p->endZ - p->startZ;
            p->direction = -p->direction;
            p->turnRate = 246;
            x = p->deltaX;
            if (x < 0)
                x = -x;
            z = p->deltaZ;
            if (z < 0)
                z = -z;
            if (x < z)
                p->callback = Sword_UpdateHorizontalWaveReturn;
            else
                p->callback = Sword_UpdateDepthWaveReturn;
        }
        p->velocityX = Half(p->velocityX);
        p->velocityZ = Half(p->velocityZ);
    }
}
