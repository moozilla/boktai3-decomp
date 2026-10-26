#include "global.h"
#include "ram.h"

struct TextState {
    u8 pad0[0x1c];
    void *pc;
};

struct MsgWin {
    u8 pad0[0x18];
    struct TextState state;
    u8 pad38[0x15e];
    u16 wait;
    u8 pad198[0xc];
    void *script;
    u16 scriptFrame, scriptCount;
    u16 frames[0x20];
    u8 pad1ec[0x2c];
    u32 flags;
};
void *MessageWindow_GetStringAtOffset(struct MsgWin *, int);
void TextState_Reset(void *);

s32 sub_08030CEC(void *script, u32 count, u32 frame, u16 *frames)
{
    struct MsgWin *p = gMessageWindow;
    if (p != 0) {
        struct TextState *state = &p->state;
        s32 i;
        p->script = script;
        state->pc = MessageWindow_GetStringAtOffset(p, 0);
        p->scriptFrame = frame;
        p->scriptCount = count;
        if ((u16)count > 0x20)
            p->scriptCount = 0x20;
        for (i = 0; i < p->scriptCount; i++)
            p->frames[i] = frames[i];
        p->wait = p->frames[p->scriptFrame];
        TextState_Reset(&p->state);
        p->flags |= 1;
    }
    return 0;
}
