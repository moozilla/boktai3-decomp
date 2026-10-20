#include "global.h"
struct BestiaryDetails {
    u8 pad[0x26];
    u8 entry, page;
    s32 title, rank, count, unused34, reward[2], label[2];
    u8 pad48;
    u8 name[0xd4e - 0x49];
    u16 flags;
    const u8 *titleScript;
    u8 counts[0xdac - 0xd54];
    u8 rewards[0xef8 - 0xdac];
    const u8 *rewardScript, *labelScript, *emptyScript, *nameScript, *itemScript, *countScript,
        *dialogueScript, *rankScript;
};
s32 GameState_IsEventFlagSet(s32);
s32 GameState_IsPairFlagSet(s32, s32);
s32 ThresholdTrack_ClassifyPosition(s32);
void BackgroundPanel_ReleaseHandles(void *);
void TextWindow_SetScript(s32, const u8 *);
void TextWindow_SetBounds(s32, s32, s32, s32, s32);
void TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
void TextWindow_SetString(s32, s32, const u8 *);
void ScrollingDialogue_ClearScript(void);
#define BestiaryDisplay_DrawEntryDetails sub_081D5478
void BestiaryDisplay_DrawEntryDetails(struct BestiaryDetails *p)
{
    s32 unlocked = GameState_IsEventFlagSet(p->entry);
    s32 rank;
    u8 *reward;
    s32 y;
    BackgroundPanel_ReleaseHandles(p);
    TextWindow_SetScript(p->title, p->titleScript);
    TextWindow_SetBounds(p->title, 8, 3, 20, 2);
    if (GameState_IsEventFlagSet(p->entry))
        TextWindow_Refresh(p->title, p->entry);
    else
        TextWindow_Refresh(p->title, 83);
    TextWindow_StartPlayback(p->title);
    switch (p->page) {
        case 0:
            if (unlocked) {
                rank = ThresholdTrack_ClassifyPosition(p->entry);
                TextWindow_SetScript(p->rank, p->rankScript);
                TextWindow_SetBounds(p->rank, 1, 16, 30, 4);
                TextWindow_Refresh(p->rank, rank);
                TextWindow_StartPlayback(p->rank);
            }
            break;
        case 1:
            if (unlocked) {
                TextWindow_SetScript(p->count, p->countScript);
                TextWindow_SetBounds(p->count, 1, 16, 30, 4);
                TextWindow_Refresh(p->count, p->counts[p->entry]);
                TextWindow_StartPlayback(p->count);
            }
            break;
        case 2:
            if (unlocked)
                p->flags = (p->flags & ~1) | 8;
            else {
                p->flags &= ~1;
                ScrollingDialogue_ClearScript();
            }
            break;
        case 3:
            if (!unlocked)
                break;
            reward = p->rewards + p->entry * 4;
            y = 16;
            TextWindow_SetScript(p->label[0], p->labelScript);
            TextWindow_SetBounds(p->label[0], 1, y, 6, 2);
            TextWindow_Refresh(p->label[0], 0);
            TextWindow_StartPlayback(p->label[0]);
            if (reward[1] == 255) {
                TextWindow_SetScript(p->reward[0], p->emptyScript);
                TextWindow_SetBounds(p->reward[0], 6, y, 20, 2);
                TextWindow_Refresh(p->reward[0], 0);
                TextWindow_StartPlayback(p->reward[0]);
            } else if (GameState_IsPairFlagSet(p->entry, 0)) {
                if (reward[1] <= 254) {
                    switch (reward[0]) {
                        case 0:
                            TextWindow_SetScript(p->reward[0], p->rewardScript);
                            TextWindow_SetBounds(p->reward[0], 6, y, 20, 2);
                            TextWindow_Refresh(p->reward[0], reward[1]);
                            TextWindow_StartPlayback(p->reward[0]);
                            break;
                        case 1:
                            TextWindow_SetScript(p->reward[0], p->nameScript);
                            TextWindow_SetBounds(p->reward[0], 6, y, 20, 2);
                            TextWindow_Refresh(p->reward[0], reward[1]);
                            TextWindow_SetString(p->reward[0], 0, p->name);
                            TextWindow_StartPlayback(p->reward[0]);
                            break;
                        case 2:
                            TextWindow_SetScript(p->reward[0], p->itemScript);
                            TextWindow_SetBounds(p->reward[0], 6, y, 20, 2);
                            TextWindow_Refresh(p->reward[0], reward[1]);
                            TextWindow_StartPlayback(p->reward[0]);
                            break;
                    }
                }
            } else {
                TextWindow_SetScript(p->reward[0], p->titleScript);
                TextWindow_SetBounds(p->reward[0], 6, y, 20, 2);
                TextWindow_Refresh(p->reward[0], 83);
                TextWindow_StartPlayback(p->reward[0]);
            }
            y += 2;
            TextWindow_SetScript(p->label[1], p->labelScript);
            TextWindow_SetBounds(p->label[1], 1, y, 6, 2);
            TextWindow_Refresh(p->label[1], 1);
            TextWindow_StartPlayback(p->label[1]);
            if (reward[3] == 255) {
                TextWindow_SetScript(p->reward[1], p->emptyScript);
                TextWindow_SetBounds(p->reward[1], 6, y, 30, 2);
                TextWindow_Refresh(p->reward[1], 0);
                TextWindow_StartPlayback(p->reward[1]);
            } else if (GameState_IsPairFlagSet(p->entry, 1)) {
                if (reward[3] <= 254) {
                    switch (reward[2]) {
                        case 0:
                            TextWindow_SetScript(p->reward[1], p->rewardScript);
                            TextWindow_SetBounds(p->reward[1], 6, y, 30, 2);
                            TextWindow_Refresh(p->reward[1], reward[3]);
                            TextWindow_StartPlayback(p->reward[1]);
                            break;
                        case 1:
                            TextWindow_SetScript(p->reward[1], p->nameScript);
                            TextWindow_SetBounds(p->reward[1], 6, y, 30, 2);
                            TextWindow_Refresh(p->reward[1], reward[3]);
                            TextWindow_SetString(p->reward[1], 0, p->name);
                            TextWindow_StartPlayback(p->reward[1]);
                            break;
                        case 2:
                            TextWindow_SetScript(p->reward[1], p->itemScript);
                            TextWindow_SetBounds(p->reward[1], 6, y, 30, 2);
                            TextWindow_Refresh(p->reward[1], reward[3]);
                            TextWindow_StartPlayback(p->reward[1]);
                            break;
                    }
                }
            } else {
                TextWindow_SetScript(p->reward[1], p->titleScript);
                TextWindow_SetBounds(p->reward[1], 6, y, 30, 2);
                TextWindow_Refresh(p->reward[1], 83);
                TextWindow_StartPlayback(p->reward[1]);
            }
            break;
    }
}
