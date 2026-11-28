#include "global.h"
#include "player/player.h"
#include "player/player_slot_status.h"
#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

s32 Player_BeginPositionRequest(void *, void *, u32, u32, u32, u32, u32, u32, u32);
s32 Player_ApplyPositionRequest(void *, void *, u32, u32, u32, u32, u32, u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void sub_08022694(void *);
void Sound_PlaySongChecked(u32);
u32 Player_GetInputDirection(void *);

struct PackedOffset {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

/* The tracked slot read back through an unsigned byte. */
static inline u8 SceneGraphicsActor_GetTrackedPlayer(struct SceneGraphicsActor *actor)
{
    return actor->graphics.trackedPlayer;
}

/* How far a facing cue has advanced past the value seen at the grab. */
static inline s32 SceneGraphicsActor_FacingDelta(s32 current, s32 previous)
{
    s32 before = previous - 0x100;
    return (current - before) & 0xff;
}

/* Quantize a player input direction to the matching body angle. */
static inline u8 SceneGraphicsActor_DirectionToAngle(u8 direction)
{
    return (((u32)direction << 29) + 0xA0000000) >> 24;
}

// confidence=low: hold the tracked player, taking health periodically, until the player
// changes facing often enough or time runs out.
void SceneActorAnimation_UpdatePlayerGrab(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    s32 slot;
    s32 alternate;
    s32 normal;
    s32 sequence;
    s32 updated;
    s32 reset;
    s32 amount;
    s32 health;

    if (p->actionChanged) {
        p->actionChanged = 0;
        slot = (s8)SceneGraphicsActor_GetTrackedPlayer(actor);
        switch (gPlayers[slot]->actionMode) {
            case 0:
            default:
                alternate = 19;
                normal = 57;
                break;
            case 2:
                alternate = 21;
                normal = 59;
                break;
            case 1:
                alternate = 20;
                normal = 58;
                break;
        }
        if (actor->graphics.animationFlags & 1)
            sequence = alternate;
        else
            sequence = normal;
        SceneActors_StartAnimation(p, &actor->animation, (int)&actor->animationResource, sequence,
                                   1, 4);
        {
            u8 *facing4 = &p->animationFacing4;
            s32 facing;
            if (*facing4 <= 1) {
                u32 angle = 0;
                p->bodyDirection = angle;
                facing = 1;
            } else {
                u32 angle = 0x40;
                p->bodyDirection = angle;
                facing = 2;
            }
            *facing4 = facing;
        }
        {
            s32 shifted = (p->bodyDirection + 0x70) >> 5;
            s32 facing8 = 7;
            facing8 &= shifted;
            {
                u8 *out = &p->animationFacing8;
                *out = facing8;
            }
        }
        {
            int tracked = (s8)SceneGraphicsActor_GetTrackedPlayer(actor);
            struct ContactPosition *position = actor->playerPositions[tracked];
            actor->graphics.grabPosition = *position;
        }
        slot = (s8)SceneGraphicsActor_GetTrackedPlayer(actor);
        Player_BeginPositionRequest(gPlayers[slot], &actor->animation.position, 0, 0, 0, 0, 1, 0,
                                    2);
        if (actor->graphics.trackedPlayer >= 0) {
            u32 facing = ((struct SceneGraphicsActor *)p)
                             ->players[actor->graphics.trackedPlayer]
                             ->facingChangeEffects;
            actor->graphics.grabStartFacing = facing;
            actor->graphics.grabLastFacing = facing;
        }
        actor->graphics.grabInputFrames = 0;
        actor->graphics.grabFacingChanges = 0;
    }

    updated = p->actionTryUpdate(p);
    if (updated) {
        slot = (s8)SceneGraphicsActor_GetTrackedPlayer(actor);
        Player_ApplyPositionRequest(gPlayers[slot], &actor->graphics.grabPosition, 50,
                                    actor->body.direction, 50, 0, 0, 0);
        return;
    }

    reset = p->actionReset(p);
    if (reset) {
        slot = (s8)SceneGraphicsActor_GetTrackedPlayer(actor);
        Player_ApplyPositionRequest(gPlayers[slot], &actor->graphics.grabPosition, 50,
                                    actor->body.direction, 50, 0, 0, 0);
        return;
    }

    {
        u8 facing;
        u32 last;
        struct Player **players;
        struct Player *player;
        s8 *tracked = &actor->graphics.trackedPlayer;
        slot = *tracked;
        {
            u32 offset = slot * 4;
            struct Player **list = ((struct SceneGraphicsActor *)p)->players;
            player = *(struct Player **)((u8 *)list + offset);
            facing = player->facingChangeEffects;
            last = actor->graphics.grabLastFacing;
            players = list;
        }
        if (last != facing) {
            actor->graphics.grabLastFacing = facing;
            actor->graphics.grabFacingChanges++;
        }
        if (actor->graphics.grabFacingChanges > 3) {
            actor->graphics.grabInputAngle =
                SceneGraphicsActor_DirectionToAngle(Player_GetInputDirection(player));
            actor->graphics.grabInputFrames = 16;
            actor->graphics.grabFacingChanges = reset;
        }
        if (actor->animation.frameIndex == 3 && actor->animation.elapsed == 0) {
            slot = *tracked;
            amount = Player_BeginPositionRequest(gPlayers[slot], &actor->animation.position, 0, 0,
                                                 0, 0, 1, actor->graphics.grabHealthAmount, 2);
            health = actor->health;
            SceneActors_SetHealth((struct StateActor *)actor, health + amount);
            if (health != actor->health) {
                struct ContactCollider *contact = &actor->overlayCollider;
                struct PackedOffset offset;
                offset.x = 24;
                offset.y = 0;
                offset.z = 24;
                DamageNumberDisplay_Show(contact, amount, 1,
                                         &actor->body.position, &offset, 1);
            }
            sub_08022694(actor);
            Sound_PlaySongChecked(844);
        }

        slot = (s8)SceneGraphicsActor_GetTrackedPlayer(actor);
        {
            struct Player *target = players[slot];
            s32 previous = actor->graphics.grabStartFacing;
            s32 current = target->facingChangeEffects;
            if (SceneGraphicsActor_FacingDelta(current, previous) > 15)
                actor->graphics.trackedPlayerEscaped = 1;
        }
        if (actor->animation.status & 2) {
            if (PlayerSlot_ReadStatus(0, 4) || actor->graphics.trackedPlayerEscaped || tick > 120)
                p->actionFinished = 1;
        }
    }
}
