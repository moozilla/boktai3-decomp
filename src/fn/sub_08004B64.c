#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    union {
        u32 word;
        struct {
            u16 x, y;
        } half;
    } xy;
    union {
        u32 word;
        struct {
            u16 z, pad;
        } half;
    } zp;
};
struct Size {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};
struct Actor {
    u8 pad[16];
    u8 direction;
    u8 pad11[39];
    struct Vec position;
};
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
void sub_08068108(struct Vec *, s32, struct Size *, s32, s32, s32);
static inline u32 scatter(const u8 *random, const u16 *coordinate)
{
    s32 value = *coordinate - 128;
    return value + *random;
}
static inline u32 frontEdge(u32 random, u32 coordinate)
{
    s32 value = coordinate + 120;
    return value + (random & 15);
}
static inline u32 backEdge(u32 random, u32 coordinate)
{
    s32 value = coordinate - 120;
    return value - (random & 15);
}
/* Consume two jitter samples per particle; publish the index before emission. */
#define FIRST()                                                                                    \
    (index = (*cursor + 1) & ({                                                                    \
                 register u32 m asm("r1") = mask;                                                  \
                 asm("" : "+r"(m));                                                                \
                 m;                                                                                \
             }),                                                                                   \
     (const u16 *)(index * 2 + (u32)table))
#define LAST()                                                                                     \
    (index = (index + 1) & mask, *({                                                               \
         register u32 *p asm("r1") = cursor;                                                       \
         p;                                                                                        \
     }) = index,                                                                                   \
     index *= 2, index += (u32)table, (const u16 *)index)
void Scene_SpawnDirectionalDust(struct Actor *input)
{
    register struct Actor *actor asm("r10") = input;
    struct Size size;
    struct Vec point;
    struct Vec *particle;
    s32 i;
    register const u16 *table asm("r4");
    register u32 *cursor asm("r9");
    u32 mask;
    size.x = 32;
    size.y = 0;
    size.z = 32;
    particle = &point;
    table = gUnk_0203B400;
    cursor = &gUnk_03005308;
    i = ({
        register s32 count asm("r1");
        asm("" : "=r"(count) : "0"(2));
        count;
    });
    mask = 1023;
    do {
        register u32 index asm("r2");
        struct Actor *local = actor;

        const u16 *random;

        point = local->position;
        switch (local->direction) {
            case 0:
                random = FIRST();
                {
                    u32 value = scatter((u8 *)random, &particle->xy.half.x);
                    u32 low = (u16)value;
                    point.xy.word = (point.xy.word & 0xFFFF0000) | low;
                }
                {
                    const u16 *second = LAST();
                    {
                        u32 value = frontEdge(*second, point.zp.half.z);
                        u32 low = (u16)value;
                        point.zp.word = (point.zp.word & 0xFFFF0000) | low;
                    }
                }
                break;
            case 1:
                random = FIRST();
                {
                    u32 value = backEdge(*random, particle->xy.half.x);
                    u32 low = (u16)value;
                    point.xy.word = (point.xy.word & 0xFFFF0000) | low;
                }
                {
                    const u16 *second = LAST();
                    {
                        u32 value = scatter((u8 *)second, &point.zp.half.z);
                        u32 low = (u16)value;
                        point.zp.word = (point.zp.word & 0xFFFF0000) | low;
                    }
                }
                break;
            case 2:
                random = FIRST();
                {
                    register u32 value asm("r1") = scatter((u8 *)random, &particle->xy.half.x);
                    u32 low = (u16)value;
                    point.xy.word = (point.xy.word & 0xFFFF0000) | low;
                }
                {
                    const u16 *second = LAST();
                    {
                        u32 value = backEdge(*second, point.zp.half.z);
                        u32 low = (u16)value;
                        point.zp.word = (point.zp.word & 0xFFFF0000) | low;
                    }
                }
                break;
            case 3:
                random = FIRST();
                {
                    register u32 value asm("r1") = frontEdge(*random, particle->xy.half.x);
                    u32 low = (u16)value;
                    point.xy.word = (point.xy.word & 0xFFFF0000) | low;
                }
                {
                    const u16 *second = LAST();
                    {
                        register u32 value asm("r1") = scatter((u8 *)second, &point.zp.half.z);
                        u32 low = (u16)value;
                        point.zp.word = (point.zp.word & 0xFFFF0000) | low;
                    }
                }
                break;
        }
        sub_08068108(particle, 0, &size, 1, 2, 4);
        i = ({
            register s32 count asm("r2") = i;
            asm("" : : "r"(count));
            count - 1;
        });
    } while (i >= 0);
}
