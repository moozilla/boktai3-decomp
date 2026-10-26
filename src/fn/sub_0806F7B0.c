#include "scene_range_watch.h"
#include "script_utility.h"
void SceneRangeWatch_CheckRange(struct SceneRangeWatch *p)
{
    s32 v = gGameState->unk876;
    u32 r;
    if (p->minimum <= v && p->maximum >= v) {
        r = 1;
        if (p->inside == 1)
            return;
    } else {
        r = 0;
        if (p->inside == 0)
            return;
    }
    {
        s32 values[3];
        struct ScriptUtilityArguments args;
        values[0] = v;
        values[1] = p->arguments[0];
        values[2] = p->arguments[1];
        /* Assign only the count bitfield; leave upper-half padding unspecified. */
        args.count = 3;
        args.values = values;
        if (r != 0) {
            if (p->enterScript != 0) {
                Script_Call(p->enterScript, &args);
                p->inside = 1;
            }
        } else {
            if (p->leaveScript != 0) {
                Script_Call(p->leaveScript, &args);
                p->inside = r;
            }
        }
    }
}
