#include "script_prompt.h"
#include "ram.h"
#include "input.h"
// MATCHING_CONCESSION: register-pinning

#define SCRIPT_PROMPT_INPUT_BASE ((u16 *)0x03005260)

void Sound_PlaySong(u32);
void Sound_PauseChannel10(u32);
void Menu_StartFadeOut(s32);
s32 Script_Call(u32, void *);
void Task_Kill(void *);
void AnimPanel_SelectActiveAnimation(struct ScriptPromptScreen *);
void Palette_UpdateHighlightPulse(void *, void *);

void sub_081E09A8(struct ScriptPromptScreen *s)
{
    s32 state;
    s32 *statep;
    if (s->timer == 1)
        ScriptPrompt_DrawFooterText(s);
    statep = &s->state;
    state = *statep;
    switch (state) {
        case 0: {
            u32 activeState;
            u16 *input;
            register u32 pressedKeys asm("r1");
            input = SCRIPT_PROMPT_INPUT_BASE;
            pressedKeys = input[1];
            activeState = 1;
            if (activeState & pressedKeys) {
                if (s->selection == 0) {
                    Sound_PlaySong(0xDD);
                    s->fadeSpeed = 3;
                    Menu_StartFadeOut(3);
                    Sound_PauseChannel10(2);
                    s->result = activeState;
                    goto set_state;
                } else {
                    goto cancel;
                }
            } else if (2 & pressedKeys) {
            cancel:
                Sound_PlaySong(0xDE);
                Menu_StartFadeOut(s->fadeSpeed);
                do {
                    s->result = state;
                set_state:
                    *statep = activeState;
                } while (0);
            }
            input = SCRIPT_PROMPT_INPUT_BASE;
            if (((input[1] & 0x20) && s->selection == 1) ||
                ((input[1] & 0x10) && s->selection == 0)) {
                s->selection = 1 - s->selection;
                Sound_PlaySong(0x37C);
            }
            Palette_UpdateHighlightPulse(&s->colors[12], s->pulseState);
            AnimPanel_SelectActiveAnimation(s);
            break;
        }
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
