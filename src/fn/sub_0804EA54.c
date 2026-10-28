#include "global.h"
#include "game_state.h"
#include "ram.h"
struct S {
    u8 f[0x868];
    u32 fl;
    u8 g[0x08];
    s16 v;
};
struct O {
    u8 f[0x18];
    u32 q;
    u8 g[0xd];
    u8 b29;
};
extern u32 gUnk_02000580;
s32 AreaEffect_TestParticleMode(struct O *);
void AreaEffect_UpdateActionParticles(struct O *);
void AreaEffect_HideSprites(struct O *);
void AreaEffect_UpdateResourceFromPlayerFlags(struct O *);
void AreaEffect_ShowEnabledSprites(struct O *);
static inline u32 mk(u32 *a, u32 m)
{
    return *a & m;
}
static inline u8 tst(u32 v)
{
    if (v)
        return TRUE;
    return FALSE;
}
s32 AreaEffect_Tick(struct O *o)
{
    if (o->b29 != 0)
        o->b29 = 0;
    o->q = gUnk_02000580;
    if (o->q != 0) {
        struct S *s = gGameState;
        if (s->v == 0) {
            if (AreaEffect_TestParticleMode(o) != 0)
                AreaEffect_UpdateActionParticles(o);
            else
                AreaEffect_HideSprites(o);
        } else {
            if (s->fl & 2)
                AreaEffect_UpdateResourceFromPlayerFlags(o);
            else
                AreaEffect_ShowEnabledSprites(o);
        }
        {
            u32 v = gPlayerActionPending;
            u32 r = 0;
            if (v == 1)
                r = 1;
            if (r != 0 && gPlayerEffectPendingCount != 0)
                o->b29 = 1;
        }
    }
    return 0;
}
