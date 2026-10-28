#include "global.h"
#include "engine.h"
#include "player.h"
s32 Script_GetActorIndex(void);
/* Script: tests flag -f (0-95) in the three flag words at +0x444 of the
   scripted actor chosen by -i; returns the masked word (nonzero if set). */
struct ScriptActorFlagState {
    u8 pad[0x444];
    u32 flags[3];
};

static inline u32 ScriptActor_TestFlag(struct ScriptActorFlagState *actor, u32 flag)
{
    if (flag >= 64)
        return actor->flags[2] & (1u << (flag - 64));
    if (flag >= 32)
        return actor->flags[1] & (1u << (flag - 32));
    return actor->flags[0] & (1u << flag);
}

u32 ScriptActor_TestFlagFromScript(void)
{
    u8 *actor;
    u8 *flagActor;
    s32 i = Script_GetActorIndex();
    actor = gPlayers[i];
    flagActor = actor;
    if (actor && Script_SeekToKeyword('f')) {
        u32 flag = Script_GetValue();
        return ScriptActor_TestFlag((struct ScriptActorFlagState *)flagActor, flag);
    }
    return 0;
}
