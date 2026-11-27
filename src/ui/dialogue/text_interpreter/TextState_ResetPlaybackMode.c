#include "global.h"
#include "ui/dialogue/text_state.h"

struct G {
    u8 f[0x12];
    s16 a;
};
extern struct G *gGameState;
void TextState_ResetPlaybackMode(struct TextState *p)
{
    s32 v;
    p->entered = 0;
    v = gGameState->a;
    p->mode = v;
}
