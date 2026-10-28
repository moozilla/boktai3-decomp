// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "game_state.h"
#include "player.h"

struct LinkRef {
    u8 unk0[0x18];
    s16 bit;
};
extern struct LinkRef *gLinkSession;

static inline s32 LocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->bit;
}

static inline u32 Busy(struct Player *p)
{
    u32 r;
    u16 a = p->active;
    r = 0;
    if (a != 0) {
        if (p->sceneMode == 3)
            r = 1;
    }
    return r;
}

static inline void FlagOr(u32 m, u32 *f)
{
    *f |= m;
}

static inline u32 FlagSet(u32 v, u32 m)
{
    return v & m;
}

static inline u32 FlagTest(u32 m, u32 *pv)
{
    u32 r;
    if (*pv & m) {
        r = 1;
        asm("" : "+r"(r));
    } else
        r = 0;
    return r;
}

u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Div(s32, s32);
void Player_SetTetherMode(struct Player *, s32, s32);
s32 Player_BeginTerrainRecovery_0823BA78(struct Player *);
s32 DamageNumberDisplay_Show(void *, u32, u32, void *, void *, u32);
void SceneHud_SetTimedState(u32, s32, s32, u32, u32, s32);

s32 Player_ApplyTerrainDamage(struct Player *p)
{
    s32 x = (s16)p->position.x >> 8;
    s32 z = (s16)p->position.z >> 8;
    u32 cell;
    u8 *data;
    u32 flash;

    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    if (FlagSet(*(u16 *)(data + 2), 0x40) && p->tetherStatusFlags == 0) {
        if (FlagTest(0x4000, &gGameState->flags868))
            Player_SetTetherMode(p, 0x22, 0);
        else
            Player_SetTetherMode(p, 0x21, 0);
        p->particleEmissionCount = 0;
        goto one;
    }
    flash = 0;
    if (p->particleEmissionCount) {
        s32 bit = LocalPlayer();
        if ((*(s16 *)((u8 *)p + 0x54E) >> bit) & 1)
            flash = 1;
    }
    if (p->terrainClampApplied)
        p->particleEmissionCount += Player_BeginTerrainRecovery_0823BA78(p);
    if (p->positionResetValue == 0 && p->movementModifierKind == 1 && *(u8 *)((u8 *)p + 0x5BB)) {
        u16 *c;
        u16 n;
        FlagOr(0x200, &p->statusFlags);
        c = (u16 *)((u8 *)p + 0x4C6);
        n = ++*c;
        if (n >= Div(0x3c, *(u8 *)((u8 *)p + 0x5BB))) {
            p->particleEmissionCount += 10;
            *c = 0;
        }
    }
    if (p->particleEmissionCount) {
        if (p == 0 || !Busy(p)) {
            DamageNumberDisplay_Show((u8 *)p + 0x230, p->particleEmissionCount, 0, &p->position,
                                     &p->movementRequestTarget, 0);
            if (p->currentStatA < p->particleEmissionCount)
                p->currentStatA = 0;
            else
                p->currentStatA -= p->particleEmissionCount;
        }
    }
    p->particleEmissionCount = 0;
    if (flash)
        SceneHud_SetTimedState(*(u32 *)((u8 *)p + 0x24), p->currentStatA, p->maximumStatA, 10, 0,
                               -1);
    if (p->currentStatA != 0)
        return 0;
one:
    return 1;
}
