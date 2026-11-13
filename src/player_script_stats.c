#include "player_script.h"
#include "game_state.h"

s32 Script_GetActorIndex(void)
{
    s32 result;
    if (Script_SeekToKeyword(0x69) == 0)
        result = 0;
    else
        result = Script_GetValue();
    return result;
}

s32 Script_ReadPositionOption(s16 *position)
{
    s32 result;
    if (Script_SeekToKeyword(0x70) == 0) {
        result = 0;
    } else {
        position[0] = Script_GetValue();
        position[1] = Script_GetValue();
        position[2] = Script_GetValue();
        result = 1;
    }
    return result;
}

void ScriptActor_SetPositionFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    if (player && Script_ReadPositionOption((s16 *)&player->position)) {
        if (Script_SeekToKeyword('d')) {
            player->facing = Script_GetValue();
            if (player->facing > 4) {
                player->heading = 8 - player->facing;
                player->side = 1;
            } else {
                player->heading = player->facing;
                player->side = 0;
            }
        }
        CellObject_Init(&player->movementContactFlags, &player->position);
        ActorSequence_Begin(player, 0, 0);
        Player_StopActionSounds(player);
    }
}

s32 ScriptActor_GetFacingFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    struct Player *player = actor;
    if (actor == NULL)
        return gGameState->unk36;
    if ((u8)(actor->tetherMode - 4) <= 1)
        return (player->coordinateAxis + 4) & 7;
    return actor->facing;
}

u32 PlayerCue_IsStateFour(void)
{
    if (gPlayers[0] == NULL || gPlayers[0]->kind != 4)
        return 0;
    return 1;
}

s32 ScriptActor_IsLocomotionActiveFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor == NULL)
        return 0;
    return Player_IsActiveInLocomotionState(actor);
}

void ScriptActor_SetLifeEnergyFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    if (player) {
        player->maximumStatA = Script_GetKeywordValue('l', 0);
        player->maximumStatB = Script_GetKeywordValue('e', 0);
        player->currentStatA = player->maximumStatA;
        player->currentStatB = player->maximumStatB;
        *(u16 *)&gGameState->life = player->maximumStatA;
        *(s16 *)&gGameState->unk2c = player->maximumStatB;
        *(u16 *)&gGameState->lifeMax = player->maximumStatA;
        *(u16 *)gGameState->unk2E = player->maximumStatB;
    }
}

void ScriptActor_SetLevelFromScript(void)
{
    struct Player *actor;
    s32 index = Script_GetActorIndex();
    actor = gPlayers[index];
    if (actor) {
        s32 level = Script_GetKeywordValue('l', actor->maximumStatA);
        actor->currentStatA = level;
        gGameState->life = level;
    }
}

void ScriptActor_SetEnergyMaxFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player) {
        u32 result = Script_GetKeywordValue(0x65, player->maximumStatB);
        player->currentStatB = result;
        gGameState->unk2c = result;
    }
}

void Player_AddExpFromScript(void)
{
    if (Script_SeekToKeyword(0x65))
        sub_08159164(Script_GetValue());
}

static inline u32 ScriptActor_TestFlag(struct Player *actor, u32 flag)
{
    if (flag >= 64)
        return actor->actionStateFlags & (1u << (flag - 64));
    if (flag >= 32)
        return actor->actionFlags & (1u << (flag - 32));
    return actor->statBonusFlags & (1u << flag);
}

u32 ScriptActor_TestFlagFromScript(void)
{
    struct Player *actor;
    struct Player *flagActor;
    s32 index = Script_GetActorIndex();
    actor = gPlayers[index];
    flagActor = actor;
    if (actor && Script_SeekToKeyword('f')) {
        u32 flag = Script_GetValue();
        return ScriptActor_TestFlag((struct Player *)flagActor, flag);
    }
    return 0;
}

void Player_InitField42C(void)
{
    struct Player *player = gPlayers[0];
    if (player)
        player->statHundredths = 1000;
}

void Player_SetField434FromScript(void)
{
    struct Player *player = gPlayers[0];
    if (player)
        player->statScale = Script_GetKeywordValue(0x70, 0);
}

