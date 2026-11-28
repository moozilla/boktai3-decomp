#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/hazards/timed_hazard.h"
#include "scene/collision/packed_contact_vector.h"
#include "scene/motion/scene_actor_terrain.h"

void *ColliderSpritePool_Create(u32, u32);
void TimedHazard_OnDamageContact();
void TimedHazard_OnAttackContact();
void Collider_SetAttackPowerAndForce(void *, u32, u32);

s32 sub_08232420(s32 id, s32 flags, s32 delay, s32 health, struct ContactPosition *position)
{
    struct PackedContactVector size, offset;
    struct TimedHazard *p;
    struct ContactPosition *ground;
    void *resource;
    if (!gColliderSpritePool && !ColliderSpritePool_Create(0xC0DB, 0))
        return -1;
    p = Heap_Alloc(sizeof(*p));
    if (!p)
        return -1;
    Memory_Clear(p, sizeof(*p));
    p->id = id;
    p->flags = flags;
    p->delay = delay;
    p->position = *position;
    ground = &p->position;
    p->ground = SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(ground), ground);
    p->health = health;
    if (p->flags & 1) {
        p->collider = Heap_Alloc(sizeof(struct ContactCollider));
        if (!p->collider)
            return -1;
        Memory_Clear(p->collider, sizeof(struct ContactCollider));
        size.x = 90;
        size.y = 128;
        size.z = 90;
        offset.x = 0;
        offset.y = 128;
        offset.z = 0;
        Collider_Init(p->collider, 0, 0x4001, 0, 16, &size, &offset);
        Collider_SetCallback(p->collider, TimedHazard_OnDamageContact, p);
        Collider_Refresh(p->collider);
        Collider_SetPosition(p->collider, &p->position, 0);
    } else
        p->collider = 0;
    if (p->flags & 1) {
        p->attack = Heap_Alloc(sizeof(struct ContactCollider));
        if (!p->attack)
            return -1;
        Memory_Clear(p->attack, sizeof(struct ContactCollider));
        size.x = 100;
        size.y = 120;
        size.z = 100;
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
        Collider_Init(p->attack, 0, 0x2001, 0, 16, &size, &offset);
        Collider_SetCallback(p->attack, TimedHazard_OnAttackContact, p);
        Collider_SetAttackPowerAndForce(p->attack, 50, 20);
        Collider_Refresh(p->attack);
        Collider_SetPosition(p->attack, &p->position, 0);
    } else
        p->attack = 0;
    resource = p->resource;
    if (!AnimData_Load(resource, 0x863A))
        return -1;
    Sprite_Init(&p->sprite, resource, 1);
    p->sprite.priority = 2;
    Anim_SetSequence(&p->anim, ((struct TimedHazardPool *)gColliderSpritePool)->animation, 1, 0, 0);
    AnimData_SelectPaletteBank(resource, p->id + 0x153);
    if (p->delay == 0) {
        p->state = 1;
        p->timer = 0;
        p->entered = 1;
    } else {
        p->state = 0;
        p->timer = 0;
        p->entered = 1;
    }
    p->remove = 0;
    p->previous = 0;
    p->next = 0;
    return TimedHazard_LinkInstance((struct TimedHazardPool *)gColliderSpritePool, p);
}
/* Script command handler that follows sub_08232420 inside its ROM symbol (no label of its own). */
// Provisional name: TimedHazard_CreateFromScript (hidden function, confidence low)
s32 sub_0823269C(void)
{
    struct PackedContactVector position;
    s32 id, flags, delay, health;
    id = Script_GetKeywordValue('t', 0);
    flags = Script_GetKeywordValue('f', 0);
    delay = Script_GetKeywordValue('e', 0);
    health = Script_GetKeywordValue('l', 0);
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    return sub_08232420(id, flags, delay, health, (struct ContactPosition *)&position);
}
