#include "global.h"
#include "render.h"
struct In {
    u8 pad[8];
    s16 f8;
    s16 fa;
};
struct Q {
    u32 f0;
    struct In *f4;
};
s32 Script_GetKeywordValue(u32, u32);
struct Q *ResourceRegistry_GetResource(u32);
void Video_SetRenderCallbacks(u32, void (*)(void), void (*)(void), void (*)(void));
void SpriteOam_QueueProjectedSprites(void);
void Sprite_RenderScaledList(void);
void sub_08219624(void);
void SpriteOam_QueueCameraRelativeSprites(void);
void sub_08214D20(void);
void sub_0821964C(void);
void Terrain_SetRenderModeFromScript(void)
{
    s32 r = Script_GetKeywordValue(0x76, 0);
    struct Q *p;
    if (r == 0) {
        p = ResourceRegistry_GetResource(0x56C2);
        Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                                 sub_08219624);
        if (p && p->f4) {
            gCamera.offsetX = p->f4->f8 >> 4;
            gCamera.offsetY = p->f4->fa >> 4;
            return;
        }
    } else {
        Video_SetRenderCallbacks(r, SpriteOam_QueueCameraRelativeSprites, sub_08214D20,
                                 sub_0821964C);
    }
    gCamera.offsetX = 0;
    gCamera.offsetY = 0;
}
