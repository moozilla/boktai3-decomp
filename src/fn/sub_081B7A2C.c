#include "global.h"
#include "engine.h"
#include "player.h"
#include "proximity_emitter.h"

// MATCHING_CONCESSION: register-pinning
// foundry draft (worker best 99.1%); what differs is mostly register allocation and instruction order
#define GrowthHazard_EmitRevealEffects GrowthHazard_PlayImpactEffect

/* Tentative growth-hazard view, consistent with AnimatedMarker_BeginReveal. */
struct GrowthHazardCollider {
    u8 unknown0[6];
    u16 flags;
};
struct AnimatedMarker {
    u32 flags;
    u8 unknown4[0x48 - 4];
    struct GrowthHazardCollider collider;
    u8 unknown50[0xf2 - 0x50];
    u16 terrainCell;
};
void AnimatedMarker_BeginReveal(struct AnimatedMarker *);
void GrowthHazard_PlayImpactEffect(struct AnimatedMarker *);
s32 Player_ApplyForcedFacing(struct Player *, s32, u32, s32, s32, u32);
u32 TetherActor_RequestKindNineAction(u32, u32);

static inline s32 GrowthHazard_GetFacing(u32 flags)
{
    u32 m = 4;
    flags &= m;
    return flags ? 0 : 0x40;
}

static inline s32 GrowthHazard_GetCell(const struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

void GrowthHazard_RevealWhenCellVacant(struct AnimatedMarker *marker)
{
    u32 occupied = 0;
    struct Player *player = gPlayers[0];
    s32 tetherCell = GrowthHazard_GetCell(TetherActor_GetPosition());
    struct GrowthHazardCollider *collider = &marker->collider;
    collider->flags |= 4;
    if (player->recoveryTerrainCell == marker->terrainCell) {
        marker->flags |= 0x200;
        Player_ApplyForcedFacing(player, 0x80, (marker->flags & 4) ? 0 : 0x40, 0x40, 0, 0);
        occupied = 1;
    }
    if (tetherCell == marker->terrainCell) {
        register u32 direction asm("r2");
        marker->flags |= 0x200;
        direction = GrowthHazard_GetFacing(marker->flags);
        TetherActor_RequestKindNineAction(0x80, direction);
        occupied = 1;
    }
    if (!occupied) {
        Sound_PlaySong(0x14a);
        GrowthHazard_PlayImpactEffect(marker);
        AnimatedMarker_BeginReveal(marker);
    }
}
