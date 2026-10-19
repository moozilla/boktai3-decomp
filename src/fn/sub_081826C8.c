#define gArmorWalkAnimations gUnk_0824EB94
#define gSineTable           gUnk_086149C4
// Tentative Armor family: on the first frame start the walk animation for the
// current facing quadrant, then every frame drive the motion vector from the
// facing angle and the given speed. Field names past 0x50 are provisional.
#include "global.h"

struct ArmorSpriteState {
    u16 unknown0, unknown2, unknown4, flags;
};
struct ArmorControlState {
    u32 unknown0, flags;
};
struct ArmorMotionVector {
    s16 x, y, z;
};
struct ArmorWalkController {
    u8 pad000[0x48];
    u8 axisX; // 0x48, 0x80-centered
    u8 pad049[0x4C - 0x49];
    u8 axisZ; // 0x4C, 0x80-centered
    u8 pad04D[0x50 - 0x4D];
    s16 motionX; // 0x50
    s16 motionY; // 0x52
    s16 motionZ; // 0x54
    u8 pad056[0xC0 - 0x56];
    u16 stepSoundLock; // 0xC0, nonzero suppresses the surface sound
    u8 pad0C2[0xC4 - 0xC2];
    u16 animationDuration; // 0xC4, turnDuration in Armor_UpdatePositiveTurn
    u8 pad0C6[0xC8 - 0xC6];
    u16 surfaceType; // 0xC8, 1 and 5 quarter the speed, 2 and 5 play a sound
    u8 pad0CA[0xB32 - 0xCA];
    u16 flagsB32; // 0xB32
    u8 padB34[5];
    u8 facing; // 0xB39
    u8 padB3A[6];
    u8 walkState;    // 0xB40
    u8 walkComplete; // 0xB41
};
extern const u32 gArmorWalkAnimations[];
extern s16 gSineTable[];
void Armor_SetAnimationFromStart(u32, u32);
void Sound_PlaySongChecked(u32);

static inline void ClearControlFlags(struct ArmorControlState *c, s32 mask)
{
    c->flags &= mask;
}

static inline s32 RoundMotion(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

static inline void SetMotion(struct ArmorMotionVector *v, s32 angle, s32 speed)
{
    v->x = RoundMotion(gSineTable[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = RoundMotion(gSineTable[angle] * speed);
}

void Armor_UpdateWalk(struct ArmorWalkController *p, s32 speed, s32 duration)
{
    u8 *s = &p->walkState;
    if (*s == 0) {
        u32 four = 4;
        Armor_SetAnimationFromStart((u32)p,
                                    (u32)&gArmorWalkAnimations[((p->facing + 0x60) >> 6) & 3]);
        (*s)++;
        p->walkComplete = 1;
        p->animationDuration = duration;
        ((struct ArmorSpriteState *)((u8 *)p + 0x238))->flags &= ~four;
        ((struct ArmorSpriteState *)((u8 *)p + 0x1E4))->flags &= ~four;
        ((struct ArmorSpriteState *)((u8 *)p + 0x28C))->flags &= ~four;
        ((struct ArmorSpriteState *)((u8 *)p + 0x190))->flags |= four;
        ClearControlFlags((struct ArmorControlState *)((u8 *)p + 0x180), ~1);
        p->flagsB32 &= ~0x20;
    }
    SetMotion((struct ArmorMotionVector *)&p->motionX, p->facing, speed);
    if (p->motionX == 0) {
        s32 d = 0x80 - p->axisX;
        if (d > 0x10)
            p->motionX = 3;
        else if (d < -0x10)
            p->motionX = -3;
    } else if (p->motionZ == 0) {
        s32 d = 0x80 - p->axisZ;
        if (d > 0x10)
            p->motionZ = 3;
        else if (d < -0x10)
            p->motionZ = -3;
    }
    if ((p->surfaceType & 3) == 1) {
        p->motionX >>= 2;
        p->motionZ >>= 2;
    }
    switch (p->surfaceType) {
        case 2:
            if (p->stepSoundLock == 0)
                Sound_PlaySongChecked(0xC2);
            break;
        case 5:
            if (p->stepSoundLock == 0)
                Sound_PlaySongChecked(0xC3);
            break;
    }
}
