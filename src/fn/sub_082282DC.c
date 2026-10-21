#include "global.h"
#include "engine.h"

struct Msg082282DC {
    u32 a;
    u16 *b;
};
struct Task082282DC {
    u8 pad0[0x18];
    u32 channel;
    u8 args[8];
    u32 script;
    s32 counter;
    u16 pad2c;
    u16 flags;
    u8 pad30[0x20];
    struct Msg082282DC *msgs;
};
s32 MessageQueue_GetMessagesById(u32, struct Msg082282DC **);
s32 Script_RunBlock(u32, void *);

void DelayedScript_Update(struct Task082282DC *t)
{
    s32 n = MessageQueue_GetMessagesById(t->channel, &t->msgs);
    s32 k = n;
    struct Msg082282DC *m = t->msgs;
    k--;
    if (n > 0) {
        u32 sent = 0xFFFF;
        do {
            if (*m->b == sent)
                goto kill;
            m++;
        } while (k-- > 0);
    }
    {
        s32 c = --t->counter;

        s32 r;
        if (c > 0 && !(2 & t->flags))
            return;
        if (1 & (*t).flags)
            r = Script_Call(t->script, t->args);
        else
            r = Script_RunBlock(t->script, t->args);
        if (r == 0) {
            if ((t->flags & 2) && t->counter > 0)
                return;
        kill:
            Task_Kill(t);
        } else
            t->counter = r;
    }
}
