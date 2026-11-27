#include "scene/terrain/terrain_trap_probe.h"
#include "objects/warp_circle/engine_map.h"
#include "global.h"
struct Args {
    u32 count : 16;
    u32 reserved : 16;
    u32 *values;
};
void Script_CallProcedureWrapper(u32, struct Args *);
void Script_ExecuteBlockWrapper(u32, struct Args *);
void Trap_DispatchEvent(struct TrapProbe *trigger, struct TerrainTrap *trap, u16 event)
{
    u32 values[14];
    struct Args args;
    u32 *out = values;
    s32 i;
    u32 command;
    *out++ = trigger->kind;
    *out++ = trap->id;
    *out++ = event;
    *out++ = trigger->position->x;
    *out++ = trigger->position->y;
    *out++ = trigger->position->z;
    for (i = 0; i < 4; i++)
        *out++ = trap->w[i];
    command = trap->command;
    for (i = 0; i < 4; i++)
        *out++ = trap->s[i];
    args.count = 14;
    args.values = values;
    if (trap->flags & 0x20)
        Script_CallProcedureWrapper(command, &args);
    else
        Script_ExecuteBlockWrapper(command, &args);
}
