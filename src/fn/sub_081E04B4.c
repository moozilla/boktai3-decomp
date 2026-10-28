#include "global.h"
#include "menu_parameters.h"
#include "input.h"
// Names are tentative, based on the static state transitions and memory writes.
#define ScreenFade_BeginExit Menu_StartFadeOut
struct StartPrompt {
    u8 unknown[0x98];
    s16 state, frame;
    void *resource;
    s32 fadeTimer, fadeShift;
};
void Sound_PlaySong(u32);
void Menu_StartFadeOut(s32);
void Script_Call(u32, void *);
void Task_Kill(void *);
// The original callback leaves r0 unspecified; preserve its non-void return ABI.
u32 StartPrompt_Update(struct StartPrompt *p)
{
    switch (p->state) {
        case 0:
            if (p->frame <= 20 && !(p->frame & 1))
                Sound_PlaySong(628);
            if (p->frame > 59 && (gPadState[1] & 8)) {
                Sound_PlaySong(221);
                p->state = 1;
                Menu_StartFadeOut(p->fadeShift);
            }
            break;
        case 1:
            if (p->fadeTimer > (1 << p->fadeShift) || p->fadeShift == 0) {
                if (gScriptMessageParameters.value)
                    Script_Call(gScriptMessageParameters.value, 0);
                Task_Kill(p);
            } else {
                p->fadeTimer++;
            }
            break;
    }
    p->frame++;
}
