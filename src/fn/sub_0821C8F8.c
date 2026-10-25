#include "global.h"

#include "engine.h"
struct Position {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct Sub0821C8F8Probe {
    u16 x, y, z, w;
};
u8 sub_0821C254(u8 *, u16 *, struct Sub0821C8F8Probe *, u8);
void Terrain_SampleSurface(u8 *, u16 *, struct Sub0821C8F8Probe *);
u32 sub_0821BF5C(void *, void *, u8);
s32 Mod(s32, s32);
s32 Div(s32, s32);

#define SUB0821C8F8_PROBE(p)  sub_0821C254(out, (u16 *)pos, (p), flag)
#define SUB0821C8F8_SAMPLE(p) Terrain_SampleSurface(out, (u16 *)pos, (p))
#define SUB0821C8F8_MOVE(dst, src, mx, mz)                                                         \
    do {                                                                                           \
        (dst)->x = (src)->x + (mx);                                                                \
        (dst)->y = (src)->y;                                                                       \
        (dst)->z = (src)->z + (mz);                                                                \
        (dst)->w = (src)->w;                                                                       \
    } while (0)
#define SUB0821C8F8_DIAGONAL(ux, uz)                                                               \
    do {                                                                                           \
        SUB0821C8F8_MOVE(&c, &base, (ux) * stepX, (uz) * stepZ);                                   \
        if (SUB0821C8F8_PROBE(&c)) {                                                               \
            SUB0821C8F8_SAMPLE(&base);                                                             \
            out[0] = 1;                                                                            \
            break;                                                                                 \
        }                                                                                          \
        qpos = *pos;                                                                               \
        qpos.x = pos->x + delta->x + (ux) * stepX;                                                 \
        s1 = sub_0821BF5C(pos, &qpos, flag);                                                       \
        rpos = *pos;                                                                               \
        rpos.z = pos->z + delta->z + (uz) * stepZ;                                                 \
        s2 = sub_0821BF5C(pos, &rpos, flag);                                                       \
        if (s1) {                                                                                  \
            if (s2) {                                                                              \
                SUB0821C8F8_SAMPLE(&base);                                                         \
                break;                                                                             \
            }                                                                                      \
            c = base;                                                                              \
            SUB0821C8F8_PROBE(&c);                                                                 \
            c.x = c.x + delta->x;                                                                  \
            c.z = c.z - (uz) * stepZ;                                                              \
            SUB0821C8F8_SAMPLE(&c);                                                                \
            out[0] = 1;                                                                            \
            break;                                                                                 \
        }                                                                                          \
        if (s2) {                                                                                  \
            c = base;                                                                              \
            SUB0821C8F8_PROBE(&c);                                                                 \
            c.x = c.x - (ux) * stepX;                                                              \
            c.z = c.z + delta->z;                                                                  \
            SUB0821C8F8_SAMPLE(&c);                                                                \
            out[0] = 1;                                                                            \
            break;                                                                                 \
        }                                                                                          \
        cellX = Mod(*(u16 *)(out + 6), gUnk_030052F8);                                             \
        cellZ = Div(*(u16 *)(out + 6), gUnk_030052F8);                                             \
        c.x = ((cellX + 1) << 8) - 1 - (ux) * stepX;                                               \
        c.z = ((cellZ + 1) << 8) - 1 - (uz) * stepZ;                                               \
        c.y = base.y;                                                                              \
        c.w = base.w;                                                                              \
        SUB0821C8F8_SAMPLE(&c);                                                                    \
        out[0] = 1;                                                                                \
    } while (0)

#ifdef NONMATCHING
// fuzzy: 20.3%
// scope shape 1228 (differences once low registers are set aside)
// confidence=low: moves an actor position by its delta, probing terrain cells in the delta's sign quadrant.
void sub_0821C8F8(void *arg0, struct Position *arg1, struct Position *arg2, u32 arg3, u32 arg4,
                  u32 arg5)
{
    u8 *out = arg0;
    struct Position *pos = arg1;
    struct Position *delta = arg2;
    s32 stepX = (u16)arg3;
    s32 stepZ = (u16)arg4;
    u8 flag = (u8)arg5;
    s32 mode = 0;
    s32 tx, tz;
    s32 cell;
    s32 cellX, cellZ;
    u32 s1, s2;
    struct Sub0821C8F8Probe base, c;
    struct Position qpos, rpos;

    if (delta->x > 0)
        mode = 0x10;
    else if (delta->x < 0)
        mode = 0x20;
    if (delta->z > 0)
        mode |= 1;
    else if (delta->z < 0)
        mode |= 2;

    tx = (s8)(pos->x >> 8);
    tz = (s8)(pos->z >> 8);
    if (tx >= 0 && tz >= 0 && tx < gUnk_030052F8 && tz < gUnk_030052FC)
        cell = gUnk_030052F4->rows[tz] + tx;
    else
        cell = 0;
    *(u16 *)(out + 6) = cell;

    base.x = pos->x + delta->x;
    base.y = pos->y + delta->y;
    base.z = pos->z + delta->z;
    base.w = 0x10;
    out[0] = 0;

    switch (mode) {
        case 0x00:
            SUB0821C8F8_SAMPLE(&base);
            break;
        case 0x01:
            SUB0821C8F8_MOVE(&c, &base, stepX, stepZ);
            if (SUB0821C8F8_PROBE(&c)) {
                SUB0821C8F8_SAMPLE(&base);
                out[0] = 1;
                break;
            }
            SUB0821C8F8_MOVE(&c, &base, -stepX, stepZ);
            if (SUB0821C8F8_PROBE(&c)) {
                SUB0821C8F8_SAMPLE(&base);
                out[0] = 1;
                break;
            }
            SUB0821C8F8_SAMPLE(&base);
            break;
        case 0x02:
            SUB0821C8F8_MOVE(&c, &base, stepX, -stepZ);
            if (SUB0821C8F8_PROBE(&c)) {
                SUB0821C8F8_SAMPLE(&base);
                out[0] = 1;
                break;
            }
            SUB0821C8F8_MOVE(&c, &base, -stepX, -stepZ);
            if (SUB0821C8F8_PROBE(&c)) {
                SUB0821C8F8_SAMPLE(&base);
                out[0] = 1;
                break;
            }
            SUB0821C8F8_SAMPLE(&base);
            break;
        case 0x10:
            SUB0821C8F8_MOVE(&c, &base, stepX, stepZ);
            if (SUB0821C8F8_PROBE(&c)) {
                SUB0821C8F8_SAMPLE(&base);
                out[0] = 1;
                break;
            }
            SUB0821C8F8_MOVE(&c, &base, stepX, -stepZ);
            if (SUB0821C8F8_PROBE(&c)) {
                SUB0821C8F8_SAMPLE(&base);
                out[0] = 1;
                break;
            }
            SUB0821C8F8_SAMPLE(&base);
            break;
        case 0x20:
            SUB0821C8F8_MOVE(&c, &base, -stepX, stepZ);
            if (SUB0821C8F8_PROBE(&c)) {
                SUB0821C8F8_SAMPLE(&base);
                out[0] = 1;
                break;
            }
            SUB0821C8F8_MOVE(&c, &base, -stepX, -stepZ);
            if (SUB0821C8F8_PROBE(&c)) {
                SUB0821C8F8_SAMPLE(&base);
                out[0] = 1;
                break;
            }
            SUB0821C8F8_SAMPLE(&base);
            break;
        case 0x11:
            SUB0821C8F8_DIAGONAL(1, 1);
            break;
        case 0x21:
            SUB0821C8F8_DIAGONAL(-1, 1);
            break;
        case 0x12:
            SUB0821C8F8_DIAGONAL(1, -1);
            break;
        case 0x22:
            SUB0821C8F8_DIAGONAL(-1, -1);
            break;
        default:
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0821C8F8);
#endif
