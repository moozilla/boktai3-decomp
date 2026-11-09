#include "global.h"
#include "display_sequence_sprite.h"

// Allocate an attack particle and initialize its sequence when requested.
#define PlayerAttackEffect_AllocateParticle sub_081606C0
struct AttackParticleSlot {
    u32 life, sequence, mode;
    s32 angle;
};
struct AttackParticleOwner {
    u8 unknown0[0x20];
    struct ContactPosition position;
};
/* PlayerAttackEffect_Create allocates 0x5A8 bytes. Its initializer sets up sixteen slots. */
struct AttackParticleEffect {
    u8 taskAndGraphics[0xC4];
    struct DisplaySequenceSprite sprites[16];
    struct DisplaySequenceState sequences[16];
    struct AttackParticleSlot slots[16];
    u32 frame, unknown588;
    const u8 *facing, *side;
    const struct AttackParticleOwner *owner;
    u32 previous, current, element;
    void *sequenceResource;
};
s32 Math_Atan2(s32, s32);
void Sprite_ApplyAnimation(struct DisplaySequenceSprite *, const u8 *);
u32 Anim_RestartSequence(u8 *, u32, u16, u8, u8);
s32 PlayerAttackEffect_AllocateParticle(struct AttackParticleEffect *effect,
                                        struct ContactPosition *position, s32 lifetime,
                                        void *animation, s32 sequence, s32 mode)
{
    struct AttackParticleSlot *slot = effect->slots;
    struct DisplaySequenceState *state;
    s32 i;
    i = 0;
    state = effect->sequences;
    do {
        if (slot->life == 0) {
            s32 angle;
            (effect->sprites + i)->flags &= ~1;
            effect->sprites[i].position = *position;
            slot->life = lifetime;
            Sprite_ApplyAnimation(&effect->sprites[i], animation);
            slot->sequence = sequence;
            slot->mode = mode;
            angle = Math_Atan2(position->x - effect->owner->position.x,
                               position->z - effect->owner->position.z);
            slot->angle = angle;
            if (*effect->side)
                slot->angle = angle + 64;
            else
                slot->angle = angle - 64;
            slot->angle &= 255;
            if (mode == 1) {
                Anim_RestartSequence((u8 *)state, (u32)effect->sequenceResource, (u16)sequence, 0,
                                     0);
                DisplaySequence_StepSprite(&effect->sprites[i], state);
            }
            return 1;
        }
        state++;
        i++;
        slot++;
    } while (i < 16);
    return 0;
}
