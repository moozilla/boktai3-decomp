#include "global.h"
#include "engine/engine.h"
#include "player/player.h"
#include "effects/particles/proximity_emitter.h"
#include "scene/motion/scene_actor_terrain.h"

// foundry draft (worker best 99.1%); what differs is mostly register allocation and instruction order
#define SolarRoot_EmitRevealEffects SolarRoot_PlayImpactEffect

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
void SolarRoot_PlayImpactEffect(struct AnimatedMarker *);
s32 Player_ApplyForcedFacing(struct Player *, s32, u32, s32, s32, u32);
u32 TetherActor_RequestKindNineAction(u32, u32);

/* Facing away from a flipped marker. */
#define GROWTH_HAZARD_FACING(flags) (((flags) & 4) ? 0 : 0x40)

void SolarRoot_RevealWhenCellVacant(struct AnimatedMarker *marker)
{
    u32 occupied = 0;
    u32 direction;
    struct Player *player = gPlayers[0];
    s32 tetherCell = SceneActorTerrain_GetCellIndex(TetherActor_GetPosition());
    struct GrowthHazardCollider *collider = &marker->collider;
    collider->flags |= 4;
    if (player->recoveryTerrainCell == marker->terrainCell) {
        marker->flags |= 0x200;
        direction = GROWTH_HAZARD_FACING(marker->flags);
        Player_ApplyForcedFacing(player, 0x80, direction, 0x40, 0, 0);
        occupied = 1;
    }
    if (tetherCell == marker->terrainCell) {
        marker->flags |= 0x200;
        direction = GROWTH_HAZARD_FACING(marker->flags);
        TetherActor_RequestKindNineAction(0x80, direction);
        occupied = 1;
    }
    if (!occupied) {
        Sound_PlaySong(0x14a);
        SolarRoot_PlayImpactEffect(marker);
        AnimatedMarker_BeginReveal(marker);
    }
}
