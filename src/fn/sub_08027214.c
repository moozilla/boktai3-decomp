#include "global.h"
#include "engine.h"
#include "terrain_cell_probe.h"

void SceneActors_RunFirstEventScript(void *, void *);
void SceneActors_RunSecondEventScript(void *, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
u32 SceneActorGraphics_GetResourceId(void *);
void SceneActors_StopBackgroundFlash(void *);
void RouteSpritePool_RequestFinishAll(void);
void WirelessGame_NotifyOpponentDefeated(void);
void sub_08022790(void *, s32);
void sub_08022878(void *);
void GameState_SetEventFlag(s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);

struct __attribute__((packed)) DeathState {
    u8 value;
};

struct DeathVector {
    s16 x, y, z, pad;
};

struct DeathActor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x2];
    struct Pos16 position;
    struct DeathVector velocity;
    u8 pad64[0x38];
    u32 flags;
    u8 padA0[0x2];
    struct DeathState state;
    u8 frame;
    u32 elapsed;
    u8 facing4;
    u8 facing8;
    u8 padAA[0x7];
    u8 entered;
    u8 padB2[0x16e];
    u8 overlay[0x20];
    u8 body[0xc];
    u16 animationFrame;
    u8 pad24E[0x6];
    u16 animationState;
    u8 pad256[0x7];
    u8 animationFlags;
    u8 pad25E[0x2];
    u8 scriptContext[0xcc];
    struct DeathVector origin;
    u8 pad334[0xe];
    u8 heading;
    u8 variant;
    u8 pad344[0x2];
    u16 minimumScale;
    u16 maximumScale;
    u8 pad34A[0x8];
    s16 floorHeight;
    u8 pad354[0x4];
    u32 entryMode;
    u8 pad35C[0x29];
    u8 retreatTimer;
    u8 pad386[0x6];
    u8 indicatorState;
    u8 pad38D[0x1];
    u8 indicatorMode;
    u8 indicatorStep;
    u32 resource;
    u8 pad394[0x7];
    u8 delay;
    u8 pad39C[0x10];
    u32 mode;
};

