#include "global.h"
#include "game_state.h"
#ifdef NONMATCHING
// fuzzy: 81.4%
// foundry draft (worker best 81.4%); what differs is mostly register allocation and instruction order
#define GameRecords_UpdateMinimum sub_0811D5A8
#define gGameState gUnk_02000710
static inline s32 ClampRecord(s32 value) {
    s32 result = value;
    if (value > 60000) result = 60000;
    else if (value < 0) result = 0;
    return result;
}
s32 sub_0811D5A8(u32 index, s32 value) {
    s32 *record;
    if (index > 11 || value < 0) goto failed;
    value = ClampRecord(value);
    { struct GameState *state = gUnk_02000710;
      state = (void *)(index * 4 + (u32)state);
      record = &state->unk7C8[0]; }
    if (*record == -1 || *record > value) goto update;
failed:
    return -1;
update:
    *record = value;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811D5A8);
#endif
