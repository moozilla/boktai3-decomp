#include "global.h"

#ifdef NONMATCHING
// fuzzy: 91.3%
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

static inline void CellCenter(struct SceneVec *out, u32 cell)
{
    s32 x = Mod(cell, *(s16 *)(gUnk_030052F4->cells + 4));
    s32 z = Div(cell, *(s16 *)(gUnk_030052F4->cells + 4));
    u8 *data;
    out->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gUnk_030052F4->cells + (cell * 4 + 12);
    out->y = (*data & 15) << 8;
    out->z = (z << 8) + 0x80;
}

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
static inline void Show(struct SceneActorRegistryResetView *a)
{
    if (a->drawableKind == 0)
        *(u32 *)a->drawable &= ~1;
    else {
        u8 *p = (u8 *)a->drawable + 0x20;
        u32 mask = ~1;
        *(u32 *)(p + 8) &= mask;
    }
}
static inline void Palette(struct SceneActorRegistryResetView *a, u32 bank)
{
    if (a->drawableKind == 0)
        AnimData_SelectPaletteBank((u8 *)a->drawable + 0x2C, bank);
    else
        *(u8 **)((u8 *)a->drawable + 0x68) = gUnk_030042E4 + bank * 32;
}

#define Octopus_ReleaseCapturedTarget sub_080EA65C
void Octopus_ReleaseCapturedTarget(struct SceneActorRegistryResetView *actor, s32 frame)
{
    u8 *drawable = actor->drawable;
    u8 *group = (u8 *)actor + 0x3D4;
    struct OctopusTarget *target;
    struct ContactPosition position;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectProfileEntry(actor, 0x14);
        {
            struct ContactCollider *collider = &actor->colliderA;
            u32 flags = collider->flags;
            flags |= 4;
            flags &= 0xFFF7;
            collider->flags = flags;
        }
        {
            u32 bit = 0x800;
            actor->flags2C0 = bit | actor->flags2C0;
        }
        {
            struct ContactPosition *dest = &actor->position;
            struct GolemRouteLink *link = (void *)(group + 0x74);
            dest->x = link->point->x;
            dest->z = link->point->z;
            dest->y = CellHeight(dest, CellIndex(dest));
        }
        *(struct ContactPosition *)(drawable + 0x1C) = actor->position;
    }
    target = (void *)actor->fieldDC;
    if (frame == 60 || frame == 90) {
        Show(actor);
        Palette(actor, *(u16 *)((u8 *)actor + 0x1E2));
        {
            u32 eight = 8;
            actor->field1E8 = eight;
        }
        {
            u32 bit = 0x8000;
            actor->flags2CC |= bit;
        }
        if (target->kind <= 3) {
            struct Player *player = target->player;
            u32 request = *(u16 *)((u8 *)actor + 0xB8);
            u32 option = *(u32 *)((u8 *)actor + 0xD4);
            Player_BeginPositionRequest(player, (void *)&actor->position, 0, 0, 0, 0, 1, request,
                                        option);
            Sound_PlaySong(0xE6);
        } else if (target->kind & 4) {
            void *victim = target->player;
            sub_0807D2E0(victim, 2, &position, 0, 0, 0);
            SceneActorBehavior_PlayReactionSound(victim);
        }
    } else
        Hide(actor);
    if ((u32)(frame - 51) <= 8)
        Camera_RequestShake(20);
    if ((u32)(frame - 81) <= 8)
        Camera_RequestShake(40);
    if (frame > 120) {
        Hide(actor);
        if (target->kind & 4)
            sub_0807D2E0(target->player, 2, &position, 0, 0x40, 0);
        else if (target->kind <= 3) {
            struct Player *player = target->player;
            s32 turn = (actor->direction + 0x20) & 0xFF;
            s32 angle = (turn >= 0 ? turn >> 6 : -(-turn >> 6)) << 6;
            CellCenter((void *)&position, actor->terrainCell);
            Player_ApplyPositionRequest(player, (void *)&position, 0x80, angle, 60, 0, 0, 0);
            SpriteParticle_SpawnBatch(&actor->position, 1, 1, 1, 0x154);
            Sound_PlaySong(0x1DF);
        }
        SceneActorAnimation_SelectDirectionalFlags(actor, 0x10);
        {
            struct Delay {
                u8 pad[8];
                u8 a, b, c, d;
                u32 ticks;
                void *cb;
            };
            struct Delay *delay = (void *)(group + 0x8C);
            void *cb = SceneActorControl_OnIdle;
            delay->ticks = 60;
            delay->cb = cb;
            delay->a = 0;
            delay->b = 0;
            delay->c = 1;
        }
        SetCtl(actor, SceneActorReaction_RestoreAlternateAfterDelay, 14);
        SetState(actor, 3, 0, 0);
        actor->position.x += 32;
        actor->position.z += 32;
        *(struct ContactPosition *)(drawable + 0x1C) = actor->position;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080EA65C);
#endif
