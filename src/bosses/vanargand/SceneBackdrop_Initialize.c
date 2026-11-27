#include "global.h"
struct SceneBackdrop {
    void *resource;
    u32 unknown4;
    s16 values[3];
};
void *sub_08216E54(u16);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void Bg_SetPriority(u32, s32);
static inline void SetValues(s16 *v)
{
    s16 a = 0x200, b = 0x100;
    v[0] = a;
    v[1] = b;
    v[2] = a;
}
void SceneBackdrop_Initialize(void *unused, struct SceneBackdrop *p)
{
    s32 layer;
    void *resource = sub_08216E54(0xa96a);
    p->resource = resource;
    layer = 1;
    Bg_SetupLayers(1, 0, resource, 0, 0, 1, &layer);
    Bg_SetPriority(2, 2);
    SetValues(p->values);
}
