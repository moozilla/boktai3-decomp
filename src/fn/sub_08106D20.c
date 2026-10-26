#include "global.h"
#include "ram.h"

struct P {
    u8 f[0x18];
    u32 a;
    u32 b;
    u8 g[0xa0 - 0x20];
    u32 c;
    u8 d;
};
u32 ScriptProc_Start(s32, s32);

s32 Pool128_InitManager(struct P *p)
{
    p->a = ScriptProc_Start(0x922e, 0x1003);
    p->b = ScriptProc_Start(0x922e, 0x931e);
    gRegionActorPool = p;
    p->c = 0;
    p->d = 0xff;
    return 0;
}
