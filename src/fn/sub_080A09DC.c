// MATCHING_CONCESSION: register-pinning
#include "scene_actor_script.h"
#include "engine.h"

#ifdef NONMATCHING
// fuzzy: 96.0%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: ground-height lookup temporaries (r0/r1/r3 swap) and a derived pool constant.
#define FIELD(p, o, T) (*(T *)((u8 *)(p) + (o)))
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
    register u32 type asm("r2");
    s32 height;
    if (override)
        data = override + 4;
    else
        data = gUnk_030052F4->cells + 12 + cell * 4;
    type = data[0] >> 4;
    height = data[0] & 15;
    height <<= 8;
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
inline static s32 GroundHeight_ReadSpawnScript(s16 cell, struct ContactPosition *position)
{
    u8 *override = TerrainModifiers_FindCellOverride(cell, 1);
    u8 *data;
    register u32 type asm("r2");
    s16 height;
    if (override)
        data = override + 4;
    else
        data = (gUnk_030052F4->cells + 12) + (cell * 4);
    type = data[0] >> 4;
    height = data[0] & 15;
    height <<= 8;
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
            u32 *radius = &(*((u32 *)(((u8 *)s) + 0x3E0)));
            *radius = ReadOrZero();
        }
        *((u8 *)(((u8 *)s) + 0x3DC)) = ReadOrZero();
        distance = ReadOrZero();
        *((u8 *)(((u8 *)s) + 0x3D9)) = distance;
        if (((u8)distance) <= 0x19)
            *((u8 *)(((u8 *)s) + 0x3D9)) = 0x1A;
        gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
        value = *((const u16 *)((gUnk_03005308 * 2) + ((u32)gUnk_0203B400)));
        value &= 0x3F;
        if (Script_GetPc())
            offset = Script_GetValue() + value;
        else
            offset = value;
        *((u16 *)(((u8 *)s) + 0x3DA)) = offset;
        if (Script_GetPc() && Script_GetValue()) {
            u32 mask = 4;
            u32 *flags = &(*((u32 *)(((u8 *)s) + 0x3D4)));
            *flags |= mask;
        }
        {
            u32 *radius = &(*((u32 *)(((u8 *)s) + 0x3E0)));
            *radius = (*radius) * (*radius);
        }
    }
    *((u8 *)(((u8 *)actor) + 0x59)) = Script_GetKeywordValue(0x64, 0);
    if (Script_SeekToKeyword(0x70)) {
        u16 *height;
        u16 *cx;
        u16 *cz;
        register u32 offset asm("r3");
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
        copy = &(*((struct Pair8 *)(((u8 *)s) + 0x434)));
        *copy = *((struct Pair8 *)(&actor->position));
        asm("" : "+r"(copy));
        cx = (u16 *)copy;
        cz = cx + 2;
        actor->cellIndex = CellAt(((s16)(*cx)) >> 8, ((s16)(*cz)) >> 8);
        {
            u32 cell = actor->cellIndex;
            s32 gh;
            offset = 0x434;
            asm("" : "+r"(offset));
            gh = GroundHeight_ReadSpawnScript(cell, (void *)(((u8 *)s) + offset));
            actor->groundHeight = gh;
        }
        {
            u16 y = *height;
            actor->heightOffset = y;
            *((u16 *)(((u8 *)s) + 0x3EA)) = y;
        }
        *height = actor->groundHeight;
        {
            u16 cell = actor->cellIndex;
            *((u16 *)(((u8 *)s) + 0x3E8)) = cell;
        }
        {
            u16 gh = actor->groundHeight;
            *((u16 *)(((u8 *)s) + 0x3DE)) = gh;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Crow_ReadSpawnScript);
#endif
