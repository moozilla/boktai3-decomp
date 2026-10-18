#include "global.h"
#define MenuPrompt_UpdateChoice     sub_0816A834
#define MenuPrompt_ReadChoice       sub_08165A30
#define MenuPrompt_SetPhase Hud_SetUpdateCallback
#define MenuPrompt_SetDisplayLevels Hud_InitSelectionBounds
struct MenuPrompt {
    u8 unk0[0xC4];
    u8 animData[0xA4C - 0xC4];
    u16 timer;
    u8 unkA4E[0x3BE0 - 0xA4E];
    u8 anim[4];
};
s32 Anim_Step(void *, void *);
int sub_08165A30(struct MenuPrompt *);
void Sound_PlaySong(u32);
void sub_0816A880(struct MenuPrompt *);
void sub_0816A5A8(struct MenuPrompt *);
void Hud_SetUpdateCallback(struct MenuPrompt *, void (*)(struct MenuPrompt *));
void sub_0816A834(struct MenuPrompt *p)
{
    int choice;
    Anim_Step(p->anim, p->animData);
    choice = sub_08165A30(p);
    if (choice == 0) {
        Sound_PlaySong(0xDE);
        Hud_SetUpdateCallback(p, sub_0816A880);
    } else if (choice == 1) {
        Sound_PlaySong(0xDD);
        sub_0816A5A8(p);
    }
}
