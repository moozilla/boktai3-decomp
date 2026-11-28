#include "global.h"
#include "engine/engine.h"
#include "system/game_state/game_state_flags.h"
#include "player/player.h"
#include "objects/scene_object.h"
#include "player/cues/player_cue.h"
#include "player/player_slot_status.h"
#include "link/session/link_session_state.h"
#include "scene/collision/scene_actor_damage_visibility.h"
#include "scene/terrain/terrain_cell_probe.h"
#include "scene/terrain/terrain_sampling.h"

extern struct AreaEffectLinkState *gLinkSession;

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

s32 Div(s32, s32);
void Player_SetTetherMode(struct Player *, s32, s32);
s32 LinkPlayer_BeginTerrainRecovery(struct Player *);
s32 DamageNumberDisplay_Show(void *, u32, u32, void *, void *, u32);
void SceneHud_SetTimedState(u32, s32, s32, u32, u32, s32);

s32 Player_ApplyTerrainDamage(struct Player *p)
{
    u8 *data;
    u32 flash;

    data = cellProps(Terrain_GetCellIndex((struct ContactPosition *)&p->position));
    if ((SceneCell_GetFlags((struct SceneCell *)data) & 0x40) && p->tetherStatusFlags == 0) {
        if (GameState_TestWorldFlags(0x4000))
            Player_SetTetherMode(p, 0x22, 0);
        else
            Player_SetTetherMode(p, 0x21, 0);
        p->particleEmissionCount = 0;
        goto one;
    }
    flash = 0;
    if (p->particleEmissionCount) {
        s32 bit = SceneActorDamage_LocalPlayer();
        if (((s16)p->lastContactFlags >> bit) & 1)
            flash = 1;
    }
    if (p->terrainClampApplied)
        p->particleEmissionCount += LinkPlayer_BeginTerrainRecovery(p);
    if (p->positionResetValue == 0 && p->movementModifierKind == 1 && p->movementModifierActive) {
        Player_SetStatusBits(&p->statusFlags, 0x200);
        if (++p->terrainDamageTimer >= Div(0x3c, p->movementModifierActive)) {
            p->particleEmissionCount += 10;
            p->terrainDamageTimer = 0;
        }
    }
    if (p->particleEmissionCount) {
        if (p == 0 || !Busy(p)) {
            DamageNumberDisplay_Show(&p->collider, p->particleEmissionCount, 0, &p->position,
                                     &p->movementRequestTarget, 0);
            if (p->currentStatA < p->particleEmissionCount)
                p->currentStatA = 0;
            else
                p->currentStatA -= p->particleEmissionCount;
        }
    }
    p->particleEmissionCount = 0;
    if (flash)
        SceneHud_SetTimedState(PlayerCue_Messages(p)->receiverIdentifier, p->currentStatA,
                               p->maximumStatA, 10, 0, -1);
    if (p->currentStatA != 0)
        return 0;
one:
    return 1;
}
