#include "global.h"
struct Controller {
    u16 id;
    u8 pad[18];
    s16 arguments[4];
};
struct Args {
    u32 count : 16, unused : 16;
    s32 *values;
};
void sub_0821AD08(u32, struct Args *);
void LevelController_RunScript(void *unused, struct Controller *p, u32 script)
{
    s32 values[5];
    struct Args args;
    s32 i;
    if (script) {
        values[0] = p->id;
        for (i = 0; i < 4; i++)
            values[i + 1] = p->arguments[i];
        args.count = 5;
        args.values = values;
        sub_0821AD08(script, &args);
    }
}
