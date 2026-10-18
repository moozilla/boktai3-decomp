#define PhaseObject_ResetMotion Ivy_InitBobbing
#include "global.h"

// Tentative family name: object with four phase entries and timed state callbacks.
struct PhaseEntry {
    u32 flags;
    u8 pad4[0x1a];
    u16 y;
    u8 pad20[0xc];
};
struct PhaseObject {
    u8 pad0[0xa8];
    struct PhaseEntry entries[4];
    u8 pad158[0x48a - 0x158];
    u16 targetY;
    u8 pad48C[0xb6e - 0x48c];
    u8 phase;
    u8 padB6F[0xce0 - 0xb6f];
    void (*callback)(struct PhaseObject *);
    u8 padCE4[0x10];
    s32 delay;
    s32 motionTimer;
};
void Ivy_InitBobbing(struct PhaseObject *);
void sub_080F3230(struct PhaseObject *);
void Sound_PlaySong(u32);
void PhaseObject_BeginEntryMotion(struct PhaseObject *p)
{
    if (--p->delay == 0) {
        u16 *targetY = &p->targetY;
        struct PhaseEntry *entry = p->entries;
        u16 *height = &p->entries[0].y;
        struct PhaseEntry *end = p->entries + 3;
        u32 mask = ~1;
        do {
            *height = *targetY;
            entry->flags &= mask;
            entry++;
            height = (u16 *)((u8 *)height + 0x2c);
        } while ((s32)entry <= (s32)end);
        Ivy_InitBobbing(p);
        p->delay = 15;
        p->motionTimer = 20;
        p->callback = sub_080F3230;
        p->phase = 0x80;
        Sound_PlaySong(0x16c);
    }
}
