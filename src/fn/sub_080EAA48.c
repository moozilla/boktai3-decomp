#include "global.h"

// Remaining differences: register allocation and instruction scheduling; see notes/codex-low.md.
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_vec.h"
#include "gba/macro.h"
#include "gba/syscall.h"

struct Player;
s32 Div(s32, s32);
s32 Mod(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Player_ApplyPositionRequest(struct Player *, struct SceneVec *, s32, u32, s32, s32, s32, u32);
s32 Player_BeginPositionRequest(struct Player *, struct SceneVec *, struct SceneVec *, s32, s32,
                                s32, s32, s32, u32);
void sub_0807D2E0(void *, u32, struct ContactPosition *, u32, u32, u32);
void SceneHud_SetTimedState(u32, s32, u32, u32, u32, u32);
void DamageNumberDisplay_Show(void *, u32, s32, void *, void *, u32);
u32 Octopus_GetAnimationStatus(struct SceneActorRegistryResetView *);
void Octopus_UpdateActiveState(void);
extern u8 *gUnk_02000580;
extern s16 *gUnk_020004B4;

struct OctopusContext {
    u8 unknown0[0x628];
    u32 flags;
    u8 unknown630[0xC];
    u8 startTick, holdTicks, interval;
};

struct OctopusTarget {
    u8 unknown0[8];
    u8 kind;
    u8 unknown9[0x33];
    void *player;
};

struct OctopusDrawable {
    u8 unknown0[7];
    u8 mode : 8;
};

struct OctopusOffset {
    u16 x, y, z, pad;
};
extern const struct OctopusOffset gUnk_086062B0[2][5];

static inline void SetCtl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

struct GolemRoutePoint {
    s16 x, z;
};
struct GolemRouteLink {
    u8 unknown0[8];
    struct GolemRoutePoint *point;
};
static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}

void SceneActorBehavior_PlayReactionSound(void *);
void Camera_RequestShake(u32);
void SpriteParticle_SpawnBatch(void *, u32, u32, u32, u32);
void SceneActorReaction_RestoreAlternateAfterDelay(void);
static inline void Hide(struct SceneActorRegistryResetView *a)
{
    u32 bit = 1;
    if (a->drawableKind == 0)
        *(u32 *)a->drawable |= bit;
    else {
        u8 *p = (u8 *)a->drawable + 0x20;
        *(u32 *)(p + 8) |= bit;
    }
}

static inline void Palette(struct SceneActorRegistryResetView *a, u32 bank)
{
    if (a->drawableKind == 0)
        AnimData_SelectPaletteBank((u8 *)a->drawable + 0x2C, bank);
    else
        *(u8 **)((u8 *)a->drawable + 0x68) = gPaletteBankData + bank * 32;
}

void SceneActorTerrain_DisableCollision(void *);
void Octopus_ReturnToRoutePoint(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u8 *group = (u8 *)actor + 0x3D4;
        u16 *bank;
        struct ContactPosition *dest;
        {
            struct ContactCollider *collider = &actor->colliderA;
            u32 flags = collider->flags;
            u32 bit = 4;
            bit |= flags;
            bit &= 0xFFF7;
            collider->flags = bit;
        }
        {
            u32 bit = 0x800;
            actor->flags2C0 = bit | actor->flags2C0;
        }
        SceneActorAnimation_SelectProfileEntry(actor, 0x14);
        {
            u16 *b = (void *)((u8 *)actor + 0x1E0);
            u32 v = *b + 3;
            u32 kind = actor->drawableKind;
            bank = b;
            if (kind == 0)
                AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, v);
            else
                *(u8 **)((u8 *)actor->drawable + 0x68) = gPaletteBankData + v * 32;
        }
        Hide(actor);
        ((struct OctopusDrawable *)actor->drawable)->mode = 3;
        SpriteParticle_SpawnBatch(&actor->position, 1, 2, 1, 0x154);
        Sound_PlaySong(0x1D9);
        SceneActorTerrain_DisableCollision(actor);
        {
            u16 *offset = &actor->paletteOffset;
            u32 value = *bank + *offset;
            Palette(actor, value);
        }
        SetCtl(actor, SceneActorControl_OnIdle, 16);
        SetState(actor, 3, 0, 0);
        dest = &actor->position;
        {
            struct GolemRouteLink *link = (void *)(group + 0x74);
            dest->x = link->point->x;
            dest->z = link->point->z;
            dest->y = CellHeight(dest, CellIndex(dest));
        }
        if (actor->drawableKind == 0)
            *(struct ContactPosition *)((u8 *)actor->drawable + 0x1C) = actor->position;
        else
            *(struct ContactPosition *)((u8 *)actor->drawable + 0x40) = actor->position;
    }
}
