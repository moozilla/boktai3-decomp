#include "global.h"
#include "directional_interaction.h"
#define SceneScriptObject_RunPendingScript sub_081B817C
/* Tentative scene script object; coordinates and five script arguments are observed here. */
struct SceneScriptObject {
    u8 unknown0[0x34];
    s16 x, y, z;
    u8 unknown3A[0xE8 - 0x3A];
    s32 argument;
    s16 parameter;
    u8 unknownEE[0xA];
    u32 pendingScript;
};
void sub_081B817C(struct SceneScriptObject *object)
{
    s32 values[5];
    struct DirectionalScriptArgs args;
    values[0] = object->argument;
    values[1] = object->parameter;
    values[2] = object->x;
    values[3] = object->y;
    values[4] = object->z;
    args.count = 5;
    args.values = values;
    Script_Call(object->pendingScript, &args);
    object->pendingScript = 0;
}
