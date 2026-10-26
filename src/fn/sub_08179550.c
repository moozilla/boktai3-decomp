#define gDisplayPalette gPaletteBankData
#include "global.h"
#include "engine.h"
struct NumericDisplayWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    s16 x, y, z;
    u8 unknown26[0x22];
    void *palette;
    u8 unknown4C[0x14];
};
struct NumericDisplay {
    u8 unknown0[0x144];
    struct NumericDisplayWidget widgets[6];
    u8 resource[0x20];
    void *script;
    u32 parameter;
};
s32 Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, void *);
s32 NumericDisplay_InitializeWidgets(struct NumericDisplay *s, u32 parameter)
{
    s32 i;
    s32 x;
    s32 result, ret;
    void *script;
    script = ScriptProc_Start(0xCB05, 0x530D);
    s->script = script;
    ResourceHeader_Relocate(s->resource, script);
    i = 0;
    do {
        switch (i) {
            case 0:
                x = 0x58;
                break;
            case 1:
                x = 0x60;
                break;
            case 2:
                x = 0x68;
                break;
            case 3:
                x = 0x78;
                break;
            case 4:
                x = 0x80;
                break;
            case 5:
                x = 0x88;
                break;
            default:
                x = 0;
                break;
        }
        result = Widget_InitLinked(&s->widgets[i], s->resource, 0x29, 0x10, 0, 0, 0, 0);
        if (result != 0) {
            ret = -1;
            goto end;
        }
        s->widgets[i].x = x;
        s->widgets[i].z = result;
        s->widgets[i].y = result;
        s->widgets[i].flags &= ~1;
        s->widgets[i].palette = gDisplayPalette;
    } while (++i <= 5);
    s->parameter = parameter;
    ret = 0;
end:
    return ret;
}
