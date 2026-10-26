#include "display_animation.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 51.0%
// Remaining: loop-invariant pointers, packed playback temporaries, and register allocation.
struct ProjectilePosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct ProjectileSprite {
    u32 flags;
    u8 pad4[4];
    u8 scaleX, scaleY;
    u8 padA[6];
    u16 frame;
    u8 pad12[0xA];
    struct ProjectilePosition position;
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
    struct ProjectilePosition *target;
    u8 pad64[0xC];
    u32 animate;
    u16 pad74, collisionMode;
    s16 timer, pad7A, homing;
    u8 pad7E[0x1E];
};
struct ProjectileSlot {
    u32 age, flags;
    u8 pad8[8];
    struct ProjectilePosition delta;
    struct DisplaySequenceState sequence;
    struct ProjectileControl control;
    struct ProjectileSprite sprite;
    struct ProjectileCollider attack, body;
    s32 launchSpeed;
    u8 launchMode;
    u8 pad175[5];
    s16 scale;
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
struct ProjectilePosition *SunOrb_GetPosition(void);
struct ProjectilePosition *Ratatosk_GetPosition(void);
struct ProjectilePosition *Player_GetEffectivePosition(s32);
void RisingActorManager_BeginRaise(s32);
s32 Math_Atan2(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void SceneProjectile_ReleaseSlot(void *, void *, s32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline s32 CellIndex(struct ProjectilePosition *v)
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
static inline void StepAnimation(struct DisplaySequenceState *s, struct ProjectileSprite *sprite)
{
    const u16 *cmd = s->commands + s->index;
    u32 bits;
    sprite->frame = *cmd >> 6;
    bits = (*cmd & 0x30) >> 4;
    if ((s->flags & 1) != (bits & 1))
        sprite->flags |= 4;
    else
        sprite->flags &= ~4;
    if ((s->flags & 2) != (bits & 2))
        sprite->flags |= 8;
    else
        sprite->flags &= ~8;
    s->elapsed++;
    if (s->elapsed >= s->duration) {
        s->elapsed = 0;
        if (s->flags & 4) {
            if (s->index == 0) {
                if (s->flags & 8) {
                    s->elapsed = s->duration;
                    goto updated;
                }
                s->index = s->frameCount - 1;
            } else
                s->index--;
        } else {
            s->index++;
            if (s->index >= s->frameCount) {
                if (s->flags & 8) {
                    s->elapsed = s->duration;
                    s->index = s->frameCount - 1;
                } else
                    s->index = 0;
            }
        }
    updated:
        cmd = s->commands + s->index;
        s->frameWeight = *cmd & 15;
        s->duration = (s->speed * s->frameWeight) >> 6;
        if (!s->duration)
            s->duration = 1;
    }
}
// confidence=low: updates ten scripted projectile slots and their launch delays.
#define SceneProjectile_UpdatePool sub_081FCA44
s32 SceneProjectile_UpdatePool(struct ProjectilePool *pool)
{
    struct ProjectileSlot *p = pool->slots;
    s32 i = 0;
    do {
        struct ProjectileSprite *sprite = &p->sprite;
        struct ProjectileControl *control = &p->control;
        if (pool->used & (1 << i)) {
            struct ProjectilePosition *position = &sprite->position;
            if (sub_08249240(p, (void *)i, control->update) == 1)
                goto release;
            sprite->flags &= ~0x200;
            if (control->timer > -10) {
                p->flags &= ~2;
                if (control->timer == 120) {
                    p->flags |= 1;
                    control->speed = 0;
                }
                if (control->timer > Div(120, 2)) {
                    p->scale -= 2;
                    if (p->scale < 0)
                        p->scale = 0;
                    sprite->scaleX = p->scale;
                    sprite->scaleY = p->scale;
                    position->y -= 4;
                }
                if (control->timer == Div(120, 2)) {
                    if (p->attack.extent == 16)
                        p->age = control->word2A;
                    else if (Ratatosk_GetSelectedPointIndex() >= 0) {
                        *position = *SunOrb_GetPosition();
                        position->y += 128;
                        p->scale = 64;
                        sprite->scaleX = 64;
                        sprite->scaleY = 64;
                        control->word2C = 120;
                        RisingActorManager_BeginRaise(Ratatosk_GetSelectedPointIndex());
                    }
                }
                sprite->flags |= 0x200;
                if ((u16)(control->timer - 31) <= 58)
                    sprite->flags |= 1;
                else if (control->timer <= 29)
                    sprite->flags &= ~1;
                if (control->timer == 1) {
                    if (p->attack.extent == 16) {
                        struct ProjectilePosition *target = Player_GetEffectivePosition(0);
                        s32 direction =
                            Math_Atan2(target->x - position->x, target->z - position->z);
                        u32 random;
                        control->direction = direction;
                        gRandomIndex = (gRandomIndex + 1) & 1023;
                        random = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
                        control->direction = direction - 8 + (random & 15);
                        control->word2C = 0;
                        control->speed = 18;
                    } else {
                        struct ProjectilePosition *target = Ratatosk_GetPosition();
                        control->direction =
                            Math_Atan2(target->x - position->x, target->z - position->z);
                        control->target = Ratatosk_GetPosition();
                        control->speed = p->launchSpeed;
                        control->byte2F = p->launchMode;
                    }
                    p->flags |= 1;
                    p->age = 0;
                }
                control->timer--;
            }
            if (p->flags & 1) {
                struct ProjectilePosition *d = &p->delta;
                s32 direction = control->direction, speed = control->speed;
                d->x = Truncate(gUnk_086149C4[(direction + 64) & 255] * speed);
                d->y = 0;
                d->z = Truncate(gUnk_086149C4[direction & 255] * speed);
            }
            position->x += p->delta.x;
            position->y += p->delta.y;
            position->z += p->delta.z;
            if (control->homing) {
                s32 dx = control->target->x - position->x, dz = control->target->z - position->z;
                u32 height;
                if (dx * dx + dz * dz <= 1023)
                    goto release;
                height = (*Cell(CellIndex(position)) & 15) << 8;
                if (height > (u32)position->y)
                    goto release;
            } else {
                p->body.flags |= 4;
                if (p->flags & 2) {
                    Collider_SetPosition(&p->attack, position, 0);
                    Collider_Refresh(&p->attack);
                    if (control->collisionMode == 2) {
                        Collider_SetPosition(&p->body, position, 0);
                        p->body.flags &= 0xFFFB;
                    }
                }
            }
            if (control->animate)
                StepAnimation(&p->sequence, sprite);
            p->age++;
            goto next;
        release:
            SceneProjectile_ReleaseSlot(pool, p, i);
        }
    next:
        i++;
        p++;
    } while (i <= 9);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081FCA44);
#endif
