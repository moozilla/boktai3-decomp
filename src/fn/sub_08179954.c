#include "global.h"
#include "scene_arc.h"
struct MeterDisplayWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0xE];
    u8 attribute;
    u8 unknown1B[5];
    s16 x, y, z;
    u8 unknown26[0x22];
    void *palette;
    u8 unknown4C[0x14];
};
struct MeterDisplay {
    u8 unknown0[0x5C];
    struct MeterDisplayWidget widgets[2];
    u8 resource[0x20];
    void *script;
    s32 scale;
};
s32 Widget_InitLinked(void *, void *, u16, u32, u32, u32, u32, void *);
s32 Div(s32, s32);
void MeterDisplay_InitializeWidgets(struct MeterDisplay *s)
{
    s32 i, result;
    u32 zero;
    u32 offset;
    u32 *flags;
    struct MeterDisplayWidget *widget;
    void *script;
    script = ScriptProc_Start(0xCB05, 0x1289);
    s->script = script;
    ResourceHeader_Relocate(s->resource, script);
    i = 0;
    zero = 0;
    widget = s->widgets;

    do {
        result =
            Widget_InitLinked(widget, s->resource, i + 0x49, 0x10, zero, zero, zero, (void *)zero);
        if (result)
            return;
        widget->x = result;
        widget->y = result;
        widget->z = result;
        flags = (u32 *)((u8 *)s + 0x64);
        flags = (u32 *)((u8 *)flags + i * 0x60);
        *flags &= ~1;
        widget->attribute = zero;
        widget++;

    } while (++i <= 1);
    s->scale = Div(*(s32 *)(gSceneCamera->unknown0 + 0x28) << 5, 100);
}
