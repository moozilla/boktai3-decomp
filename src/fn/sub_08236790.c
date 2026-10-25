#include "global.h"
#include "scene_panel_rig.h"

struct GlowOwner {
    u8 pad0[0x1A];
    u8 flag1A;
};
struct GlowTask {
    u32 flags;
    u8 pad4[0x18];
    s16 x;
    s16 y;
    u8 pad20[0x28];
    u32 flags48;
    u8 pad4C[0x18];
    u32 savedPosition[2];
    u8 pad6C[0x24];
    u8 f90;
    u8 f91;
    u8 f92, f93, f94;
    s8 f95;
    u16 timer;
    void (*callback)(void);
    struct GlowOwner *owner;
};
void RadialBurstEffect_EmitOrbitParticle(struct GlowOwner *, void *);
void sub_08235DB0(struct GlowOwner *);
void RadialBurstEffect_StartBurst(struct GlowOwner *, void *);
void TimedSprite_UpdateFrames(void);
void Sound_PlaySong(u32);

void sub_08236790(struct GlowTask *p)
{
    p->flags &= ~1;
    if (++p->timer <= 0x2F) {
        p->y += p->f95;
        p->f95--;
        if (p->owner->flag1A != 0 && p->f95 > 0)
            RadialBurstEffect_EmitOrbitParticle(p->owner, &p->x);
    } else if (p->owner->flag1A != 0) {
        p->flags |= 1;
        p->f90 = 0;
    } else {
        Sound_PlaySong(0x165);
        ScreenTransition_SendCommand(0, 4, p->f92, p->f93, p->f94, 0x1FFF, 0);
        sub_08235DB0(p->owner);
        p->f91 = 1;
        p->flags |= 1;
        p->flags48 &= ~1;
        {
            u32 a = ((u32 *)&p->x)[0];
            u32 b = ((u32 *)&p->x)[1];
            p->savedPosition[0] = a;
            p->savedPosition[1] = b;
        }
        p->callback = TimedSprite_UpdateFrames;
        p->timer = 0;
        RadialBurstEffect_StartBurst(p->owner, &p->x);
    }
}
