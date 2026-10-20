#include "global.h"

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
u32 SceneActorAnimation_SelectDirectionalEntry(void *, u32);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void Mummy_UpdateWalking(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        SceneActorAttachment_ReplaceResponseSlot(p, 0, 0x7F, 0);
    SceneActorAnimation_SelectDirectionalEntry(p, 1);
}
