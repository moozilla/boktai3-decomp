#include "scene_particle_burst.h"
#include "scene_particle_step.h"
/* Steps an animated sprite's display sequence (SceneParticle_StepSprite, as in SceneParticleBurst_UpdateSlot); sets result=2 when the sequence ends. */
struct SequencedSprite {
    struct SceneParticleEffect sprite;
    struct DisplaySequenceState state;
    u8 unknown38[0xA];
    u8 result;
    u8 unknown43;
    u8 restore;
};
void SequencedSprite_StepAnimation(u8 *owner, struct SequencedSprite *input)
{
    struct SequencedSprite *s = input;
    struct SceneParticleEffect *sprite = &s->sprite;
    if (s->restore) {
        s->sprite.flags &= ~1;
        s->restore = 0;
    }
    if (s->state.index == 1 && s->state.elapsed == 0)
        *(u16 *)((u8 *)s + 0x1A) += 32;
    {
        const u16 *graphics = *(const u16 **)(owner + 0x1C);
        if (SceneParticle_StepSprite(&s->state, sprite, graphics))
            s->result = 2;
    }
}
