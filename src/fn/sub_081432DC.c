#include "global.h"
#include "player.h"
#include "game_state.h"
#include "scene_facing.h"

// confidence=low: approaches the saved interaction position, then holds an energy-consuming action.
extern const s16 gUnk_086149C4[];
s32 Player_GetMovementSpeed(void *);
s32 Math_Atan2(s32, s32);
void Player_UpdateFacingSide(void *);
s32 ActorVariant_GetTableValue(u32, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
/* Approach the solar stand, charge the player, then release the interaction. */
void Player_UpdateHeldInteraction(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0: {
            s32 x = p->resourceStationPosition.x - p->position.x;
            s32 z = p->resourceStationPosition.z - p->position.z;
            s32 speed = Player_GetMovementSpeed(p);
            p->actionVariant = 0;
            if (x * x + z * z <= speed * speed) {
                speed = 0;
                p->velocityX = x;
                p->velocityZ = z;
                {
                    u32 facing = p->resourceStationFacing;
                    u8 *direction = &p->facing;
                    *direction = facing;
                    Player_UpdateFacingSide(p);
                    Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
                    if (*direction > 4) {
                        p->heading = speed;
                        p->side = 1;
                    } else {
                        p->heading = speed;
                        p->side = speed;
                    }
                    ActorSequence_Begin(p, 8, 1);
                }
            } else {
                s32 angle = Math_Atan2(x, z);
                p->velocityX = scale(gUnk_086149C4[(angle + 64) & 255] * speed);
                p->velocityZ = scale(gUnk_086149C4[angle & 255] * speed);
                p->facing = SceneFacing_EightWay((u16)angle);
                Player_UpdateFacingSide(p);
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 1), 64, 1);
            }
            break;
        }
        case 1: {
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 12), 64, 1)) {
                Sound_PlaySong(0xCB);
                if (!(p->inputKeys[0] & 1)) {
                    ActorSequence_Begin(p, 8, 4);
                } else if (p->actionMode == 6) {
                    Sound_PlaySong(0x124);
                    ActorSequence_Begin(p, 8, 3);
                } else if (p->currentStatB >= p->maximumStatB) {
                    Sound_PlaySong(0x12E);
                    ActorSequence_Begin(p, 8, 3);
                } else if (gGameState->unk77C > 0) {
                    Sound_PlaySong(0xD8);
                    p->chargeSoundActive = 1;
                    ActorSequence_Begin(p, 8, 2);
                } else {
                    Sound_PlaySong(0x124);
                    ActorSequence_Begin(p, 8, 3);
                }
            }
            break;
        }
        case 2: {
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 13), 64, 1);
            if (!(p->inputKeys[0] & 1)) {
                Sound_StopSong(0xD8);
                p->chargeSoundActive = 0;
                ActorSequence_Begin(p, 8, 4);
            } else if (p->currentStatB >= p->maximumStatB) {
                Sound_StopSong(0xD8);
                p->chargeSoundActive = 0;
                Sound_PlaySong(0x12E);
                ActorSequence_Begin(p, 8, 3);
            } else if (gGameState->unk77C <= 0) {
                Sound_StopSong(0xD8);
                p->chargeSoundActive = 0;
                Sound_PlaySong(0x124);
                ActorSequence_Begin(p, 8, 3);
            } else {
                {
                    u32 mask = 0x40;
                    p->statusFlags = mask | p->statusFlags;
                }
            }
            break;
        }
        case 3: {
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 14), 64, 1);
            if (!(p->inputKeys[0] & 1)) {
                ActorSequence_Begin(p, 8, 4);
            }
            break;
        }
        case 4: {
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 15), 64, 1)) {
                ActorSequence_Begin(p, 0, 0);
                Player_UpdateFacingSide(p);
            }
            break;
        }
    }
}
