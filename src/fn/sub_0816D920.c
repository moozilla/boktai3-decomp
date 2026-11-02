#include "scene_vec.h"

static inline void SetIconPosition(struct SceneVec *out, s32 x, s32 y)
{
    out->x = x;
    out->y = y;
    out->z = 0;
    out->pad = 0;
}
void SceneHud_GetActionIconPosition(struct SceneVec *out, s32 index)
{

    if (index <= 15) {
        s32 col = index & 3;
        s32 row = index >> 2;
        SetIconPosition(out, col * 24 + 0x70, row * 24 + 0x20);
        return;
    }
    if (index == 0x27)
        SetIconPosition(out, 0xD8, 0x50);
    else if (index == 0x26)
        SetIconPosition(out, 0xD8, 0x38);
    else if (index == 0x24 || index == 0x28)
        SetIconPosition(out, 0xD8, 0x68);
    else if (index == 0x20)
        SetIconPosition(out, 0xD8, 0x20);
    else {
        out->x = 0x90;
        out->y = 0x20;
    }
}
