#include "global.h"
struct S {
    u8 a[0x78];
    u32 w78;
};
void Callback_InvokeActor(struct S *, u32);
u32 MovingGlyph_UpdateTask(struct S *p)
{
    Callback_InvokeActor(p, p->w78);
    return 0;
}
