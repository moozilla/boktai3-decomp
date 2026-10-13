#include "palette_controller.h"
struct PaletteController *sub_08219FBC(u32, u32);
void sub_0821A04C(struct PaletteController *, s32 (*)(struct PaletteController *), u32 (*)(void));
s32 Palette_InitializeFromScript(struct PaletteController *);
void sub_0821A0C0(struct PaletteController *);
s32 Palette_Update(struct PaletteController *);
u32 sub_080031E0(void);
struct PaletteController *sub_080033FC(void)
{
    struct PaletteController *r;
    if (gUnk_02000468 != 0)
        return gUnk_02000468;
    r = sub_08219FBC(11, 0xe94);
    if (r) {
        sub_0821A04C(r, Palette_Update, sub_080031E0);
        if (Palette_InitializeFromScript(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
