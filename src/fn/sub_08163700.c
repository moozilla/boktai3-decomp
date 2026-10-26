#include "global.h"
#include "ram.h"
struct GameOver {
    u8 pad0[0x338];
    const u8 *script;
    u8 pad33C[4];
    u8 state, phase, flag;
    u8 pad343[3];
    u16 timer, duration, delay;
    u8 mode, finished;
    u8 pad34E[2];
    u32 parameter;
};
void TransitionPanel_InitSprites(void *);
void TransitionPanel_InitOverlay(void *);
s32 Script_SeekToKeyword(s32);
const u8 *Script_GetTextSourcePc(void);
s32 Script_GetValue(void);
// confidence=low: initializes the transition panel and reads optional text, timing and mode parameters.
#define GameOver_Initialize sub_08163700
s32 GameOver_Initialize(struct GameOver *p)
{
    s32 present;
    TransitionPanel_InitSprites(p);
    TransitionPanel_InitOverlay(p);
    p->state = 0;
    p->phase = 0;
    p->timer = 0;
    p->flag = 0;
    if (Script_SeekToKeyword('r'))
        p->script = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('c')) {
        p->duration = Script_GetValue();
        p->delay = Script_GetValue();
    } else {
        p->duration = 500;
        p->delay = 250;
    }
    p->mode = 0;
    present = Script_SeekToKeyword('p');
    if (present)
        p->parameter = Script_GetValue();
    else
        p->parameter = present;
    gGameOver = p;
    p->finished = 0;
    return 0;
}
