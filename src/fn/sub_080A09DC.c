#include "scene_actor_script.h"
#include "engine.h"

#ifdef NONMATCHING
// fuzzy: 92.9%
// Remaining differences: register allocation around the ground-height lookup.
#define Crow_ReadSpawnScript sub_080A09DC
#define FIELD(p, o, T)       (*(T *)((u8 *)(p) + (o)))
struct Pair8 {
    u32 a, b;
};
static inline s32 ReadOrZero(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
static inline u32 CellAt(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline s32 GroundHeight(u32 cell, struct ContactPosition *position)
{
    u8 *override = TerrainModifiers_FindCellOverride(cell, 1);
    u8 *data;
    u32 type;
    s32 height;
    if (override)
        data = override + 4;
    else
        data = gUnk_030052F4->cells + 12 + cell * 4;
    type = data[0] >> 4;
    height = (data[0] & 15) << 8;
    {
        u32 slope;
        if (type == 1)
            goto alongZ;
        if (type == 2)
            goto alongX;
        goto finished;
    alongZ:
        slope = (u8)position->z;
        goto subtract;
    alongX:
        slope = (u8)position->x;
    subtract:
        height -= slope;
    }
finished:
    return height;
}
void Crow_ReadSpawnScript(void *input)
{
    struct SceneActorScriptPositionView *actor = input;
    void *s = input;
    if (Script_SeekToKeyword(0x50)) {
        u32 value;
        u32 distance;
        u32 offset;
        {
            u32 *radius = &FIELD(s, 0x3E0, u32);
            *radius = ReadOrZero();
        }
        FIELD(s, 0x3DC, u8) = ReadOrZero();
        distance = ReadOrZero();
        FIELD(s, 0x3D9, u8) = distance;
        if ((u8)distance <= 0x19)
            FIELD(s, 0x3D9, u8) = 0x1A;
        gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
        value = *(const u16 *)(gUnk_03005308 * 2 + (u32)gUnk_0203B400);
        value &= 0x3F;
        if (Script_GetPc())
            offset = Script_GetValue() + value;
        else
            offset = value;
        FIELD(s, 0x3DA, u16) = offset;
        if (Script_GetPc() && Script_GetValue()) {
            u32 mask = 4;
            u32 *flags = &FIELD(s, 0x3D4, u32);
            *flags |= mask;
        }
        {
            u32 *radius = &FIELD(s, 0x3E0, u32);
            *radius = *radius * *radius;
        }
    }
    FIELD(actor, 0x59, u8) = Script_GetKeywordValue(0x64, 0);
    if (Script_SeekToKeyword(0x70)) {
        u16 *height;
        u16 *cx, *cz;
        u32 offset;
        struct Pair8 *copy;
        actor->position.x = ReadOrZero();
        actor->position.y = ReadOrZero();
        {
            u8 *pc = Script_GetPc();
            s32 value;
            height = &actor->position.y;
            if (pc)
                value = Script_GetValue();
            else
                value = 0;
            actor->position.z = value;
        }
        copy = &FIELD(s, 0x434, struct Pair8);
        *copy = *(struct Pair8 *)&actor->position;
        asm("" : "+r"(copy));
        cx = (u16 *)copy;
        cz = cx + 2;
        actor->cellIndex = CellAt((s16)*cx >> 8, (s16)*cz >> 8);
        offset = 0x434;
        asm("" : "+r"(offset));
        actor->groundHeight = GroundHeight(actor->cellIndex, (void *)((u8 *)s + offset));
        {
            u16 y = *height;
            actor->heightOffset = y;
            FIELD(s, 0x3EA, u16) = y;
        }
        *height = actor->groundHeight;
        {
            u16 cell = actor->cellIndex;
            FIELD(s, 0x3E8, u16) = cell;
        }
        FIELD(s, 0x3DE, u16) = actor->groundHeight;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080A09DC);
#endif
