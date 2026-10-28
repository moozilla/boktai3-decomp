#include "engine.h"
#include "runtime.h"
// fix.py best (sweep after the inlined-insn fix)
struct TrapPosition {
    s16 x, y, z;
};
struct TrapProbe {
    u16 kind;
    u8 unknown2, count;
    struct TrapPosition *position;
    u32 serials[10];
};
struct TrapRectangle {
    s16 minX, minZ, maxX, maxZ;
    u8 pad8[4];
};
extern u16 gUnk_03005260[];
void Trap_DispatchEvent(struct TrapProbe *, struct TerrainTrap *, u16);
static inline u32 Pressed(struct TerrainTrap *trap)
{
    if (trap->flags & 16) {
        u32 value = gUnk_03005260[1] & trap->f20;
        return (-value | value) >> 31;
    }
    return 1;
}
static inline s32 Contains(struct TrapPosition *position, struct TerrainTrap *trap)
{
    struct TrapRectangle *rect = trap->f28;
    s32 i = 0;
    s32 count = trap->fe;
    if (i < count) {
        s16 x = position->x;
        do {
            if (x >= rect->minX && x < rect->maxX && position->z >= rect->minZ &&
                position->z < rect->maxZ)
                return 1;
            rect++;
            i++;
        } while (i < count);
    }
    return 0;
}

static inline s32 Append(struct TrapProbe *p, s32 count, const u32 *serial, volatile s32 *index)
{
    s32 i;
    for (i = 0; i < count; i++)
        if (p->serials[i] == *serial) {
            *index = i;
            return 1;
        }
    if (count > 9)
        return 2;
    p->serials[count] = *serial;
    *index = count;
    return 0;
}
static inline s32 Remove(struct TrapProbe *p, s32 count, const u32 *serial, volatile s32 *index)
{
    s32 i;
    for (i = 0; i < count; i++) {
        if (p->serials[i] == *serial) {
            *index = i;
            {
                s32 j;
                for (j = i; j < count; j++) {
                    if (j < count - 1)
                        p->serials[j] = p->serials[j + 1];
                    else
                        p->serials[j] = 0;
                }
            }
            return 1;
        }
    }
    return 0;
}
// confidence=low: reconciles a probe's ten tracked trap serials and dispatches enter/stay/leave events.
#define Trap_UpdateProbeContacts sub_0821DE30
inline static s32 Append_0821DE30(u32 *serials, s32 count, const u32 *serial, volatile s32 *index)
{
    s32 i;
    for (i = 0; i < count; i++)
        if (serials[i] == (*serial)) {
            *index = i;
            return 1;
        }

    if (count > 9)
        return 2;
    {
        serials[count] = *serial;
    }
    *index = count;
    return 0;
}

void Trap_UpdateProbeContacts(struct TrapProbe *probe)
{
    volatile s32 index = 0;
    s32 count;
    s32 i;
    gTrapDispatchStarted = 1;
    count = probe->count;
    i = 0;
    for (; i < gTerrainMap->trapCount; i++) {
        {
            {
                struct TerrainTrap *trap = &gTerrainMap->traps[i];
                if ((trap->f6 != probe->kind) && (trap->f6 != 0x14C9))
                    goto nextTrap;
                if (Pressed(trap) && Contains(probe->position, trap)) {
                    s32 result = Append_0821DE30(probe->serials, count, &trap->serial, &index);
                    if (result == 0) {
                        if ((((trap->f4 == 0xDD2) || (trap->f4 == 0x14C9)) ||
                             (trap->f4 == 0x1516)) ||
                            (trap->f4 == 0x1517))
                            Trap_DispatchEvent(probe, trap, 0xDD2);
                        count++;
                    } else if (result == 1) {
                        if (((trap->f4 == 0xA5BF) || (trap->f4 == 0x1516)) || (trap->f4 == 0x1517))
                            Trap_DispatchEvent(probe, trap, 0xA5BF);
                    } else if (result == 2)
                        return;
                } else if (Remove(probe, count, &trap->serial, &index) == 1) {
                    count--;
                    if (((trap->f4 == 0xD5CC) || (trap->f4 == 0x14C9)) || (trap->f4 == 0x1516))
                        Trap_DispatchEvent(probe, trap, 0xD5CC);
                }
            nextTrap:
            }
        }
    }
    probe->count = count;
}
