#include "global.h"
#include "contact_collider.h"

struct RingVector {
    u32 x, y;
};
struct SceneEffectRing {
    void (*update)(void *);
    void (*destroy)(void *);
    u8 animation[0x1c];
    u8 sprite[0x1c];
    struct RingVector position;
    u8 pad48[8];
    u8 sequence[0x10];
    struct ContactCollider collider;
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
void SceneEffectRing_Update(void *);
void SceneEffect_DestroyRing(void *);
extern const struct ContactPosition gUnk_0824F6B8, gUnk_0824F6C0;

void SceneEffect_InitializeRing(struct RingOwner *owner, struct SceneEffectRing *ring,
                                struct RingVector *pos, u32 mode)
{
    u32 animation = SceneEffect_LoadRingAnimation();
    void *data = ring->animation;
    struct ContactCollider *collider;
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
    collider->offset = gUnk_0824F6B8;
    collider->bounds = gUnk_0824F6C0;
    if (mode) {
        ring->mode = 0x10;
        {
            u8 facing = 0x60;
            collider->direction = facing;
        }
    } else {
        ring->mode = 0;
        {
            u8 facing = 0xe0;
            collider->direction = facing;
        }
    }
    update = SceneEffectRing_Update;
    destroy = SceneEffect_DestroyRing;
    ring->update = update;
    ring->destroy = destroy;
}
