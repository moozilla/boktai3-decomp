#define SceneSong_EnterAndWait sub_081F8C6C
#include "global.h"
struct SceneSongState {
    u8 pad0[0xAA];
    u8 state;
    u8 padAB[4];
    u8 done, entered;
    u8 padB1[0x184 - 0xB1];
    u32 complete;
};
void Sound_PlaySong(u32);
static inline u8 TakeEntry(struct SceneSongState *p)
{
    if (p->entered) {
        p->entered = 0;
        p->done = 0;
        return TRUE;
    }
    return FALSE;
}
void sub_081F8C6C(struct SceneSongState *p)
{
    struct SceneSongState *state = p;
    if (TakeEntry(p)) {
        u32 value = 0x13;
        state->state = value;
        Sound_PlaySong(0x559);
    }
    if (state->done)
        p->complete = 1;
}
