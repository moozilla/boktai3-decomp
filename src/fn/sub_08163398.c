#include "global.h"
struct SceneTransitionEntry {
    u32 flags;
    u8 pad04[0x4C];
};
struct SceneTransition {
    u8 pad0[0x18];
    struct SceneTransitionEntry entries[8];
    u8 pad298[0xA8];
    u8 state;
    u8 pad341[5];
    u16 timer;
};
void sub_08218508(void);
void sub_08214E3C(void);
void sub_08219674(void);
void Video_SetRenderCallbacks(int, void *, void *, void *);
void SceneTransition_FinishDelay(struct SceneTransition *p)
{
    if (++p->timer > 31) {
        void *a = sub_08218508;
        void *b = sub_08214E3C;
        void *c = sub_08219674;
        u32 mask = ~1;
        struct SceneTransitionEntry *entry = p->entries;
        int i = 7;
        do {
            entry->flags &= mask;
            entry++;
        } while (--i >= 0);
        Video_SetRenderCallbacks(0, a, b, c);
        p->state = 2;
    }
}
