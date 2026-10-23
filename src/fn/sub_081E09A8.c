#include "global.h"
#ifdef NONMATCHING
// fuzzy: 95.6%

struct ScriptPromptTask {
    u8 pad0[0x24];
    u32 thirdFlags;
    u8 pad28[0xAC - 0x28];
    u32 firstFlags;
    u8 padB0[0x134 - 0xB0];
    u32 secondFlags;
    u8 pad138[0x1B4 - 0x138];
    s32 selection;
    s32 timer;
    s32 fadeTimer;
    s32 fadeSpeed;
    s32 state;
    s32 result;
    u8 pad1CC[0x1EC - 0x1CC];
    u8 pulse[8];
    u8 pulseState[4];
};

extern u16 gUnk_03005260[];
extern u32 gUnk_02000388[];
void Sound_PlaySong(u32);
void Sound_PauseChannel10(u32);
void Menu_StartFadeOut(s32);
s32 Script_Call(u32, void *);
void Task_Kill(void *);
void ScriptPrompt_DrawFooterText(struct ScriptPromptTask *);
void AnimPanel_SelectActiveAnimation(struct ScriptPromptTask *);
void Palette_UpdateHighlightPulse(void *, void *);

void sub_081E09A8(struct ScriptPromptTask *s)
{
    s32 state;
    s32 *statep;
    if (s->timer == 1)
        ScriptPrompt_DrawFooterText(s);
    statep = &s->state;
    state = *statep;
    switch (state) {
        case 0: {
            u32 one;
            if (gUnk_03005260[1] & (one = 1)) {
                if (s->selection == 0) {
                    Sound_PlaySong(0xDD);
                    s->fadeSpeed = 3;
                    Menu_StartFadeOut(3);
                    Sound_PauseChannel10(2);
                    s->result = one;
                    goto set_state;
                } else {
                    goto cancel;
                }
            } else if (gUnk_03005260[1] & 2) {
            cancel:
                Sound_PlaySong(0xDE);
                Menu_StartFadeOut(s->fadeSpeed);
                do {
                    s->result = state;
                set_state:
                    *statep = one;
                } while (0);
            }
            if (((gUnk_03005260[1] & 0x20) && s->selection == 1) ||
                ((gUnk_03005260[1] & 0x10) && s->selection == 0)) {
                s->selection = 1 - s->selection;
                Sound_PlaySong(0x37C);
            }
            Palette_UpdateHighlightPulse(s->pulse, s->pulseState);
            AnimPanel_SelectActiveAnimation(s);
            break;
        }
        case 1:
            if (s->fadeTimer > (1 << s->fadeSpeed) || s->fadeSpeed == 0) {
                s->secondFlags |= 1;
                s->firstFlags |= 1;
                s->thirdFlags |= 1;
                if (s->result == 0)
                    Script_Call(gUnk_02000388[3], 0);
                else
                    Script_Call(gUnk_02000388[2], 0);
                Task_Kill(s);
            }
            s->fadeTimer++;
            break;
        case 2:
            break;
    }
    s->timer++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081E09A8);
#endif
