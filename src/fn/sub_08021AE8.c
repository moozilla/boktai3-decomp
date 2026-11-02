#include "script_utility.h"
#include "state_actor.h"
extern u32 gUnk_02000560;
void SceneActors_RunFirstEventScript(struct StateActor *actor, s16 *position)
{
    int args[8];
    struct ScriptUtilityArguments call;
    u32 script = actor->firstEventScript;
    if (script) {
        int id = script;
        int mask = 4;
        if (!(gUnk_02000560 & mask)) {
            int *in, *out, i;
            struct ScriptUtilityArguments *packet;
            args[0] = position[0];
            args[1] = position[1];
            args[2] = position[2];
            packet = &call;
            in = actor->eventArguments;
            out = args + 3;
            i = 3;
            do {
                *out++ = *in++;
                i--;
            } while (i >= 0);
            {
                u32 mask = 0xffff0000;
                u32 header;
                header = *(u32 *)&call;
                header &= mask;
                header |= 7;
                *(u32 *)&call = header;
            }
            packet->values = args;
            Script_Call(id, packet);
        }
    }
}
