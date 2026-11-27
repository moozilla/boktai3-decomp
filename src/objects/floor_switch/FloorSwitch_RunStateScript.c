#include "global.h"
#include "recovered/script_call_arguments_layout.h"
#include "objects/floor_switch/floor_switch_layout.h"
void Script_Call(u32, struct ScriptCallArguments *);
static inline void fill(struct FloorSwitch *p, s32 *values)
{
    values[0] = p->name;
    values[1] = p->s[0];
    values[2] = p->s[1];
    values[3] = p->s[2];
    values[4] = p->s[3];
    values[5] = p->w[0];
    values[6] = p->w[1];
    values[7] = p->w[2];
    values[8] = p->w[3];
    values[9] = p->field5A;
}
void FloorSwitch_RunStateScript(struct FloorSwitch *p)
{
    s32 values[10];
    struct ScriptCallArguments args;
    if (p->flags88 & 2) {
        if (p->r[0]) {
            fill(p, values);
            args.count = 10;
            args.values = values;
            Script_Call(p->r[0], &args);
        }
    } else if (p->flags88 & 1) {
        if (p->r[1]) {
            fill(p, values);
            args.count = 10;
            args.values = values;
            Script_Call(p->r[1], &args);
        }
    }
}
