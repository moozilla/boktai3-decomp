#include "global.h"
struct Rect {
    s16 x0, z0, x1, z1;
    u8 y0, y1;
    u16 id;
};
struct Ref {
    u8 bytes[4];
    u16 first, index;
};
u8 *Script_GetPc(void);
u32 Script_GetValue(void);
struct Rect *Terrain_FindBoxesById(u16, u16 *);
void Script_ReadReference(struct Ref *);
void Script_WriteReferenceValue(struct Ref *, s32, s32);
static inline s32 half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -((-value) >> 1);
}
static inline s32 half_shift(s32 value)
{
    if (value >= 0)
        value >>= 1;
    else
        value = -((-value) >> 1);
    return value << 4;
}
s32 Terrain_ExportBoxBoundsFromScript(void)
{
    u16 count;
    struct Ref ref;
    struct Rect *r;
    s32 value, sum;
    u8 *write;
    if (!Script_GetPc())
        return -1;
    r = Terrain_FindBoxesById(Script_GetValue(), &count);
    if (!r)
        return -1;
    Script_ReadReference(&ref);
    Script_WriteReferenceValue(&ref, 0, r->x0);
    Script_ReadReference(&ref);
    Script_WriteReferenceValue(&ref, 0, r->y0 << 4);
    Script_ReadReference(&ref);
    Script_WriteReferenceValue(&ref, 0, r->z0);
    Script_ReadReference(&ref);
    Script_WriteReferenceValue(&ref, 0, r->x1);
    Script_ReadReference(&ref);
    Script_WriteReferenceValue(&ref, 0, r->y1 << 4);
    Script_ReadReference(&ref);
    Script_WriteReferenceValue(&ref, 0, r->z1);
    Script_ReadReference(&ref);
    write = (u8 *)&ref;
    sum = r->x1 + r->x0;
    value = half(sum);
    Script_WriteReferenceValue((struct Ref *)write, 0, value);
    Script_ReadReference(&ref);
    write = (u8 *)&ref;
    sum = r->y1 + r->y0;
    Script_WriteReferenceValue((struct Ref *)write, 0, half_shift(sum));
    Script_ReadReference(&ref);
    write = (u8 *)&ref;
    sum = r->z1 + r->z0;
    value = half(sum);
    Script_WriteReferenceValue((struct Ref *)write, 0, value);
    return 0;
}
