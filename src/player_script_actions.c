#include "player_script.h"

void ScriptActor_BlockActionAccumulatorFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor != NULL)
        actor->actionAccumulatorBlocked = 1;
}

void ScriptActor_SetField4A2FromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player)
        player->burstParameter = 1;
}

void ScriptActor_BlockActionRequestsFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor != NULL)
        actor->actionRequestBlocked = 1;
}

void ScriptActor_AllowActionRequestsFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor != NULL)
        actor->actionRequestBlocked = 0;
}

u8 ScriptActor_HasEnergyOverrideFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor == NULL)
        return 0;
    return actor->statModifierContext.statBOverrideEnabled;
}

void ScriptActor_EnterKindFourFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor != NULL)
        Player_EnterKindFour(actor);
}

void ScriptActor_PlayKindFourSequenceFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    struct Player *player = actor;
    if (actor != NULL) {
        Player_EnterKindFour(actor);
        if (Script_SeekToKeyword('d'))
            actor->facing = Script_GetValue();
        Player_UpdateFacingSide(player);
        ActorSequence_Begin(player, 0x24, 0);
    }
}

s32 ScriptActor_TestFacingPointFromScript(void)
{
    struct PlayerScriptPackedPoint point;
    struct Player *actor, *player;
    s32 index = Script_GetActorIndex();
    actor = gPlayers[index];
    player = actor;
    if (actor != NULL && actor->tetherMode == 12 && Script_SeekToKeyword('p')) {
        s32 dx, dz, direction, angle;
        point.x = Script_GetValue();
        point.y = Script_GetValue();
        point.z = Script_GetValue();
        dx = (s16)point.x - actor->position.x;
        dz = (s16)point.z - actor->position.z;
        actor = NULL;
        if (dx * dx + dz * dz <= 1400 * 1400) {
            direction = ((player->facing + 5) & 7) * 32;
            angle = Math_Atan2(dx, dz);
            direction -= 256;
            if ((u32)((angle - direction) & 255) - 33 > 190 && Script_SeekToKeyword('d') &&
                (u32)((Script_GetValue() - direction) & 255) - 33 > 190)
                return 1;
        }
    }
    return 0;
}

static inline u32 PlayerScript_TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}

u32 ScriptActor_TestFlagBit20FromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player == NULL || !PlayerScript_TestFlags(&player->statusFlags, 0x100000))
        return 0;
    return 1;
}
