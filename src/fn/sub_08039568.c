#include "global.h"
struct Task {
    u8 pad0[0x18];
    void *listener;
    u16 state;
};
void *Script_GetKeywordValue(int, int);
void Video_SetRenderCallbacks(int, void *, void *, void *);
void Video_OnFrameNoOp(void);
void Video_OnRenderNoOp(void);
void Sprite_OnRenderPhase(void);
int WirelessExit_Init(struct Task *p)
{
    p->listener = Script_GetKeywordValue(101, 0);
    p->state = 0;
    Video_SetRenderCallbacks(0, Video_OnFrameNoOp, Video_OnRenderNoOp, Sprite_OnRenderPhase);
    return 0;
}
