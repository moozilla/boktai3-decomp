#include "global.h"

struct Msg08197470 {
    u8 pad0[6];
    u8 type;
};
struct Queue08197470 {
    u8 pad[6], count[2];
    struct Msg08197470 *commands[2][4];
};
extern struct Globals08197470 {
    u8 pad[36], bank;
} *gUnk_0200047C;
void sub_08195A4C(void *, u32);
void sub_08020D54(void *, void *);
s32 sub_0819736C(void *);

static inline int count(struct Queue08197470 *q)
{
    u8 *bank = &gUnk_0200047C->bank;
    return q->count[*bank];
}

s32 sub_08197470(u8 *t)
{
    struct Queue08197470 *queue = (struct Queue08197470 *)(t + 0x18);
    int i;
    for (i = 0; i < count(queue); i++) {
        struct Globals08197470 *g = gUnk_0200047C;
        struct Msg08197470 *cmd = queue->commands[g->bank][i];
        switch (cmd->type) {
        case 0:
            sub_08195A4C(t, 0xd);
            break;
        case 33:
            sub_08195A4C(t, 0xe);
            break;
        case 34:
            sub_08195A4C(t, 0xf);
            break;
        case 35:
            sub_08195A4C(t, 0x10);
            break;
        case 36:
            sub_08195A4C(t, 0x12);
            break;
        }
        sub_08020D54(queue, cmd);
    }
    return sub_0819736C(t);
}
