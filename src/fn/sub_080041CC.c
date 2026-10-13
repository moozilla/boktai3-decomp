#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define Scene_SpawnEdgeParticles sub_080041CC
/* Eight-byte position; the helpers below access its halfword coordinates. */
struct Vec {
    u16 x, y;
    u32 z;
};
struct Size {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 unused : 16;
};
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
void sub_08068108(struct Vec *, s32, struct Size *, s32, s32, s32);
static inline void scatter(u16 *coordinate, const u8 *random)
{
    s32 value = *coordinate - 128;
    register u32 noise asm("r2") = *random;
    {
        u32 result = value + noise;
        asm("" : "+r"(result));
        *coordinate = result;
    }
}
static inline u32 edgeJitter(const u16 *random)
{
    u32 value = *random;
    value &= 15;
    return value;
}
static inline const u16 *scatterAddress(u32 index, const u16 *table)
{
    register u32 address asm("r2") = index * 2;
    address += (u32)table;
    asm("" : "+r"(address));
    return (const u16 *)address;
}
void Scene_SpawnEdgeParticles(struct Vec *position, s32 offset)
{
    struct Size size;
    struct Vec point;
    u16 *p;
    s32 i;
    size.x = 32;
    size.y = 0;
    size.z = 32;
    for (i = 0; i < 10; i++) {
        u32 seed;
        register const u16 *table asm("r3") = gUnk_0203B400;
        p = (u16 *)&point;
        /* The two jitter samples are consumed before the next external call. */
        seed = (gUnk_03005308 + 1) & 1023;
        gUnk_03005308 = seed;
        point = *position;
        if (!(i & 1)) {
            u32 index = (seed + 1) & 1023;
            const u16 *random = (const u16 *)(index * 2 + (u32)table);
            p[0] += offset + edgeJitter(random);
            index = (index + 1) & 1023;
            gUnk_03005308 = index;
            random = scatterAddress(index, table);
            scatter(&p[2], (u8 *)random);
        } else {
            register u32 index asm("r1") = (seed + 1) & 1023;
            const u16 *random;
            random = scatterAddress(index, table);
            scatter(&p[0], (u8 *)random);
            index = (index + 1) & 1023;
            gUnk_03005308 = index;
            random = (const u16 *)(index * 2 + (u32)table);
            p[2] += offset + edgeJitter(random);
        }
        sub_08068108(&point, 0, &size, 1, 3, 4);
    }
}
