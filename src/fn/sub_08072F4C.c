#include "global.h"
#include "scene_actor_contacts.h"
#include "scene_actor_registry.h"
#include "engine.h"

/* Terrain cell record: the modifier override (+4) if one exists, else the map's cell entry. */

/* Cell lies in the direction `dir` (low nibble) and carries no blocking override bits. */

/* Half of the signed value a, rounded toward zero. */

/* Map cell index for a position, or 0 when the position is off the map. */

/* Contact probe of one candidate position; inlined twice in the ROM (looped copy and single copy). */

// confidence=low: scene actor contact search; reports a contact distance through arg2 and returns whether one was found.

/* Terrain cell record: the modifier override (+4) if one exists, else the map's cell entry. */

/* Cell lies in the direction `dir` (low nibble) and carries no blocking override bits. */

/* Half of the signed value a, rounded toward zero. */

/* Map cell index for a position, or 0 when the position is off the map. */

/* Contact probe of one candidate position; inlined twice in the ROM (looped copy and single copy). */

// confidence=low: scene actor contact search; reports a contact distance through arg2 and returns whether one was found.
void SceneActorContacts_SetDistanceGate(void *, void *, s32, s32, s32, s32);
s32 SceneActorContacts_TestPlayerCondition(void *, s32, s32);
s32 SceneActorTerrain_IsFacingPosition(void *, void *);
void TetherActor_CopySampledPosition(void *);
s32 sub_08076CDC(void *, void *, void *);
s32 sub_0821D4F0(void *, void *, s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Div(s32, s32);
s32 Math_Atan2(s32, s32);
s32 Nav_IsPositionInMesh(void *, void *, void *);
void *memcpy(void *, const void *, u32);
extern u32 gUnk_02000560[];

/* Terrain cell record: the modifier override (+4) if one exists, else the map's cell entry. */
static inline u8 *Sub08072F4C_Cell(s32 cell)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        return data + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}
/* Cell lies in the direction `dir` (low nibble) and carries no blocking override bits. */
static inline s32 Sub08072F4C_Walkable(s32 cell, s32 dir)
{
    u8 *rec = Sub08072F4C_Cell(cell);
    if ((rec[0] & 15) != dir)
        return 0;
    rec = Sub08072F4C_Cell(cell);
    return (*(u16 *)(rec + 2) & 0x43) == 0;
}
/* Half of the signed value a, rounded toward zero. */
static inline s32 Sub08072F4C_Half(s32 a)
{
    if (a < 0)
        return -((-a) >> 1);
    return a >> 1;
}
/* Map cell index for a position, or 0 when the position is off the map. */
static inline s32 Sub08072F4C_CellAt(struct ContactPosition *p)
{
    s32 cx = (s8)(p->x >> 8);
    s32 cz = (s8)(p->z >> 8);
    if (cx < 0 || cz < 0)
        return 0;
    if (cx >= gTerrainWidth || cz >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[cz] + cx;
}

/* Contact probe of one candidate position; inlined twice in the ROM (looped copy and single copy). */
static inline s32 Sub08072F4C_Probe(struct SceneActorRegistryActor *self, void *obj,
                                    struct ContactPosition *pos, s32 kind, s32 *out)
{
    u8 *a1 = (u8 *)self;
    u8 *g = a1 + 0xDC;
    s16 dv[3];
    s32 dist, angle, r4, sb, rc, cell, dir, width, cur, res;

    dv[0] = pos->x - self->position.x;
    dv[1] = pos->y - self->position.y;
    dv[2] = pos->z - self->position.z;
    dist = dv[0] * dv[0] + dv[2] * dv[2];
    if (SceneActorTerrain_IsFacingPosition(self, pos)) {
        *out = dist;
        SceneActorContacts_SetDistanceGate(self, g, dist, 0, 0, kind);
        return 1;
    }
    angle = Math_Atan2(dv[0], dv[2]) - a1[0x59];
    if (angle > 0x80)
        angle -= 0x100;
    else if (angle < -0x80)
        angle += 0x200;
    if (angle < 0)
        angle = -angle;
    if (angle > a1[0xEA])
        return 0;

    if (kind == 2) {
        if (gEngineFlags & 0x4800) {
            if (!SceneActorContacts_TestPlayerCondition(obj, 1, 0x10000))
                goto plain;
            r4 = Sub08072F4C_Half(*(s32 *)(a1 + 0xF8));
            if (g[0xF] & 4)
                sb = r4;
            else
                sb = Div(r4 * 7, 10);
        } else {
            if (SceneActorContacts_TestPlayerCondition(obj, 1, 0x10000) ||
                !(*(u32 *)((u8 *)gGameState + 0x868) & 0x20)) {
                r4 = Sub08072F4C_Half(*(s32 *)(a1 + 0xF8));
                if (g[0xF] & 4)
                    sb = r4;
                else
                    sb = Div(r4 * 7, 10);
            } else {
                goto plain;
            }
        }
    } else {
        if (gEngineFlags & 0x4800)
            return 0;
        if (!(*(u32 *)((u8 *)gGameState + 0x868) & 0x20)) {
            r4 = Sub08072F4C_Half(*(s32 *)(a1 + 0xF8));
            if (g[0xF] & 4)
                sb = r4;
            else
                sb = Div(r4 * 7, 10);
        } else {
            goto plain;
        }
    }
    goto gate;
plain:
    r4 = *(s32 *)(a1 + 0xF8);
    sb = *(s32 *)(a1 + 0xF4);
gate:
    if (dist > r4)
        return 0;
    rc = sub_08076CDC(self, a1 + 0x5C, pos);
    if (rc < 0)
        return 0;
    cell = Sub08072F4C_CellAt(pos);
    cur = *(u16 *)(a1 + 0x2F4);
    dir = (s8)(*(u16 *)(a1 + 0x5E) >> 8);
    width = *(s16 *)(gTerrainMap->cells + 4);
    res = (cell - 1 == cur && Sub08072F4C_Walkable(cell, dir)) ||
          (cell == cur + width && Sub08072F4C_Walkable(cell, dir)) ||
          (cell + 1 == cur && Sub08072F4C_Walkable(cell, dir)) ||
          (cell == cur - width && Sub08072F4C_Walkable(cell, dir));
    if (res) {
        if (!(g[0xF] & 4)) {
            if (!Nav_IsPositionInMesh(*(void **)(a1 + 0x5D0), a1 + 0x5C, pos))
                return 0;
        }
        *out = dist;
        SceneActorContacts_SetDistanceGate(self, g, dist, 0, 0, kind);
        return 1;
    }
    if (rc != 2) {
        void *src;
        if (g[0xF] & 0x20)
            src = a1 + 0xE0;
        else
            src = a1 + 0x5C;
        if (!sub_0821D4F0(src, pos, 10, 0x100))
            return 0;
    }
    if (dist >= *out)
        return 0;
    *out = dist;
    SceneActorContacts_SetDistanceGate(self, g, dist, sb, rc, kind);
    return 1;
}

#ifdef NONMATCHING
// fuzzy: 27.2%
// scope shape 863 (differences once low registers are set aside)
// confidence=low: scene actor contact search; reports a contact distance through arg2 and returns whether one was found.
u8 sub_08072F4C(struct SceneActorContactNode *arg0, struct SceneActorRegistryActor *arg1, s32 *arg2)
{
    u8 snap[0x48];
    u8 *a1 = (u8 *)arg1;
    u8 *g;
    u8 *obj;
    s32 kind;
    s32 tries;
    s32 idx;

    kind = arg0->kind;
    if (kind == 2)
        goto kind2;
    if (kind != 1 && kind != 4)
        return 0;
    memcpy(snap, arg0->object, 0x48);
    tries = 0;
    do {
        if (Sub08072F4C_Probe(arg1, snap, (struct ContactPosition *)(snap + 0xC), arg0->kind,
                              arg2) &&
            Nav_IsPositionInMesh(*(void **)(a1 + 0x5D0), a1 + 0x5C, snap + 0xC))
            return 1;
        TetherActor_CopySampledPosition(snap + 0xC);
        tries++;
    } while (tries <= 1);
    return 0;

kind2:
    g = a1 + 0xDC;
    if (!(g[0xF] & 2))
        return 0;
    obj = arg0->object;
    if (gEngineFlags & 0x4800) {
        u32 t;
        if (obj == 0)
            return 0;
        t = obj[8];
        idx = -1;
        if (t <= 3)
            idx = t;
        if (idx < 0)
            return 0;
        if (gUnk_02000560[idx] & 8)
            return 0;
    }
    if (SceneActorContacts_TestPlayerCondition(obj, 1, 0x21000))
        return 0;
    obj = arg0->object;
    kind = arg0->kind;
    if (Sub08072F4C_Probe(arg1, obj, (struct ContactPosition *)(obj + 0xC), kind, arg2) &&
        Nav_IsPositionInMesh(*(void **)(a1 + 0x5D0), a1 + 0x5C, obj + 0xC))
        return 1;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08072F4C);
#endif
