// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct RingVector {
    u32 x, y;
};
struct RingCollider {
    u8 pad[0x14];
    struct RingVector size, offset;
};
struct SceneEffectRing {
    void (*update)(void *);
    void (*destroy)(void *);
    u8 animation[0x1c];
    u8 sprite[0x1c];
    struct RingVector position;
    u8 pad48[8];
    u8 sequence[0x10];
    struct RingCollider collider;
    u8 pad84[0x1e];
    u8 flags;
    u8 padA3[0x11];
    u8 mode, age;
};
struct RingOwner {
    u8 pad[0x24];
    u16 attack, element;
};
u32 SceneEffect_LoadRingAnimation(void);
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_Init(void *, void *, u32);
void Anim_RestartSequence(void *, u32, u32, u32, u32);
void Collider_Init(void *, u16, u32, u32, u32, const void *, const void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void sub_08198DD8(void *);
void SceneEffect_DestroyRing(void *);
extern const struct RingVector gUnk_0824F6B8, gUnk_0824F6C0;

static inline u8 Ring_SetMode(struct SceneEffectRing *ring, u32 mode)
{
    if (mode) {
        ring->mode = 0x10;
        return 0x60;
    } else {
        ring->mode = 0;
        return 0xe0;
    }
}

void SceneEffect_InitializeRing(struct RingOwner *owner, struct SceneEffectRing *ring,
                                struct RingVector *pos, u32 mode)
{
    register u8 flags asm("r0");
    u32 animation = SceneEffect_LoadRingAnimation();
    void *data = ring->animation;
    struct RingCollider *collider;
    void (*update)(void *);
    void (*destroy)(void *);
    AnimData_Load(data, 0x856e);
    AnimData_SelectPaletteBank(data, 0x101);
    Sprite_Init(ring->sprite, data, 0);
    Anim_RestartSequence(ring->sequence, animation, 0, 0, 0);
    ring->position = *pos;
    ring->age = 0;
    collider = &ring->collider;
    Collider_Init(collider, 0, 0x2101, 0, 0x10, &gUnk_0824F6B8, &gUnk_0824F6B8);
    Collider_SetAttackProperties(collider, owner->attack, owner->element, 0x10, 0x80000, 0);
    collider->size = gUnk_0824F6B8;
    collider->offset = gUnk_0824F6C0;
    flags = Ring_SetMode(ring, mode);
    ring->flags = flags;
    update = sub_08198DD8;
    destroy = SceneEffect_DestroyRing;
    ring->update = update;
    ring->destroy = destroy;
}