static inline s32 sub_08027214_Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline u32 Death_GetTerrainHeight(struct Pos16 *position)
{
    u8 *data = cellProps(cellIndexAt(position));
    u32 kind = *data >> 4;
    u32 height = (*data & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

static inline s32 Death_CellBlocks(struct Pos16 *position)
{
    u8 *data = cellProps(cellIndexAt(position));
    u16 flags = *(u16 *)(data + 2);
    flags &= 1;
    return flags;
}

static inline void Death_SetVelocity(s16 *vector, s32 angle, s32 speed)
{
    {
        s32 x = sub_08027214_Scale(gUnk_086149C4[(angle + 0x40) & 0xff] * speed);
        s32 y = 0;
        vector[0] = x;
        vector[1] = y;
    }
    vector[2] = sub_08027214_Scale(gUnk_086149C4[angle & 0xff] * speed);
}

static inline u32 Death_GetMode(struct DeathActor *actor)
{
    return actor->mode;
}

static inline void Death_SetState(struct DeathState *out, struct DeathState state)
{
    *out = state;
}

void sub_08027214(struct DeathActor *base, s32 tick)
{
    struct DeathActor *p = base;
    u8 *pos;

    if (base->entered) {
        u32 zero = 0;
        base->entered = zero;
        SceneActors_RunFirstEventScript(base, (u8 *)base + 0x260);
        if (base->entryMode == 1) {
            s32 angle = base->heading + 0x80;
            struct DeathState state;
            s32 sequence;
            base->angle = angle;
            {
                s32 direction = (base->angle + 0x60) >> 6;
                u32 mask = 3;
                mask &= direction;
                base->facing4 = mask;
            }
            {
                s32 direction = (base->angle + 0x70) >> 5;
                state.value = 7;
                direction = (u8)direction & state.value;
                base->facing8 = direction;
            }
            sequence = (base->mode & 1) ? 0xB : 0x31;
            SceneActors_StartDirectionalAnimation(base, (u8 *)base + 0x240, (u8 *)base + 0x220,
                                                  sequence, 2, 1, 4);
            Death_SetState(&base->state, state);
            base->elapsed = zero;
            Sound_PlaySongChecked(0x359);
        } else {
            u32 flash = Death_GetMode(base) & 1;
            s32 sequence = flash ? 0x20 : 0x46;
            SceneActors_StartUnmirroredAnimation(base, (u8 *)base + 0x240, (u8 *)base + 0x220,
                                                 sequence, 1, zero);
            base->minimumScale = zero;
            base->maximumScale = zero;
            base->variant = 0;
            if (base->mode & 1) {
                base->state.value = 1;
                base->elapsed = zero;
                Sound_PlaySongChecked(0x1C5);
            } else {
                base->state.value = zero;
                base->elapsed = zero;
                Sound_PlaySongChecked(0x359);
            }
        }
        {
            u32 resource = SceneActorGraphics_GetResourceId(p);
            u32 mode = 2;
            u32 step = 1;
            p->resource = resource;
            p->indicatorMode = mode;
            p->indicatorStep = step;
        }
        if (p->indicatorMode == 2) {
            p->indicatorState = 2;
        } else if (p->indicatorMode == 1) {
            p->indicatorState = 0;
        }
        p->retreatTimer = 0;
        RouteSpritePool_RequestFinishAll();
        SceneActors_StopBackgroundFlash(p);
        if (gEngineFlags & 0x1000) {
            WirelessGame_NotifyOpponentDefeated();
        }
    }

    if (Death_GetTerrainHeight(&base->position) < (u32)p->floorHeight ||
        Death_CellBlocks(&base->position)) {
        p->retreatTimer = 8;
    }

    pos = (u8 *)&base->position;
    if (p->retreatTimer) {
        p->retreatTimer--;
        {
            s16 *origin = (s16 *)((u8 *)p + 0x32c);
            s32 dx = origin[0] - *(s16 *)(pos + 0);
            s32 dy = origin[2] - *(s16 *)(pos + 4);
            s32 angle = Math_Atan2(dx, dy);
            Death_SetVelocity((s16 *)((u8 *)base + 0x5c), angle, 20);
        }
    }

    switch (base->state.value) {
        case 0:
            p->delay = 8;
            if (base->frame > 4) {
                base->state.value = 2;
                base->elapsed = 0;
                SceneActors_StartUnmirroredAnimation(base, (u8 *)p + 0x240, (u8 *)p + 0x220, 0x21,
                                                     2, 0);
                Sound_PlaySongChecked(0x350);
            }
            break;
        case 1:
            if ((tick & 3) == 0) {
                sub_08022790(p, 1);
            }
            p->delay = 8;
            if (base->frame > 4) {
                base->state.value = 2;
                base->elapsed = 0;
                SceneActors_StartUnmirroredAnimation(base, (u8 *)p + 0x240, (u8 *)p + 0x220, 0x21,
                                                     2, 0);
                p->mode &= ~1;
                Sound_PlaySongChecked(0x359);
            }
            break;
        case 2:
            if (p->animationState == 1 && p->animationFrame == 0) {
                Sound_PlaySongChecked(0x350);
            }
            if (p->animationFlags & 2) {
                {
                    u32 nextState = 3;
                    base->state.value = nextState;
                }
                base->elapsed = 0;
                SceneActors_StartUnmirroredAnimation(base, (u8 *)p + 0x240, (u8 *)p + 0x220, 0x22,
                                                     2, 0);
                sub_08022878((u8 *)p + 0x260);
            }
            break;
        case 3:
            if (p->animationFlags & 2) {
                {
                    u32 nextState = 4;
                    base->state.value = nextState;
                }
                base->elapsed = 0;
                SceneActors_StartUnmirroredAnimation(base, (u8 *)p + 0x240, (u8 *)p + 0x220, 0x23,
                                                     2, 0);
            }
            break;
        case 4:
            if (p->animationState == 0xA && p->animationFrame == 0) {
                Sound_PlaySongChecked(0x354);
            }
            if (p->animationFlags & 2) {
                {
                    u32 nextState = 5;
                    base->state.value = nextState;
                }
                base->elapsed = 0;
                SceneActors_StartUnmirroredAnimation(base, (u8 *)p + 0x240, (u8 *)p + 0x220, 0x24,
                                                     2, 0);
            }
            break;
        case 5:
            if (p->animationFlags & 2) {
                {
                    u32 nextState = 6;
                    base->state.value = nextState;
                }
                base->elapsed = 0;
                GameState_SetEventFlag(0x4D);
                SceneActors_RunSecondEventScript(base, (u8 *)p + 0x260);
            }
            break;
        case 6:
            break;
        case 7:
            p->delay = 8;
            if (base->elapsed > 0xEF) {
                {
                    u32 nextState = 5;
                    base->state.value = nextState;
                }
                base->elapsed = 0;
            }
            break;
        default:
            break;
    }
    base->elapsed += 1;
    {
        u32 mask = 0xD;
        base->flags |= mask;
    }
}
