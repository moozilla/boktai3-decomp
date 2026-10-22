#include "global.h"
struct State {
    u8 pad0[0x15];
    s8 current;
    u8 pad16[2];
    void *pc;
};
struct Render {
    u32 flags;
};
struct Actor {
    u8 pad0[0x18];
    struct State state;
    u8 pad34[0x17C - 0x34];
    u8 x, y, width, height;
    u16 scriptIndex;
    u8 mode, pad183;
    s8 previousMode;
    u8 pad185[0x18C - 0x185];
    void *custom;
    u8 pad190[0x196 - 0x190];
    s16 wait;
    u8 pad198[0x230 - 0x198];
    s16 offsetX, offsetY, offsetZ;
    u8 pad236[0x28C - 0x236];
    struct Render render;
    u8 pad290[0x2A8 - 0x290];
    u16 frameStep, frameRate;
    u8 pad2AC[0x2CC - 0x2AC];
    void *callback;
};
void TextState_ClearSelectedChoices(struct State *);
void TextState_RunSelectedProcedures(struct State *);
void MessageWindow_ShowMessage(s32);
void Callback_InvokeActor(struct Actor *, void *);
void MessageWindow_ApplyPalette(struct Actor *);
int MessageWindow_UpdateSpeakerPortrait(struct Actor *);
void MessageWindow_ClearTextCompletion(void);
s32 sub_08031840(struct Actor *p)
{
    struct State *state = &p->state;
    if (state->pc) {
        p->custom = state->pc;
        state->pc = 0;
    }
    TextState_ClearSelectedChoices(state);
    if (p->wait >= 0)
        MessageWindow_ShowMessage(p->wait);
    if (p->callback)
        Callback_InvokeActor(p, p->callback);
    if (p->y + p->height <= 9) {
        p->offsetX = 0;
        p->offsetY = -102;
        p->offsetZ = 0;
        p->frameStep = 4;
        p->frameRate = 0x40;
    } else {
        p->offsetX = 0;
        p->offsetY = 0;
        p->offsetZ = 0;
        p->frameStep = 4;
        p->frameRate = 0x40;
    }
    if (p->callback != MessageWindow_ClearTextCompletion) {
        MessageWindow_ApplyPalette(p);
        if (p->previousMode != state->current)
            MessageWindow_UpdateSpeakerPortrait(p);
    } else if (p->previousMode != -1) {
        p->render.flags |= 1;
        p->previousMode = -1;
    }
    TextState_RunSelectedProcedures(state);
    return 0;
}
