#include "global.h"
#include "scene_vec.h"
#include "display_animation.h"
#include "engine.h"
struct ProximitySequence {
    u8 pad0[0x3c];
    struct SceneVec position;
    u8 pad44[0x8c];
    void (*update)(void);
    struct DisplaySequenceState animation;
    s32 triggerDistanceSquared;
    s16 effectX, effectY, effectZ, unknownEE;
    s16 timer, unknownF2, variant;
    u16 spread;
    u8 padF8[8];
    void *resource;
};
extern const u16 gUnk_0203B400[];
s32 Mod(s32, s32);
void ProximitySequence_TestPlayerDistance(void);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void *SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                    u32, s32, s32, u32, s32, u32, u32);

struct BurstVelocity {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline s32 ProximitySequence_NextSpread(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}
static inline void ProximitySequence_CopyPosition(struct SceneVec *dst, struct SceneVec *src)
{
    *dst = *src;
}
static inline void ProximitySequence_RaiseOrigin(struct SceneVec *p)
{
    p->y += 0x80;
}

s32 ProximitySequence_EmitBurst(struct ProximitySequence *p)
{
    struct SceneVec position;
    struct BurstVelocity velocity;
    u32 attributes, palette;
    s32 random, base;
    ProximitySequence_CopyPosition(&position, &p->position);
    ProximitySequence_RaiseOrigin(&position);
    random = ProximitySequence_NextSpread(gUnk_0203B400);
    base = p->spread - 8;
    velocity.x = base + (random & 15);
    velocity.y = 0;
    velocity.z = base + (ProximitySequence_NextSpread(gUnk_0203B400) & 15);
    if (!p->variant) {
        attributes = 0x100000;
        palette = 0x1c6;
    } else {
        attributes = 0x8000;
        palette = 0x1c5;
    }
    if (Mod(p->timer, 10) == 0 && p->timer <= 89) {
        if (p->timer == 80) {
            if (!p->variant)
                Sound_PlaySong(0x517);
            else
                Sound_PlaySong(0x518);
        }
        if (Mod(p->timer, 20) == 0)
            SpriteEffectController_Create(&position, &velocity, 0x61f9, 0x31f4, 8, 0x200, 9,
                                          palette, 2, 0, 0, 0, p->effectX, p->effectY, attributes,
                                          p->effectZ, 0, 0);
        else
            SpriteEffectController_Create(&position, &velocity, 0x61f9, 0x31f4, 8, 0x400, 9,
                                          palette, 2, 0, 0, 0, p->effectX, p->effectY, attributes,
                                          p->effectZ, 0, 0);
    }
    if (p->timer > 0) {
        if (p->timer <= 89)
            Anim_RestartSequence(&p->animation, p->resource, 6, 0, 0);
        p->timer--;
    } else
        p->update = ProximitySequence_TestPlayerDistance;
    return 0;
}
