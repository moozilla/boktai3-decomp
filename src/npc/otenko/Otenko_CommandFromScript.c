#include "npc/otenko/otenko_actor.h"
#include "engine/engine.h"
#include "player/player.h"
#include "engine/ram.h"
struct OtenkoActor *Actor_FindById(void *manager, s32 name);
s32 Math_Atan2(s32 dx, s32 dz);
void sub_081CEE50(void);
/* Set the motion angle and its directional animation rows. */
static inline void setAngle(struct OtenkoActor *npc, s32 angle)
{
    u16 a;
    npc->body.direction = angle;
    a = angle;
    {
        s32 t;
        u32 m;
        t = a + 0x70;
        t >>= 5;
        m = 7;
        m &= t;
        npc->facing8 = m;
    }
    {
        s32 t = a + 0x60;
        u32 m;
        t >>= 6;
        m = 3;
        m &= t;
        npc->facing4 = m;
    }
    npc->targetAngle = npc->body.direction;
}
/* Registry routine 8918: puts scene NPC -n (default: the last one created,
   gSceneNpc) into the scripted state (mode 0x10, state 14, update
   sub_081CEE50), then -p x y z, -d angle, -w face the player, -s sets flag 1. */
void Otenko_CommandFromScript(void)
{
    struct OtenkoActor *npc;
    u8 mode;
    void (*update)(void);
    if (!gSceneNpc)
        return;
    if (Script_SeekToKeyword('n')) {
        void *manager = gSceneActorListManager;
        if (!manager)
            return;
        npc = Actor_FindById(manager, Script_GetKeywordValue('n', 0));
        if (!npc)
            return;
    } else
        npc = (struct OtenkoActor *)gSceneNpc;
    mode = 0x10;
    npc->stage = 0;
    npc->stageEntered = 0;
    npc->state = mode;
    npc->timer = 0;
    npc->commandEntered = 1;
    update = sub_081CEE50;
    npc->prevState = 14;
    npc->stateCallback = update;
    npc->stateFrame = 0;
    npc->animationEntered = 1;
    npc->animationPhase = 0;
    if (Script_SeekToKeyword('p')) {
        npc->body.position.x = Script_GetValue();
        npc->body.position.y = Script_GetValue();
        npc->body.position.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('d'))
        setAngle(npc, Script_GetValue());
    if (Script_GetKeywordValue('w', 0) > 0) {
        struct Player *player = gPlayers[0];
        s16 *pos = &npc->body.position.x;
        setAngle(npc, Math_Atan2(player->position.x - pos[0], player->position.z - pos[2]));
    }
    if (Script_GetKeywordValue('s', 0) > 0)
        npc->commandFlags |= 1;
}
