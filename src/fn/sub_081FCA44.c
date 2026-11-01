#include "display_animation.h"
#include "display_sequence_inline.h"
#include "scene_vec.h"
#include "engine.h"
struct ProjectileSprite {
    u32 flags;
    u8 pad4[4];
    u8 scaleX, scaleY;
    u8 padA[6];
    u16 frame;
    u8 pad12[0xA];
    struct SceneVec position;
    u8 pad24[8];
};
struct ProjectileCollider {
    u8 pad0[6];
    u16 flags;
    u16 pad8, extent;
    u8 padC[0x48];
};
struct ProjectileControl {
    u16 word28, word2A, word2C;
    s8 direction;
    u8 byte2F;
    s32 speed;
    u8 pad34[4];
    void *update;
    u8 pad3C[0x24];
    struct SceneVec *target;
    u8 pad64[0xC];
    u32 animate;
    u16 pad74;
    s16 collisionMode;
    s16 timer, pad7A, homing;
    u8 pad7E[0x1E];
};
struct ProjectileLaunch {
    s32 speed;
    u8 mode;
    u8 unknown5[5];
    s16 scale;
};
struct ProjectileSlot {
    u32 age, flags;
    u8 pad8[8];
    struct SceneVec delta;
    struct DisplaySequenceState sequence;
    struct ProjectileControl control;
    struct ProjectileSprite sprite;
    struct ProjectileCollider attack, body;
    struct ProjectileLaunch launch;
    u8 pad17C[0x44];
};
struct ProjectilePool {
    u8 pad0[0x18];
    u32 used;
    u8 pad1C[4];
    struct ProjectileSlot slots[10];
};
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
s32 Ratatosk_GetSelectedPointIndex(void);
struct SceneVec *SunOrb_GetPosition(void);
struct SceneVec *Ratatosk_GetPosition(void);
struct SceneVec *Player_GetEffectivePosition(s32);
void RisingActorManager_BeginRaise(s32);
s32 Math_Atan2(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void SceneProjectile_ReleaseSlot(void *, void *, s32);
struct ProjectileSpriteAttackView {
    struct ProjectileSprite sprite;
    struct ProjectileCollider attack;
};
static inline u16 AttackExtent(struct ProjectileSprite *sprite)
{
    return ((struct ProjectileSpriteAttackView *)sprite)->attack.extent;
}
static inline void Shrink(struct ProjectileLaunch *launch, s16 *scale,
                          struct ProjectileSprite *sprite)
{
    launch->scale -= 2;
    if (launch->scale < 0)
        launch->scale = 0;
    sprite->scaleX = *scale;
    sprite->scaleY = *scale;
    sprite->position.y -= 4;
}
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline void Polar(struct SceneVec *out, struct ProjectileControl *control)
{
    s32 angle = control->direction;
    s32 speed = control->speed;
    out->x = Truncate(speed * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = Truncate(speed * gUnk_086149C4[angle & 255]);
}
static inline s32 CellIndex(struct SceneVec *v)
{
    s32 x = v->x >> 8, z = v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *Cell(s32 i)
{
    u8 *cell = TerrainModifiers_FindCellOverride(i, 1);
    if (cell)
        return cell + 4;
    return gTerrainMap->cells + 12 + i * 4;
}
static inline void StepAnimation(struct ProjectileSlot *cursor, void *obj)
{
    struct ProjectileSprite *sprite = obj;
    struct DisplaySequenceState *state = &cursor->sequence;
    const u16 *command;
    u32 flags;
    {
        u32 offset = DisplaySequence_GetIndex(&cursor->sequence) << 1;
        const u16 *table = cursor->sequence.commands;
        command = (const u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        u32 first = 1;
        u32 bit = 1;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        flags = state->flags;
        if (flags & 4) {
            if (state->index == 0) {
                u32 hold = 8;
                hold &= flags;
                if (hold)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else
                state->index--;
        } else {
            if (++state->index >= state->frameCount) {
                u32 hold = 8;
                hold &= flags;
                if ((u8)hold) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else
                    state->index = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        {
            s32 weight = state->frameWeight;
            s32 speed = state->speed;
            speed *= weight;
            speed >>= 6;
            state->duration = speed;
            if (!(u8)speed)
                state->duration = 1;
        }
    }
}
// confidence=low: updates ten scripted projectile slots and their launch delays.
#define SceneProjectile_UpdatePool sub_081FCA44

s32 SceneProjectile_UpdatePool(struct ProjectilePool *pool)
{
    struct ProjectileSlot *p;
    s32 i;
    struct ProjectileSprite *geometry;
    struct ProjectileCollider *collider;
    struct ProjectileSprite *animationSprite;
    struct SceneVec *terrainPosition;
    for (p = pool->slots, i = 0; i <= 9; i++, p++) {
        struct ProjectileSprite *sprite = &p->sprite;
        struct ProjectileLaunch *launch = &p->launch;
        u32 active = pool->used & (1 << i);
        terrainPosition = &p->sprite.position;
        if (active) {
            struct ProjectileControl *control;
            struct SceneVec *position, *d;
            s32 result;
            geometry = sprite;
            collider = &p->attack;
            control = &p->control;
            result = sub_08249240(p, (void *)i, control->update);
            animationSprite = sprite;
            if (result == 1) {
                SceneProjectile_ReleaseSlot(pool, p, i);
            } else {
                sprite->flags &= ~0x200;
                if (control->timer > -10) {
                    {
                        u32 mask = ~2;
                        p->flags &= mask;
                    }
                    if (control->timer == 120) {
                        p->flags |= 1;
                        control->speed = 0;
                    }
                    if (control->timer > Div(120, 2)) {
                        Shrink(launch, &p->launch.scale, sprite);
                    }
                    if (control->timer == Div(120, 2)) {
                        if (AttackExtent(sprite) == 16)
                            p->age = control->word2A;
                        else if (Ratatosk_GetSelectedPointIndex() >= 0) {
                            sprite->position = *SunOrb_GetPosition();
                            sprite->position.y += 128;
                            launch->scale = 64;
                            sprite->scaleX = 64;
                            sprite->scaleY = 64;
                            control->word2C = 120;
                            RisingActorManager_BeginRaise(Ratatosk_GetSelectedPointIndex());
                        }
                    }
                    animationSprite->flags |= 0x200;
                    if ((u16)(control->timer - 31) <= 58)
                        animationSprite->flags |= 1;
                    else if (control->timer <= 29)
                        animationSprite->flags &= ~1;
                    if (control->timer == 1) {
                        if (AttackExtent(sprite) == 16) {
                            struct SceneVec *target = Player_GetEffectivePosition(0);
                            s32 direction = Math_Atan2(target->x - sprite->position.x,
                                                       target->z - sprite->position.z);
                            u32 random;
                            s32 aim;
                            p->control.direction = direction;
                            gRandomIndex = (gRandomIndex + 1) & 1023;
                            random = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
                            aim = direction + ((random & 15) - 8);
                            p->control.direction = aim;
                            control->word2C = 0;
                            control->speed = 18;
                        } else {
                            struct SceneVec *target = Ratatosk_GetPosition();
                            p->control.direction = Math_Atan2(target->x - sprite->position.x,
                                                              target->z - sprite->position.z);
                            p->control.target = Ratatosk_GetPosition();
                            control->speed = launch->speed;
                            control->byte2F = launch->mode;
                        }
                        p->flags |= 1;
                        p->age = 0;
                    }
                    control->timer--;
                }
                if (p->flags & 1)
                    Polar(&p->delta, control);
                d = &p->delta;
                position = &geometry->position;
                geometry->position.x += p->delta.x;
                position->y += d->y;
                position->z += d->z;
                if (control->homing) {
                    s32 dx = control->target->x - sprite->position.x,
                        dz = control->target->z - sprite->position.z;
                    u32 height;
                    if (dx * dx + dz * dz <= 1023)
                        goto release;
                    height = (*Cell(CellIndex(terrainPosition)) & 15) << 8;
                    if (height > (u32)sprite->position.y) {
                    release:
                        SceneProjectile_ReleaseSlot(pool, p, i);
                        continue;
                    }
                } else {
                    u32 mask;
                    p->body.flags |= 4;
                    mask = 2;
                    if (p->flags & mask) {
                        Collider_SetPosition(collider, position, 0);
                        Collider_Refresh(collider);
                        if (p->control.collisionMode == 2) {
                            collider = &p->body;
                            Collider_SetPosition(collider, position, 0);
                            collider->flags &= 0xFFFB;
                        }
                    }
                }
                if (p->control.animate)
                    StepAnimation(p, animationSprite);
                p->age++;
            }
        }
        terrainPosition = &p->sprite.position;
    }
    return 0;
}
