#include "global.h"
#include "engine.h"
#include "ram.h"

// confidence=low: advances falling projectiles and switches to impact on terrain contact.
#define Hresvelgr_UpdateFallingProjectile sub_081A4BF8
struct ProjectilePosition {
    s16 x, y, z, pad;
};
struct FallingProjectile {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    struct ProjectilePosition position;
    u8 unknown28[0x38];
    struct ProjectilePosition precisePosition, velocity;
    u8 shadow[0x6C];
    void *callback;
    u16 timer, phase, unknownE4, delay;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void ScaledShadow_SetGraphic(void *, s32, s32);
void sub_081A4AB4(void *);
void Hresvelgr_RepelNearbyTetherActor(void *);
void Hresvelgr_UpdateProjectileImpact(void *);
void Hresvelgr_SetFallingAttackState(void *, void (*)(void *));
static inline void AddPosition(struct ProjectilePosition *out, struct ProjectilePosition *v)
{
    out->x = out->x + v->x;
    out->y = out->y + v->y;
    out->z = out->z + v->z;
}
static inline s32 CellIndex(struct ProjectilePosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *CellProperties(s32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return (cell * 4 + 12) + gTerrainMap->cells;
}
static inline u32 Height(struct ProjectilePosition *p)
{
    u8 *cell = CellProperties(CellIndex(p));
    u32 shape = *cell >> 4;
    s32 height = (*cell & 15) << 8;
    switch (shape) {
        case 1:
            height -= *(u8 *)&p->z;
            break;
        case 2:
            height -= *(u8 *)&p->x;
            break;
    }
    return height;
}
void Hresvelgr_UpdateFallingProjectile(struct FallingProjectile *p)
{
    u8 *pool = gHresvelgrProjectilePool;
    if (p->delay) {
        if (--p->delay == 0) {
            ScaledShadow_Initialize(p->shadow, &p->position, 64, 64, 64, 64, 6400, 1, 6, 1, 0);
            ScaledShadow_SetGraphic(p->shadow, 0x1C1B, 7);
            p->flags &= ~1;
        }
    } else {
        u16 phase = p->phase;
        void *resource = pool + 0x18;
        if (!phase && p->position.y <= 0x2FF) {
            Anim_SetKeyframes(p, resource, 34, 1, 0);
            p->phase++;
        }
        p->velocity.y--;
        AddPosition(&p->precisePosition, &p->velocity);
        p->position = p->precisePosition;
        Anim_Step(p, resource);
        if ((u32)p->precisePosition.y <= Height(&p->precisePosition)) {
            sub_081A4AB4(p);
            Hresvelgr_RepelNearbyTetherActor(p);
            Hresvelgr_SetFallingAttackState(p, Hresvelgr_UpdateProjectileImpact);
        } else
            p->timer++;
    }
}
