#include "global.h"
struct SceneSelectionKeyframe {
    u8 unknown0[0x24];
    u8 data[0x108];
    u32 flags;
    u8 unknown130[0x54];
    u8 animation[0x9DC];
    s16 selection;
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
u32 ForgingBook_IsCategoryComplete(struct SceneSelectionKeyframe *, s32);
s32 SceneKeyframe_UpdateSelectionVisibility(struct SceneSelectionKeyframe *p)
{
    Anim_LoadKeyframe(p->animation, p->data, (u16)(p->selection + 0x4C), 0);
    if (ForgingBook_IsCategoryComplete(p, p->selection))
        p->flags &= ~1;
    else
        p->flags |= 1;
    return 0;
}
