#include "global.h"
#define MenuPrompt_SetPhase         Hud_SetUpdateCallback
#define MenuPrompt_SetDisplayLevels Hud_InitSelectionBounds
struct MenuPrompt {
    u8 unk0[0xC4];
    u8 animData[0xA4C - 0xC4];
    u16 timer;
    u8 unkA4E[0x3BE0 - 0xA4E];
    u8 anim[4];
};
s32 Anim_Step(void *, void *);
int SceneHud_UpdateBinaryChoice(struct MenuPrompt *);
void Sound_PlaySong(u32);
void MenuPrompt_FadeChoice(struct MenuPrompt *);
void ScenePanel_BeginDetail(struct MenuPrompt *);
void Hud_SetUpdateCallback(struct MenuPrompt *, void (*)(struct MenuPrompt *));
void MenuPrompt_UpdateChoice(struct MenuPrompt *p)
{
    int choice;
    Anim_Step(p->anim, p->animData);
    choice = SceneHud_UpdateBinaryChoice(p);
    if (choice == 0) {
        Sound_PlaySong(0xDE);
        Hud_SetUpdateCallback(p, MenuPrompt_FadeChoice);
    } else if (choice == 1) {
        Sound_PlaySong(0xDD);
        ScenePanel_BeginDetail(p);
    }
}
