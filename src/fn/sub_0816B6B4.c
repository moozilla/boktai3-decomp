#define gSceneGameState          gGameState
#define gSceneSequenceEntries    gUnk_08611BA8
#define gSceneSequenceSelections gUnk_08611BC0
#include "global.h"
#include "game_state.h"
struct SceneSequenceEntry {
    u8 unknown0[15];
    u8 message;
};
struct SceneSequenceSelection {
    u8 entry;
    u8 unknown1[3];
};
extern const struct SceneSequenceEntry *const gUnk_08611BA8[];
extern const struct SceneSequenceSelection *const gUnk_08611BC0[];
s32 SceneSequence_GetSelectedMessage(void)
{
    struct GameState *state = gGameState;
    s32 result;
    if ((s16)state->savedX <= 5) {
        const struct SceneSequenceEntry *entries = gUnk_08611BA8[(s16)state->savedX];
        const struct SceneSequenceSelection *selection = gUnk_08611BC0[(s16)state->savedX];
        result = entries[selection[(s16)state->savedY].entry].message;
    } else {
        result = (s16)state->savedX + 56;
    }
    return result;
}
