#include "global.h"
#include "scene_vec.h"
#include "engine.h"
#include "ram.h"
struct PoisonHandBurst {
    u8 pad0[0x18];
    s32 frame;
    u8 pad1C[0x24];
    struct SceneVec position;
    u8 pad48[0xd8];
    s16 effectX, effectY, effectZ, unknown126;
    u8 pad128[4];
    u16 spread;
    u8 pad12E[0x16];
    s16 timer;
};
void PoisonHand_RecordDefeat(void *);
void *SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                    u32, s32, s32, u32, s32, u32, u32);
struct BurstVelocity {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline s32 PoisonHand_NextSpread(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}
static inline void PoisonHand_CopyPosition(struct SceneVec *dst, struct SceneVec *src)
{
    *dst = *src;
}
static inline void PoisonHand_RaiseOrigin(struct SceneVec *p)
{
    p->y += 0x80;
}
s32 PoisonHand_UpdateDefeatBurst(struct PoisonHandBurst *p)
{
    struct SceneVec position;
    struct BurstVelocity velocity;
    s32 random, base;
    if (!p->timer) {
        s32 f = p->frame;
        u32 attributes, palette;
        if (f <= 15) {
            PoisonHand_CopyPosition(&position, &p->position);
            PoisonHand_RaiseOrigin(&position);
            random = PoisonHand_NextSpread(gRandomTable);
            base = p->spread - 8;
            velocity.x = base + (random & 15);
            velocity.y = 0;
            velocity.z = base + (PoisonHand_NextSpread(gRandomTable) & 15);
            attributes = 0x8000;
            palette = 0x1c5;
            if (f & 1)
                SpriteEffectController_Create(&position, &velocity, 0x61f9, 0x31f4, 8, 0x200, 9,
                                              palette, 2, 0, 0, 0, p->effectX, p->effectY,
                                              attributes, p->effectZ, 0, 0);
            else
                SpriteEffectController_Create(&position, &velocity, 0x61f9, 0x31f4, 8, 0x400, 9,
                                              palette, 2, 0, 0, 0, p->effectX, p->effectY,
                                              attributes, p->effectZ, 0, 0);
        }
        if (p->frame == 16)
            PoisonHand_RecordDefeat(p);
    }
    return 0;
}
