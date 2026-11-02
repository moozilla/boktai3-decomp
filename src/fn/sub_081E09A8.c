#include "script_prompt.h"
#include "ram.h"
#include "input.h"

void Sound_PlaySong(u32);
void Sound_PauseChannel10(u32);
void Menu_StartFadeOut(s32);
s32 Script_Call(u32, void *);
void Task_Kill(void *);
void AnimPanel_SelectActiveAnimation(struct ScriptPromptScreen *);
void Palette_UpdateHighlightPulse(void *, void *);

void sub_081E09A8(struct ScriptPromptScreen *s)
{
    if (s->timer == 1)
        ScriptPrompt_DrawFooterText(s);
    switch (s->state) {
        case 0:
            if (gPadState[1] & 1) {
                if (s->selection == 0) {
                    Sound_PlaySong(0xDD);
                    s->fadeSpeed = 3;
                    Menu_StartFadeOut(3);
                    Sound_PauseChannel10(2);
                    s->result = 1;
                    s->state = 1;
                } else {
                    Sound_PlaySong(0xDE);
                    Menu_StartFadeOut(s->fadeSpeed);
                    s->result = 0;
                    s->state = 1;
                }
            } else if (gPadState[1] & 2) {
                Sound_PlaySong(0xDE);
                Menu_StartFadeOut(s->fadeSpeed);
                s->result = 0;
                s->state = 1;
            }
            if (((gPadState[1] & 0x20) && s->selection == 1) ||
                ((gPadState[1] & 0x10) && s->selection == 0)) {
                s->selection = 1 - s->selection;
                Sound_PlaySong(0x37C);
            }
            Palette_UpdateHighlightPulse(&s->colors[12], s->pulseState);
            AnimPanel_SelectActiveAnimation(s);
            break;
        case 1:
            if (s->fadeTimer > (1 << s->fadeSpeed) || s->fadeSpeed == 0) {
                s->widgets[2].flags |= 1;
                s->widgets[1].flags |= 1;
                s->widgets[0].flags |= 1;
                if (s->result == 0)
                    Script_Call(gScriptPromptSettings.cancelScript, 0);
                else
                    Script_Call(gScriptPromptSettings.acceptScript, 0);
                Task_Kill(s);
            }
            s->fadeTimer++;
            break;
        case 2:
            break;
    }
    s->timer++;
}
