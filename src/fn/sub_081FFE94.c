#include "homing_projectile.h"
#include "ram.h"
#include "player_cue_sequence.h"
struct HomingManager {
    u8 task[0x18];
    u32 activeMask;
    struct HomingSlot slots[3];
    s32 count;
    u16 palettes[2][16];
};
extern const HomingBehavior gUnk_08613FA8[];
extern const s16 gUnk_086149C4[];
void HomingProjectile_CreateManager(void);
struct HomingSlot *HomingProjectile_FindFreeSlot(struct HomingManager *, s32 *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void Collider_SetPosition(void *, void *, u32);
void Sprite_LinkToList(void *, u32);

static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void Vector(struct ContactPosition *v, s32 angle, s32 inScale)
{
    s32 scale = inScale;
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    {
        s32 sample = gUnk_086149C4[angle & 255];
        v->z = Scale(sample * scale);
    }
}

s32 HomingProjectile_Spawn(struct HomingParams *params, struct ContactPosition *position)
{
    struct HomingManager *m = gHomingManager;
    struct HomingSlot *p;
    s32 index;
    u16 animId, resourceId;
    s32 sequence;
    u8 direction;
    u8 flips[2];
    u8 *first;
    u8 *second;
    u8 *saved;
    u32 check;
    if (!m) {
        HomingProjectile_CreateManager();
        return -1;
    }
    p = HomingProjectile_FindFreeSlot(m, &index);
    if (!p)
        return -1;
    HomingProjectile_SetBehaviorCallback(params, gUnk_08613FA8[params->variant]);
    Memory_Copy(&p->params, params, 0x20);
    p->lifetime = 0;
    p->state = 0;
    p->flashTimer = 0;
    Vector(&p->velocity, params->angle, params->speed);
    p->sprite.position = *position;
    p->target = *position;
    p->origin = *position;
    p->origin.y += 0x80;
    {
        struct HomingCollider *collider = &p->secondaryCollider;
        Collider_SetAttackProperties(collider, params->attack, params->element, 0, 0,
                                     params->attackFlags);
        Collider_SetPosition(collider, position, 0);
        collider++;
        Collider_SetBodyProperties(collider, params->body, 0x402, 1);
        Collider_SetPosition(collider, position, 0);
        {
            s32 mask = ~4;
            collider->flags = mask & collider->flags;
        }
    }
    if (p->params.variant == 0) {
        animId = 0x9247;
        resourceId = 0x1DF8;
        sequence = 7;
    } else {
        animId = 0x48DE;
        resourceId = 0xA4B2;
        sequence = 0;
    }
    AnimData_Load(&p->graphics, animId);
    p->graphics.palette = m->palettes[0];
    p->paletteModeCopy = p->graphics.paletteMode;
    p->graphics.resource = ScriptProc_Start(0x922E, resourceId);
    direction = (((u16)params->angle + 0x60) >> 6) & 3;
    first = &flips[0];
    second = &flips[1];
    check = (u8)(direction - 1);
    saved = second;
    if (check <= 1)
        *first = 1;
    else
        *first = 0;
    if (direction > 1)
        *second = 1;
    else
        *second = 0;
    Anim_SetSequence(&p->sequence, p->graphics.resource, (u16)sequence, flips[0], *saved);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->sprite, &p->sequence);
    p->sprite.flags &= ~1;
    Sprite_LinkToList(&p->sprite, 0);
    p->sprite.priority = 2;
    m->activeMask |= 1 << index;
    m->count++;
    return index;
}
