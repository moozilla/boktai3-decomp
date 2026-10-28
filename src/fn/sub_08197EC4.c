#include "global.h"
#include "engine.h"
#include "input.h"

struct Msg08197EC4 {
    u32 a;
    u16 *b;
};
struct Task08197EC4 {
    u8 pad0[0x18];
    u32 channel;
    void *script1;
    void *script2;
    u16 keys;
    u16 flags;
    u8 args[8];
};
s32 MessageQueue_GetMessagesById(u32, struct Msg08197EC4 **);
void Script_RunBlock(void *, void *);

void KeyTrigger_Update(struct Task08197EC4 *t)
{
    struct Msg08197EC4 *m;
    s32 n = MessageQueue_GetMessagesById(t->channel, &m);
    for (; n > 0; n--, m++) {
        switch (*m->b) {
            case 0:
                t->flags &= ~1;
                break;
            case 1:
                t->flags |= 1;
                break;
        }
    }
    if (((t->flags & 2) || !(gEnginePauseFlags & 7)) && (t->flags & 1) &&
        (gPadState[1] & t->keys)) {
        if (t->script1)
            Script_RunBlock(t->script1, t->args);
        if (t->script2)
            Script_Call(t->script2, t->args);
    }
}