void ScriptActor_ApplyItemFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];

    if (player != NULL && Script_SeekToKeyword('E')) {
        s32 itemId = Script_GetValue();
        if (gUnk_08E61280[itemId].actionKind == 5) {
            s32 timer =
                Script_SeekToKeyword('t') ? Script_GetValue() : gUnk_08E61280[itemId].valueB * 60;
            player->sceneMode = gUnk_08E61280[itemId].valueA;
            player->active = timer;
            player->unk536 = itemId;
            player->actionEighteenParameter = 0x16E;
            player->actionEighteenDuration = 48;
        } else if (gUnk_08E61280[itemId].actionKind == 1) {
            s32 amount = Div(player->maximumStatA * gUnk_08E61280[itemId].valueA, 100);
            s32 life = player->currentStatA + amount;
            if (life > player->maximumStatA) {
                amount = player->maximumStatA - player->currentStatA;
                player->currentStatA = player->maximumStatA;
            } else {
                player->currentStatA = life;
            }
            if (amount > 0) {
                DamageNumberDisplay_Show(&player->collider, amount, 1, &player->position,
                                         &player->movementRequestTarget, 0);
            }
            player->actionEighteenParameter = 0x16E;
            player->actionEighteenDuration = 48;
        }
    }
}

s32 ScriptActor_GetField534FromScript(void)
{
    struct Player *actor;
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    actor = player;
    if (player == 0)
        return -1;
    if (actor->active == 0)
        return -1;
    return actor->sceneMode;
}

void ScriptActor_ClearField534FromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player) {
        u16 *mode = &player->sceneMode;
        u32 zero;
        zero = 0;
        *mode = zero;
        player->active = zero;
        player->unk536 = zero;
    }
}

void ScriptActor_SetActionFlagsFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    struct Player *player = actor;
    if (actor != NULL && Script_SeekToKeyword('f')) {
        sub_0813B3AC(actor, Script_GetValue());
        if (Player_GetEventFlags(actor, 0x3ffe))
            actor->actionEnabled = Player_IsEnchantmentAvailable(actor, (s8)actor->thresholdMode);
        if (Script_SeekToKeyword('p'))
            player->scriptActionParameter = Script_GetValue();
    }
}

void ScriptActor_EmitEffectsFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor != NULL)
        Player_EmitQueuedEffects(actor);
}

s32 Player_TestScriptStatus(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player == 0 || Script_SeekToKeyword(0x73) == 0)
        goto fail;
    index = Script_GetValue();
    if (index > 1) {
    fail:
        return 0;
    }
    if (player->scriptStatus[index] == 0)
        goto fail;
    return 1;
}

void Player_SetSlotValueFromScript(void)
{
    s32 slot, value;
    if (Script_SeekToKeyword('s') && (slot = Script_GetValue()) <= 1) {
        if (Script_SeekToKeyword('t'))
            value = Script_GetValue();
        else
            value = gUnk_08610E24[slot];
        if (gPlayers[0])
            sub_0813D834(gPlayers[0], slot, value);
        else
            gPlayerInitialSlotValues[slot] = value;
    }
}

void ScriptActor_ClearSlotFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player != 0 && Script_SeekToKeyword(0x73) != 0)
        Player_ClearStateTimer(player, Script_GetValue());
}

void ScriptActor_ClearSlotsFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player) {
        s32 slot = 0;
        do {
            Player_ClearStateTimer(player, slot);
            slot++;
        } while (slot <= 1);
    }
}

u32 ScriptActor_TestCounter628FromScript(void)
{
    struct Player *player;
    s32 actorIndex = Script_GetActorIndex();
    player = gPlayers[actorIndex];
    if (player != 0) {
        u32 slot = player->timedEventSlot;
        struct GameState *state = gGameState;
        if (slot[state->counters628] > 0)
            goto positiveCounter;
    }
    return 0;
positiveCounter:
    return 1;
}

void ScriptActor_ClearCounter628FromScript(void)
{
    struct Player *player;
    s32 actorIndex = Script_GetActorIndex();
    player = gPlayers[actorIndex];
    if (player) {
        u32 slot = player->timedEventSlot;
        struct GameState *state = gGameState;
        slot[state->counters628] = 0;
    }
}
