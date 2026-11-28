#include "npc/sabata_rider/sabata_rider.h"

/* tentative: parses n/P/s/p script keywords into actor fields */

struct ScriptPosition {
    s32 x, y, z, unused;
};
/* gParticipantActors is the player; +0x18 is its motion state (x at +0x28),
 * +0x5F4 its viewport origin X. */
struct MotionState {
    u8 unknown0[0x10];
    s32 x;
};
extern u8 *gParticipantActors;

static inline void Actor_SetScreenPosition(struct RegistryActor *actor, struct ScriptPosition *pos)
{
    u8 *player;
    struct MotionState *state;
    player = gParticipantActors;
    state = (struct MotionState *)(player + 0x18);
    actor->stats.position.y = pos->y << 12;
    actor->stats.position.z = pos->z << 12;
    if (state == NULL) {
        actor->stats.movementOffset = 0;
        actor->stats.position.x = pos->x << 12;
    } else {
        actor->stats.movementOffset = (pos->x << 12) - (state->x + *(s32 *)(player + 0x5F4));
        actor->stats.position.x = state->x + *(s32 *)(player + 0x5F4) + actor->stats.movementOffset;
        actor->stats.relativeOrigin[0] = actor->stats.movementOffset;
        actor->stats.relativeOrigin[2] = actor->stats.position.z;
    }
}

void ActorScript_ParseParams(struct RegistryActor *actor)
{
    struct ScriptPosition position;
    actor->control.owner = (void *)Script_GetKeywordValue('n', 0);
    if (Script_SeekToKeyword('P')) {
        s32 a, b, c, d, e;
        a = Script_GetPc() ? Script_GetValue() + 12 : 12;
        b = Script_GetPc() ? Script_GetValue() + 18 : 18;
        c = Script_GetPc() ? Script_GetValue() + 6 : 6;
        d = Script_GetPc() ? Script_GetValue() : 0;
        e = Script_GetPc() ? Script_GetValue() : 0;
        actor->stats.state48[3] = e;
        actor->stats.state48[0] = a;
        actor->stats.state48[2] = b;
        actor->stats.state48[1] = c;
        actor->stats.state48[4] = d;
    }
    if (Script_SeekToKeyword('s')) {
        s32 health, amount;
        health = Script_GetPc() ? Script_GetValue() : 1000;
        amount = Script_GetPc() ? Script_GetValue() : 100;
        actor->stats.value = health;
        actor->stats.limit = health;
        actor->stats.amount = amount;
    }
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetPc() ? Script_GetValue() : 0;
        position.y = Script_GetPc() ? Script_GetValue() : 0;
        position.z = Script_GetPc() ? Script_GetValue() : 0;
        Actor_SetScreenPosition(actor, &position);
    }
}
