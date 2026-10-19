#include "global.h"
#include "contact_actor.h"

#define gIdx (*(u32 *)0x03005308)
#define gTable ((u16 *)0x0203B400)

struct Pt { s32 x:16; s32 y:16; s32 z:16; s32 w:16; };
struct Inner { u8 pad[0xC]; struct ContactPosition pos; };
struct Outer { u8 pad0[0x48]; struct Inner in; u8 pad1[0x58 - 0x48 - sizeof(struct Inner) + 0x50]; u8 f; };

s32 SceneParticle_CreateParticles(const struct ContactPosition *anchor,
                                  const struct ContactPosition *offset,
                                  const struct ContactPosition *range,
                                  const struct ContactPosition *jitter, s32 count, u32 priority);

void sub_08022694(u8 *p)
{
    struct Pt base;
    struct Pt off;
    struct Inner *in = (struct Inner *)(p + 0x48);
    s32 i;

    if (p[0xA8] <= 1) {
        base.x = 0x40;
        base.y = 200;
        base.z = -0x80;
    } else {
        base.x = -0x80;
        base.y = 200;
        base.z = 0x40;
    }
    i = 3;
    do {
        u32 idx = (gIdx + 1) & 0x3FF;
        off.x = base.x + (gTable[idx] & 0x7F) - 0x40;
        off.y = base.y;
        idx = (idx + 1) & 0x3FF;
        gIdx = idx;
        off.z = base.z + (gTable[idx] & 0x7F) - 0x40;
        SceneParticle_CreateParticles(&in->pos, (struct ContactPosition *)&off, 0, 0, 1, 1);
        i--;
    } while (i >= 0);
}
