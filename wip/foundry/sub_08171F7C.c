#define SceneHud_InitializeActionPanel sub_08171F7C
#define SceneHud_ClearWidgetFlags      sub_08165B94
#include "global.h"
#include "scene_hud_display.h"
s32 Anim_LoadKeyframe(void *anim, void *data, u32 index, u32 mode);
void sub_080335B4(void);
void sub_08163EB8(void *, void (*)(void), int);
void sub_08163F14(void *, u32, int, int, u32);
void sub_08165594(s32 a, s32 b, s32 c, s32 d, s32 e);
void sub_0816568C(void);
void sub_08165ACC(void *);
void sub_08165B94(void *p, s32 i, u32 m);
void sub_08166094(void *, int, int);
void sub_0816616C(void *);
void sub_0816618C(void *, int, int);
void sub_081663A0(void *, int, int, int, int);
void sub_08166534(void *, s32, s32);
void sub_08166B7C(void *, int);
void sub_08166CD0(void *);
void sub_08166ED4(void *, int, int, int, int);
void sub_08166F84(void *, u32, u32, u32, u32);
void sub_081672FC(void *);
void sub_08170C08(void *p);
void sub_08170D78(void *);

void sub_08171524(void);
void sub_08170C90(void *);
void sub_08171F7C(struct SceneHudState *p)
{
    struct SceneInputWidget *w;
    void *resource;
    sub_0816616C(p);
    sub_08163F14(p, p->panelResource, 0, 8, p->panelConfig);
    sub_08165ACC(p);
    sub_0816568C();
    w = &p->modeWidget;
    w->flags &= ~1;
    resource = p->widgetResource;
    Anim_LoadKeyframe(w, resource, 0x56, 0);
    sub_08165B94(p, 0x4D, 1);
    sub_0816618C(p, 8, 0x20);
    sub_08165594(8, 4, 8, 3, 0);
    sub_08166ED4(p, 8, 3, 0, 0);
    sub_08166F84(p, 8, 4, 0, 0);
    sub_08166CD0(p);
    sub_08166534(p, 10, 6);
    sub_081672FC(p);
    w = &p->categoryWidgets[8];
    w->flags &= ~1;
    Anim_LoadKeyframe(w, resource, 0x66, 0);
    w = &p->categoryWidgets[9];
    w->flags &= ~1;
    Anim_LoadKeyframe(w, resource, 0x70, 0);
    sub_08170C08(p);
    sub_08170C90(p);
    sub_08170D78(p);
    sub_08166094(p, 0, 0);
    sub_08166B7C(p, 3);
    sub_08163EB8(p, sub_08171524, 0);
    sub_081663A0(p, 0, 1, 0, 0);
    sub_080335B4();
}
