#include "effect_emitter.h"
// MATCHING_CONCESSION: register-pinning

void EffectEmitter_ReceiveHit(struct EmitterAttack *attack, void *unused,
                              struct EffectEmitter *object)
{
    s32 damage = attack->damage;
    struct EmitterVec position;
    if (!damage) {
        u32 mask = 64;
        if (attack->flags & mask)
            damage = 50;
        else
            damage = 30;
    }
    if (!object->cooldown && damage > 0) {
        object->cooldown = 8;
        AnimData_SelectPaletteBank(&object->resource, 487);
        object->flash = 8;
        object->health -= damage;
        if (object->health <= 0) {
            object->health = object->maximum;
            position = object->sprite.position;
            position.y += 64;
            EffectManager_SpawnParticlesAt(object->count, 1, object->kind, &position, 0, 600, 100);
        }
    }
}

static inline s32 isVisible(void *scene)
{
    void *saved = scene;
    s32 result;
    if ((void *)gUnk_02000710->region == saved) {
        result = 1;
        asm("" : : "r"(result));
    } else {
        result = 0;
    }
    return result;
}
static inline void setFrame(struct EmitterSprite *sprite, u16 frame)
{
    sprite->frame = frame;
}
s32 EffectEmitter_Update(struct EffectEmitter *object)
{
    s32 visible = isVisible(object->scene);
    if (!visible) {
        object->sprite.flags |= 1;
        object->collider.state |= 4;
        ActorShadow_Disable(object->shadow);
    } else {
        object->sprite.flags &= ~1;
        object->collider.state &= ~4;
        ActorShadow_Enable(object->shadow);
        if (object->cooldown)
            object->cooldown--;
        if (object->flash) {
            object->flash--;
            if (!object->flash)
                AnimData_SelectPaletteBank(&object->resource, 486);
        }
        if (++object->timer >= gUnk_08604AB4[object->phase]) {
            object->timer = 0;
            object->phase++;
            if (object->phase > 3)
                object->phase = 0;
            setFrame(&object->sprite, object->phase);
        }

        if (object->rising) {
            object->offset += 2;
            if (object->offset > 50) {
                object->offset = 50;
                object->rising = 0;
            }
        } else {
            object->offset -= 2;
            if (object->offset < -50) {
                object->offset = -50;
                object->rising = 1;
            }
        }
        object->sprite.position.y = object->baseY + object->offset;
    }
    return 0;
}

u32 EffectEmitter_Destroy(struct EffectEmitter *object)
{
    Collider_Remove(&object->collider);
    Sprite_Remove(&object->sprite);
    Shadow_Destroy(object->shadow);
    return 0;
}

s32 EffectEmitter_Initialize(struct EffectEmitter *object, u32 id, u32 unused)
{
    struct SceneExtent position, size, offset;
    struct SceneCollider *collision;
    struct SceneAnimation *resource;
    struct EmitterSprite *sprite;
    u32 zero = 0;
    object->id = id;
    object->maximum = Script_GetKeywordValue(108, 200);
    object->health = 1;
    object->cooldown = zero;
    object->count = Script_GetKeywordValue(110, 4);
    object->kind = Script_GetKeywordValue(101, 50);
    object->phase = zero;
    object->timer = zero;
    object->flash = zero;
    if (Script_SeekToKeyword(112)) {
        position.x = Script_GetValue();
        position.y = Script_GetValue() + 100;
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    object->scene = Region_FindContaining(&position);
    object->rising = 0;
    object->baseY = ((struct EmitterVec *)&position)->y;
    object->offset = 0;
    size.x = 30;
    size.y = 320;
    size.z = 30;
    offset.x = 0;
    offset.y = 160;
    offset.z = 0;
    collision = &object->collider;
    Collider_Init(collision, 0, 0x4001, 0, 16, &size, &offset);
    Collider_SetCallback(collision, EffectEmitter_ReceiveHit, object);
    Collider_SetPosition(collision, (struct SceneVec *)&position, 0);
    Collider_Refresh(collision);
    resource = &object->resource;
    AnimData_Load(resource, 1642);
    sprite = &object->sprite;
    Sprite_Init(sprite, resource, 0);
    sprite->frame = 0;
    AnimData_SelectPaletteBank(resource, 486);
    object->sprite.position = *(struct EmitterVec *)&position;
    Shadow_Initialize(object->shadow, &object->sprite.position, 0);
    return 0;
}

void *EffectEmitter_Create(u32 a, u32 b)
{
    void *p = Task_Create(8, 0x118);
    if (p) {
        Task_SetCallbacks(p, EffectEmitter_Update, EffectEmitter_Destroy);
        if (EffectEmitter_Initialize(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
