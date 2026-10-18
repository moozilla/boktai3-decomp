#include "global.h"
#include "directional_interaction.h"
#define ScriptNotifier_CallWithValue sub_081386F0
struct ScriptNotifier {
    u8 unknown0[0x34];
    u16 value;
    u8 unknown36[0x86];
    u32 script;
};
void sub_081386F0(struct ScriptNotifier *notifier)
{
    if (notifier->script) {
        s32 value = notifier->value;
        struct DirectionalScriptArgs args;
        args.count = 1;
        args.values = &value;
        Script_Call(notifier->script, &args);
    }
}
