#define PhaseObject_ActivateNextChild sub_080F3420
#include "global.h"

// Tentative family name: object with four phase entries and timed state callbacks.
struct PhaseChild {
    u8 pad0[0x29];
    u8 flags;
    u8 pad2A[4];
    u8 active, step;
    u8 pad30[0x44];
    void (*callback)(struct PhaseChild *);
    u8 pad78[0x11c];
};
struct PhaseObject {
    u8 pad0[0x5ac];
    struct PhaseChild children[3];
    u8 padA68[0xb4c - 0xa68];
    u16 interval, cooldown;
    u8 padB50[0xce4 - 0xb50];
    void (*callback)(struct PhaseObject *);
    u8 padCE8[0xd2c - 0xce8];
    u8 childCount;
};
void sub_080F5F24(struct PhaseChild *);
void sub_080F34C8(struct PhaseObject *);
void sub_080F3420(struct PhaseObject *p)
{
    s32 i;
    s32 stride;
    if (p->cooldown) {
        p->cooldown--;
        return;
    }
    goto search;
activate: {
    struct PhaseChild *child = (struct PhaseChild *)((u8 *)p + i * stride + 0x5ac);
    u32 one = 1;
    u8 *q = &child->active;
    *q = one;
    q++;
    *q = 0;
    child->flags = one | child->flags;
    p->children[i].callback = sub_080F5F24;
    p->cooldown = p->interval;
    return;
}
search:
    i = 0;
    if (i < p->childCount) {
        s32 count;
        u8 *active;
        stride = 0x194;
        count = p->childCount;
        active = &p->children[0].active;
        do {
            if (!*active)
                goto activate;
            active += stride;
            i++;
        } while (i < count);
    }
    p->callback = sub_080F34C8;
}
