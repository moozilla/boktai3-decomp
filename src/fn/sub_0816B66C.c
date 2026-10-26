#define gSceneGameState gGameState
#include "global.h"
#include "game_state.h"
struct SceneSequenceEntry {
    u8 unknown0[15];
    u8 message;
};
struct SceneSequence {
    u8 unknown0[0x48F8];
    struct SceneSequenceEntry *entries;
    u8 unknown48FC[8];
    u8 selection;
};
s32 MessageWindow_ShowMessage(s32);
void SceneSequence_SelectEntryMessage(struct SceneSequence *s)
{
    if ((s16)gGameState->savedX > 5)
        MessageWindow_ShowMessage((s16)gGameState->savedX + 56);
    else
        MessageWindow_ShowMessage(s->entries[s->selection].message);
}
