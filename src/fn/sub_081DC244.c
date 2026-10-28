#include "global.h"
#include "input.h"
/* ScreenMessage_Update: handles confirm/cancel and the optional yes/no cursor, or waits out the fade, then runs the chosen script and kills the task */

struct ScreenMessage {
    u8 pad0[0x24];
    u32 firstFlags;
    u8 pad28[0xAC - 0x28];
    u32 secondFlags;
    u8 padB0[0x12C - 0xB0];
    s32 state;
    s32 result;
    s32 fadeTimer;
    s32 fadeSpeed;
    s32 selection;
    void *textWindow;
    u8 pad144[0x160 - 0x144];
    u8 pulse[8];
    u8 pulseState[4];
};

extern u32 gScreenParams[];
void Sound_PlaySong(u32);
void TextWindow_Destroy(void *);
s32 Script_Call(u32, void *);
void Task_Kill(void *);
void PairedFlags_SelectActive(struct ScreenMessage *);
void Palette_UpdateHighlightPulse(void *, void *);
void Palette_SetBackdropColor(u32);

void ScreenMessage_Update(struct ScreenMessage *s)
{
    u32 *g;

    switch (s->state) {
        case 0:
            if (gPadState[1] & 1) {
                if (s->selection == 0 || gScreenParams[2] == 0) {
                    Sound_PlaySong(0xDD);
                    TextWindow_Destroy(s->textWindow);
                    if (gScreenParams[2] != 0) {
                        s->firstFlags |= 1;
                        s->secondFlags |= 1;
                    }
                    Script_Call(gScreenParams[3], 0);
                    Task_Kill(s);
                } else {
                    Sound_PlaySong(0xDE);
                    TextWindow_Destroy(s->textWindow);
                    if (gScreenParams[2] != 0) {
                        s->firstFlags |= 1;
                        s->secondFlags |= 1;
                    }
                    Script_Call(gScreenParams[4], 0);
                    Task_Kill(s);
                }
            } else if (gPadState[1] & 2) {
                if (gScreenParams[2] == 0) {
                    Sound_PlaySong(0xDD);
                    TextWindow_Destroy(s->textWindow);
                    Script_Call(gScreenParams[3], 0);
                    Task_Kill(s);
                } else {
                    Sound_PlaySong(0xDE);
                    TextWindow_Destroy(s->textWindow);
                    s->firstFlags |= 1;
                    s->secondFlags |= 1;
                    Script_Call(gScreenParams[5], 0);
                    Task_Kill(s);
                }
            }
            if (gScreenParams[2] == 1) {
                if (((gPadState[1] & 0x20) && s->selection == 1) ||
                    ((gPadState[1] & 0x10) && s->selection == 0)) {
                    s->selection = 1 - s->selection;
                    Sound_PlaySong(0x37C);
                }
                PairedFlags_SelectActive(s);
                Palette_UpdateHighlightPulse(s->pulse, s->pulseState);
            }
            Palette_SetBackdropColor(0x73FF);
            break;
        case 1:
            if (s->fadeTimer > (1 << s->fadeSpeed) || s->fadeSpeed == 0) {
                TextWindow_Destroy(s->textWindow);
                g = gScreenParams;
                if (g[2] != 0) {
                    s->firstFlags |= 1;
                    s->secondFlags |= 1;
                }
                if (s->result == 0)
                    Script_Call(g[5], 0);
                else if (s->result == 1)
                    Script_Call(g[3], 0);
                else
                    Script_Call(g[4], 0);
                Task_Kill(s);
            }
            s->fadeTimer++;
            break;
        case 2:
            break;
    }
}
