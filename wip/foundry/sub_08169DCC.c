#define SceneHud_RestoreOptionControls sub_08169DCC
#define SceneHud_UpdateOptionControls sub_081699BC
#define SceneHud_BeginControlPhase sub_08163EA0
#define gSceneHud gUnk_02000214
#include "global.h"
#include "scene_hud_display.h"
extern struct SceneHudState *gUnk_02000214;
void sub_081680F4(struct SceneHudState *);
void sub_08168850(struct SceneHudState *);
void sub_081686FC(struct SceneHudState *);
void sub_08168A0C(struct SceneHudState *);
void sub_08168784(struct SceneHudState *);
void sub_08168214(struct SceneHudState *);
u32 sub_0803975C(void);
void sub_08164480(struct SceneHudState *, s32, s32, s32, s32, s32, s32);
void sub_08163EA0(struct SceneHudState *, void (*)(struct SceneHudState *));
void sub_081699BC(struct SceneHudState *);
void sub_08169DCC(void)
{
    struct SceneHudState *p = gUnk_02000214;
    if (p) {
        sub_081680F4(p);
        sub_08168850(p);
        sub_081686FC(p);
        sub_08168A0C(p);
        sub_08168784(p);
        sub_08168214(p);
        p->auxiliaryToggle = 0;
        if (sub_0803975C()) {
            struct SceneHudControlAnimation *anim = &p->controlAnimation;
            anim->flags &= ~1;
            Anim_LoadKeyframe(anim, p->controlAnimationResource, 0x1C, 1);
        }
        {
        u32 maximum = *(u16 *)((*(u8 **)((u8 *)p + 0xA40)) + 0x42A);
        *(u16 *)((*(u8 **)((u8 *)p + 0xA40)) + 0x428) = maximum;
        }
        sub_08164480(p, 0, 4, 2, *(u16 *)((*(u8 **)((u8 *)p + 0xA40)) + 0x428), *(u16 *)((*(u8 **)((u8 *)p + 0xA40)) + 0x42A), 1);
        Sound_PlaySong(0x10C);
        p->pulseFrame = 0;
        sub_08163EA0(p, sub_081699BC);
    }
}
