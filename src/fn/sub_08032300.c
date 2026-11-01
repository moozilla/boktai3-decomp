// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"
struct Game {
    u8 pad0[0x5A8];
    s16 form;
    u8 pad5aa[0x2BE];
    u32 flags;
};
int TextState_GetPairSpeakerLabel(void)
{
    int v = gGameState->form;
    int result;
    if (v == 1) {
        goto fallback;
    }
    if (v > 1) {
        goto fallback;
    }
    if (v != 0) {
        goto fallback;
    }
    result = 31;
    v = 1;
    if (v) {
    }
    goto done;
fallback:
    result = 32;

done:
    return result;
}
