#define ActorGroup_CallMemberScript sub_081042DC
/* Provisional ActorGroup names: static behavior established; semantic ownership tentative. */
#include "global.h"
#include "directional_interaction.h"
struct ActorGroupMember {
    u8 unk0[0x1C];
    s16 x, y, z;
    u8 unk22[0x13E];
    u32 script;
    s32 parameters[3];
};
void sub_081042DC(struct ActorGroupMember *member)
{
    s32 values[12];
    struct DirectionalScriptArgs arguments;
    u32 script = member->script;
    if (script) {
        values[0] = member->x;
        values[1] = member->y;
        values[2] = member->z;
        values[3] = member->parameters[0];
        values[4] = member->parameters[1];
        values[5] = member->parameters[2];
        values[6] = -1;
        values[7] = 0;
        values[8] = 0;
        values[9] = 0;
        values[10] = 0;
        values[11] = -1;
        arguments.count = 12;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
}
