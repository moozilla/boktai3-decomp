#include "global.h"
extern u8 *gUnk_02000488;
void TetherActor_SetState(u8 *, u32);
void TetherActor_UpdatePendingEffect(u8 *);
void SceneActorContacts_RemoveNode(u8 *);
void TetherActor_BeginDetachedState(u32 a)
{
    u8 *p = gUnk_02000488;
    if (p) {
        u8 *q;
        if (a)
            TetherActor_SetState(p, 7);
        else
            TetherActor_SetState(p, 6);
        q = p + 0x254;
        if (*q) {
            SceneActorContacts_RemoveNode(p + 0x244);
            *q = 0;
        }
        *(u16 *)(p + 0x380) = 0;
        TetherActor_UpdatePendingEffect(p);
    }
}
