#ifndef GUARD_BALLISTIC_SPRITE_EFFECT_H
#define GUARD_BALLISTIC_SPRITE_EFFECT_H
#include "particle_emitter.h"

struct BallisticSpriteEffectManager {
    u8 task[0x18];
    struct ParticleEmitter entries[4];
    void *sequenceResource;
};
struct BallisticSpriteBoundsHalf {
    u32 low : 16;
    s32 high : 16;
};
union BallisticSpriteBounds {
    struct {
        struct BallisticSpriteBoundsHalf horizontal, vertical;
    } axis;
    u32 words[2];
};
void SoundDelay_Update(struct ParticleEmitter *);
void BallisticSpriteEffect_UpdateLaunch(struct ParticleEmitter *);
s32 BallisticSpriteEffect_UpdateManager(struct BallisticSpriteEffectManager *);
s32 BallisticSpriteEffect_DestroyManager(struct BallisticSpriteEffectManager *);
s32 SceneProcSprites_Initialize(struct BallisticSpriteEffectManager *);
struct BallisticSpriteEffectManager *BallisticSpriteEffect_EnsureManager(void);
void BallisticSpriteEffect_OnAnimationComplete(void);
void SceneProcSprites_SubmitBounds(struct ParticleEmitter *, void (*)(void), u32, u32);
s32 SceneProcSprites_CreateEntry(struct ScenePoolVec32 *, s32, s32, u32, u16 *);
#endif
