#define SceneSequence_SelectEntryMessage sub_0816B66C
#define gSceneGameState gUnk_02000710
#include "global.h"
#include "game_state.h"
struct SceneSequenceEntry { u8 unknown0[15]; u8 message; };
struct SceneSequence {
    u8 unknown0[0x48F8];
    struct SceneSequenceEntry *entries;
    u8 unknown48FC[8];
    u8 selection;
};
s32 MessageWindow_ShowMessage(s32);
void sub_0816B66C(struct SceneSequence *s)
{
    if ((s16)gUnk_02000710->savedX > 5)
        MessageWindow_ShowMessage((s16)gUnk_02000710->savedX + 56);
    else
        MessageWindow_ShowMessage(s->entries[s->selection].message);
}
