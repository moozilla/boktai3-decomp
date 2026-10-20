#include "global.h"
#include "engine.h"
#define SceneNpc_UpdateStopped sub_081CEF3C
struct Npc {
    u8 pad0[6];
    u8 state, field7, pad8, field9;
    u8 padA[2];
    u16 flagsC, flagsE;
    u32 field10;
    u8 pad14[4];
    u8 field18;
    u8 pad19[0xBB];
    u8 blockD4[0x1F8];
    void *update;
    u8 pad2D0[0x14];
    void *callback;
};
s32 DisplayAnimation_TestFinished(void *);
void Actor_OnHiddenIdleUpdate(void);
void SceneNpc_UpdateStopped(void *unused, struct Npc *p)
{
    if (p->field9) {
        p->field9 = 0;
        p->field7 = 0;
        Actor_PlayAnimationEntry(p, 5, 2, 0, 4);
        {
            s32 two = 2;
            if ((p->flagsC & two) == 0)
                Sound_PlaySong(0x11F);
        }
        {
            s32 m = -3;
            p->flagsC = m & p->flagsC;
        }
    }
    p->flagsE |= 1;
    if ((u8)Callback_InvokeActor(p, p->callback) != 1 &&
        DisplayAnimation_TestFinished(p->blockD4)) {
        void (*next)(void);
        p->field7 = 1;
        next = Actor_OnHiddenIdleUpdate;
        p->state = 1;
        p->update = next;
        p->field10 = 0;
        p->field9 = 1;
        p->field18 = 0;
    }
}
