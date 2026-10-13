#include "global.h"
struct Args {
    unsigned count : 16;
    void *values;
};
void sub_0821AD08(void *, struct Args *);
void ClockEvents_RunScript(u8 *p, int event)
{
    struct Args args;
    args.count = 1;
    args.values = &event;
    if (*(void **)(p + 40))
        sub_0821AD08(*(void **)(p + 40), &args);
}
