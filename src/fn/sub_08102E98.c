#include "global.h"

extern u8 *gUnk_02000580;
void NamedObject_UpdatePlayerAttachment(u8 *);
void NamedObject_UpdateTetherAttachment(u8 *);
s32 Player_TestAttachmentFlags(u8 *, u32);
void NamedObject_MoveAttachedPlayer(u8 *);
s32 TetherActor_TestStatusFlags(u32);
void NamedObject_MoveAttachedTether(u8 *);
void Memory_Clear(u8 *, u32);

void NamedObject_UpdateFollowers(u8 *s)
{
    if (gUnk_02000580 != 0) {
        NamedObject_UpdatePlayerAttachment(s);
        NamedObject_UpdateTetherAttachment(s);
        if (Player_TestAttachmentFlags(gUnk_02000580, 1))
            NamedObject_MoveAttachedPlayer(s);
        if (TetherActor_TestStatusFlags(1))
            NamedObject_MoveAttachedTether(s);
        Memory_Clear(s + 0x58, 8);
    }
}
