#include "scene/collision/contact_actor.h"
#include "monsters/poison_hand.h"
#include "engine/math/scene_vec.h"
#include "engine/engine.h"
#include "engine/ram.h"
void PoisonHand_RecordDefeat(void *);
void *SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                    u32, s32, s32, u32, s32, u32, u32);
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
s32 PoisonHand_UpdateDefeatBurst(struct PoisonHand *p)
{
    struct SceneVec position;
    struct ContactPackedVector velocity;
    s32 random, base;
    if (!p->initialDead) {
        s32 f = p->age;
        u32 attributes, palette;
        if (f <= 15) {
            PoisonHand_CopyPosition(&position, (struct SceneVec *)&p->sprite.position);
            PoisonHand_RaiseOrigin(&position);
            random = PoisonHand_NextSpread(gRandomTable);
            base = (u16)p->spread - 8;
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
        if (p->age == 16)
            PoisonHand_RecordDefeat(p);
    }
    return 0;
}
