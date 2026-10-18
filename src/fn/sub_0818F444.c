#define Nidhoggr_CreateBurstEffects     sub_0818F444
#define Nidhoggr_AllocateSpriteEffect   sub_0818F218
#define Nidhoggr_StepMovingSpriteEffect sub_0818EF64
/* The current build ELF still exports these two original address names. */
#include "global.h"

struct NidhoggrPoolSequence {
    const volatile u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct NidhoggrEffectMotion {
    u16 x, y, z, unknown6;
};
struct NidhoggrPoolSprite {
    u32 flags;
    u8 unknown4[12];
    u16 frame;
    u8 unknown12[10];
    struct NidhoggrEffectMotion position;
    u8 unknown24[8];
};
struct NidhoggrAnimatedSlot {
    void (*update)(struct NidhoggrAnimatedSlot *);
    void (*onComplete)(struct NidhoggrAnimatedSlot *);
    u8 animationData[0x1c];
    struct NidhoggrPoolSprite sprite;
    struct NidhoggrPoolSequence sequence;
    struct NidhoggrEffectMotion motion;
    u8 lifetime;
    u8 unknown69[3];
};
struct NidhoggrPool {
    struct NidhoggrAnimatedSlot slots[24];
    u32 activeSlots;
    u8 effects[48 * 0x188];
    u32 activeEffects[2];
    void *sequenceResource;
};
struct NidhoggrAnimatedSlot *sub_0818F218(struct NidhoggrPool *);
void sub_0818EF64(struct NidhoggrAnimatedSlot *);
void Nidhoggr_DestroySpriteEffect(struct NidhoggrAnimatedSlot *);
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_Init(void *, void *, u32);
void Anim_RestartSequence(struct NidhoggrPoolSequence *, void *, u32, u32, u32);
void ExplosionEffect_Create(void *, u32, u32, u32, u32);
extern const u8 gUnk_08611D1C[];
extern const struct NidhoggrEffectMotion gUnk_0824F214[];

void sub_0818F444(struct NidhoggrPool *pool, struct NidhoggrEffectMotion *position)
{
    s32 i;
    for (i = 0; i <= 5; i++) {
        struct NidhoggrAnimatedSlot *slot = sub_0818F218(pool);
        const struct NidhoggrEffectMotion *motion;
        struct NidhoggrEffectMotion *output;
        if (!slot)
            return;
        AnimData_Load(slot->animationData, 0xc7ed);
        AnimData_SelectPaletteBank(slot->animationData, 0x1a6);
        Sprite_Init(&slot->sprite, slot->animationData, 0);
        Anim_RestartSequence(&slot->sequence, pool->sequenceResource, gUnk_08611D1C[i], 0, 0);
        output = &slot->sprite.position;
        motion = &gUnk_0824F214[i];
        output->x = position->x + motion->x;
        output->y = position->y + motion->y;
        output->z = position->z + motion->z;
        slot->motion = *motion;
        {
            void (*update)(struct NidhoggrAnimatedSlot *) = sub_0818EF64;
            void (*complete)(struct NidhoggrAnimatedSlot *) = Nidhoggr_DestroySpriteEffect;
            slot->update = update;
            slot->onComplete = complete;
        }
        slot->lifetime = 20;
    }
    ExplosionEffect_Create(position, 0x10, 0, 0, 0);
}
