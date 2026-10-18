#include "global.h"
#define SceneKeyframe_SetGridPosition sub_08210C14
struct SceneGridKeyframe {
    u8 unknown0[0x24];
    u8 data[0x80];
    u8 animation[0x20];
    s16 x, y;
    u8 unknownC8[0xABC];
    s16 column, row;
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
s32 sub_08210C14(struct SceneGridKeyframe *p)
{
    p->x = p->column * 40;
    p->y = p->row * 40;
    Anim_LoadKeyframe(p->animation, p->data, 0x58, 0);
    return 0;
}
