#include "global.h"
/* Tentative family name; visual identity is not yet established. */
#include "contact_actor.h"
#include "ram.h"
#define ActorEffect_CreateManager FragmentProjectilePool_GetOrCreate
#define ActorEffect_Allocate      FragmentProjectilePool_AllocSprite
struct ActorEffect {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *resource;
    u8 unknown10[12];
    struct ContactPosition position;
    u8 unknown24[8];
    u8 graphics[28];
    struct DisplaySequenceState animation;
    struct ContactCollider colliders[2];
    u32 value;
    u8 unknown104[0x174 - 0x104];
    u16 frame;
    u8 state, unknown177, unknown178, unknown179, active, mode, unknown17C, variant;
    u16 unknown17E;
    u16 parameter180, unknown182, lifetime, parameter186, parameter188, unknown18A;
};
struct ActorEffectManager {
    u8 task[24];
    void *sequences;
};
struct ActorEffectManager *FragmentProjectilePool_GetOrCreate(void);
struct ActorEffect *FragmentProjectilePool_AllocSprite(void *);
void ActorEffect_InitColliders(struct ActorEffect *, s32, s32, s32, s32);
void ActorEffect_Spawn(struct ContactPosition *position, u32 value, s32 a, s32 b, s32 c, s32 d,
                       s32 parameter188, s32 parameter186, s32 parameter180, s32 variant)
{
    struct ActorEffectManager *manager = gActorEffectManager;
    struct ActorEffect *effect;
    void *graphics;
    if (!manager) {
        manager = FragmentProjectilePool_GetOrCreate();
        if (!manager)
            return;
    }
    effect = FragmentProjectilePool_AllocSprite(manager);
    if (!effect)
        return;
    effect->state = 1;
    effect->frame = 0;
    effect->active = 1;
    effect->value = value;
    effect->parameter186 = parameter186;
    effect->parameter180 = parameter180;
    effect->mode = 4;
    effect->parameter188 = parameter188;
    effect->lifetime = 60;
    effect->variant = variant;
    graphics = effect->graphics;
    AnimData_Load(graphics, 0xd45e);
    Sprite_Init(effect, graphics, 0);
    Anim_SetSequence(&effect->animation, manager->sequences, 20, 0, 0);
    AnimData_SelectPaletteBank(effect->resource, 0x13f);
    effect->position = *position;
    effect->flags |= 1;
    effect->priority = 1;
    ActorEffect_InitColliders(effect, a, b, c, d);
}
