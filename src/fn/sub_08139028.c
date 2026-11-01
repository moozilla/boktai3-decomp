#include "global.h"
#include "contact_swarm.h"
#include "ram.h"
#include "background_controller.h"
#include "player_cue_sequence.h"
struct BallisticSpriteVector {
    s32 x, y, z, w;
};
struct BallisticSpriteCamera {
    u8 unk_00[0x44];
    s32 x;
};
struct BallisticSpriteEffect {
    void (*update)(struct BallisticSpriteEffect *);
    u8 unk_04[0x20 - 4];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState anim;
    u8 unk_5C[0x9c - 0x5c];
    struct BallisticSpriteVector position, velocity;
    u8 unk_BC[0xcc - 0xbc];
    s32 yOffset, initialVelocity;
    u8 entered, unk_D5, phase, unk_D7;
    u16 timer;
};
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_086141BC[];
void Sound_PlaySong(u32);
void SoundDelay_Update(struct BallisticSpriteEffect *);
void NativeParticle_CreateWithFlag(void *, s32, void *, s32, s32, s32, s32);
static inline s32 FrameDelay(s32 speed, s32 weight)
{
    return (speed * weight) >> 6;
}
static inline u32 BallisticSpriteEffect_StepAnimation(struct ContactSwarmSprite *sprite,
                                                      struct DisplaySequenceState *state)
{
    return PlayerCue_StepSequenceEnded(sprite, state);
}
static inline u32 RandomValue(void)
{
    return gUnk_086141BC[gEffectRandomTableIndex];
}
// Tentative: update the initial drift and particle trail of a ballistic sprite.
void BallisticSpriteEffect_UpdateLaunch(struct BallisticSpriteEffect *p)
{
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->phase = 0;
        p->timer = 0;
        p->entered = 0;
        Sound_PlaySong(0x1b4);
    }
    p->position.x += p->velocity.x + gUnk_086149C4[p->phase] * (p->timer >> 3);
    p->position.y += p->velocity.y - gUnk_086149C4[p->phase] * (p->timer >> 1);
    p->position.z += p->velocity.z;
    p->phase++;
    p->timer++;
    {
        struct BallisticSpriteVector *pos = &p->position;
        struct ContactPosition *dst = &p->sprite.position;
        u32 yOff = p->yOffset;
        dst->x = (pos->x >> 12) - ((struct BallisticSpriteCamera *)gSceneCamera)->x;
        dst->y = ((s32)(pos->z + (pos->y + yOff)) >> 12) + 0x60;
        dst->z = pos->z >> 12;
    }
    if (p->timer <= 19) {
        struct {
            u32 x : 16, y : 16, z : 16, pad : 16;
        } offset;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        offset.x = (RandomValue() & 15) - 7;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        offset.y = (RandomValue() & 15) - 7;
        NativeParticle_CreateWithFlag(&p->sprite.position, 0, &offset, 1, 1, 0, 0);
    }
    BallisticSpriteEffect_StepAnimation(&p->sprite, &p->anim);
    {
        u16 *timer = &p->timer;
        if (*timer > 150) {
            p->update = SoundDelay_Update;
            p->entered = 1;
        }
    }
}
