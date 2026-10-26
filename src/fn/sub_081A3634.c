#include "global.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
struct XZ {
    u32 x, z;
};
struct ShotE {
    u32 flags;
    u8 f4[0x1c - 4];
    struct XZ pos;
    u8 f24[0x80 - 0x24];
    s16 v[3];
    u8 f86[2];
    u8 flag88;
    u8 angle;
    u16 param;
    u16 speed;
    u16 f8e;
};
struct ShotG {
    u8 f[0x3c];
    u32 n;
};
struct ShotE *Hresvelgr_FindInactiveShot(void);
void EffectVariant_SetProperties(struct ShotE *, u32);

static inline s32 shift12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -(-v >> 12);
}
void DirectionBurst_EmitConfigured(const struct XZ *origin, u32 angle, u32 speed, u32 a3, s32 a4,
                                   u32 a5, u32 param, u32 a7)
{
    struct ShotG *g = gDirectionBurstEmitter;
    struct ShotE *e = Hresvelgr_FindInactiveShot();
    if (e) {
        g->n++;
        e->pos = *origin;
        e->speed = speed;
        if (a4 <= 0)
            a7 = 0;
        {
            s16 *v = e->v;
            s32 sp = e->speed;
            {
                s32 t0;
                t0 = shift12(gUnk_086149C4[(angle + 0x40) & 0xff] * sp);
                {
                    s32 z = 0;
                    v[0] = t0;
                    v[1] = z;
                }
            }
            {
                s32 t2;
                s32 z1;
                t2 = shift12(gUnk_086149C4[angle & 0xff] * sp);
                z1 = 0;
                v[2] = t2;
                e->flag88 = a7;
                e->f8e = z1;
            }
        }
        EffectVariant_SetProperties(e, angle);
        e->angle = angle;
        e->param = param;
        e->flags &= ~1;
    }
}
