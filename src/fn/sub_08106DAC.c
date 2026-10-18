#include "global.h"
#include "directional_interaction.h"
/* Tentative trigger payload view, established by the two script-call paths. */
struct ScriptTriggerActor {
    u8 unknown0[0x8C];
    s32 id;
    u32 pendingScript;
    u16 oneShotScript;
    u8 unknown96[2];
    s32 arguments[6];
    s32 oneShotArguments[2];
    u8 unknownB8[8];
    u16 flags;
};
void ScriptTriggerActor_DispatchPendingScripts(struct ScriptTriggerActor *actor)
{
    s32 values[8];
    struct DirectionalScriptArgs args;
    if (actor->flags & 0x100) {
        Memory_Clear(values, 0x20);
        if (actor->oneShotScript) {
            values[0] = actor->id;
            values[1] = actor->oneShotArguments[0];
            values[2] = actor->oneShotArguments[1];
            args.count = 3;
            args.values = values;
            Script_Call(actor->oneShotScript, &args);
            actor->oneShotScript = 0;
        }
    }
    if (actor->flags & 0x80) {
        Memory_Clear(values, 0x20);
        if (actor->pendingScript) {
            values[0] = actor->id;
            values[1] = actor->arguments[0];
            values[2] = actor->arguments[1];
            values[3] = actor->arguments[2];
            values[4] = actor->arguments[3];
            values[5] = actor->arguments[4];
            values[6] = actor->arguments[5];
            values[7] = (actor->flags >> 10) & 1;
            args.count = 8;
            args.values = values;
            Script_Call(actor->pendingScript, &args);
            actor->pendingScript = 0;
        }
    }
    actor->flags &= 0xFA7F;
}
