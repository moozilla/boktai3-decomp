// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 95.8%
// Still differs: HasFlag mask/pointer order and spill store position at 08101B80, base reload at 08101BE4,
// sb/r1 reuse for the 'where' stores.
#define NamedObject_RegisterFollowCells sub_08101B1C
#include "player.h"
#include "terrain_cell_probe.h"
struct NamedObject {
    u8 pad0[0xB6];
    u16 state;
    u16 flags;
    u8 padBA[0xDC - 0xBA];
    u32 name;
    u8 padE0[0xF3 - 0xE0];
    u8 kind;
    u8 padF4[0x10C - 0xF4];
    struct Pos16 position;
    u8 pad114[0x158 - 0x114];
    u8 registration[4][0x10];
    u8 registered[4];
};
struct NamedObjectManager {
    u8 pad0[0x1370];
    u32 followName;
};
extern struct NamedObjectManager *gUnk_020001AC;
void ActorGroup_ReleaseMember(struct NamedObject *);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
s32 Div(s32, s32);
s32 Mod(s32, s32);
static inline s32 GetCell(struct Pos16 *p)
{
    s32 x, z, cell;
    x = (s16)p->x >> 8;
    z = (s16)p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        cell = 0;
    else
        cell = gUnk_030052F4->rows[z] + x;
    return cell;
}
static inline u8 HasFlag(u32 mask, u16 *flags)
{
    if (*flags & mask)
        return TRUE;
    return FALSE;
}
static inline u8 HasBits(u16 *p)
{
    u32 v = *p;
    if (!(v & 0xC0))
        return 0;
    return 1;
}
void NamedObject_RegisterFollowCells(struct NamedObject *objectArg)
{
    register struct NamedObject *object asm("r8") = objectArg;
    struct NamedObjectManager *manager = gUnk_020001AC;
    s32 base, cell, col, row;
    s32 i;
    struct Pos16 where;
    register struct Pos16 *w asm("sb");
    u16 *flags;
    u8 *registration;
    if (!manager || object->name != manager->followName || object->kind == 2 ||
        *((u8 *)gUnk_02000580[0] + 0x457) != 6 || (s16)object->state != 3) {
        ActorGroup_ReleaseMember(object);
        return;
    }
    if (HasFlag(0x800, flags = &object->flags))
        return;
    base = GetCell(&object->position);
    cell = base + 1;
    i = 0;
    registration = object->registration[0];
    for (; i < 4; i++) {
        if (HasBits((u16 *)(cellProps(cell) + 2))) {
            TerrainModifiers_RegisterCell(registration, cell, 0, (s16)object->position.y >> 8, 0xFF,
                                          5);
            object->registered[i] = 1;
            w = &where;
            col = Mod(cell, *(s16 *)(gUnk_030052F4->cells + 4));
            row = Div(cell, *(s16 *)(gUnk_030052F4->cells + 4));
            where.x = col * 256 + 0x80;
            w->y = (*cellProps(cell) & 0xF) << 8;
            w->z = row * 256 + 0x80;
            {
                s32 k2 = 0x800;
                *flags |= k2;
            }
        }
        switch (i) {
            case 0:
                cell = base + *(s16 *)(gUnk_030052F4->cells + 4);
                break;
            case 1:
                cell = base - 1;
                break;
            case 2:
                cell = base - *(s16 *)(gUnk_030052F4->cells + 4);
                break;
        }
        registration += 0x10;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08101B1C);
#endif
