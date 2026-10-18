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
void Nidhoggr_SetBehavior(void *, u32);
void SceneMessages_SetPendingTrigger(void *, void *);
s32 Nidhoggr_Update(void *);

static inline int count(struct Queue08197470 *q)
{
    u8 *bank = &gUnk_0200047C->bank;
    return q->count[*bank];
}

s32 Nidhoggr_UpdateCutscene(u8 *t)
{
    struct Queue08197470 *queue = (struct Queue08197470 *)(t + 0x18);
    int i;
    for (i = 0; i < count(queue); i++) {
        struct Globals08197470 *g = gUnk_0200047C;
        struct Msg08197470 *cmd = queue->commands[g->bank][i];
        switch (cmd->type) {
            case 0:
                Nidhoggr_SetBehavior(t, 0xd);
                break;
            case 33:
                Nidhoggr_SetBehavior(t, 0xe);
                break;
            case 34:
                Nidhoggr_SetBehavior(t, 0xf);
                break;
            case 35:
                Nidhoggr_SetBehavior(t, 0x10);
                break;
            case 36:
                Nidhoggr_SetBehavior(t, 0x12);
                break;
        }
        SceneMessages_SetPendingTrigger(queue, cmd);
    }
    return Nidhoggr_Update(t);
}
