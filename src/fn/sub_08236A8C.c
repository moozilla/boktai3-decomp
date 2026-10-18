#include "global.h"
#define ParticleEffect_Initialize sub_08236A8C
/* Tentative family name; owner and variant are retained by the task. */
struct ParticleEffect {
    u8 pad[0x18];
    u8 variant;
    u8 pending;
    u8 stopFlags;
    u8 stopped;
    void *owner;
};
void sub_082368DC(struct ParticleEffect *, u32);
void sub_082366A8(struct ParticleEffect *, u32);
void sub_082364A4(struct ParticleEffect *);
void Sound_PlaySong(u32);
s32 sub_08236A8C(struct ParticleEffect *p, void *owner, u32 a, u32 b, u32 variant, u32 flags)
{
    p->variant = variant;
    p->owner = owner;
    sub_082368DC(p, b);
    sub_082366A8(p, a);
    sub_082364A4(p);
    if (variant == 0)
        Sound_PlaySong(0x39C);
    else
        Sound_PlaySong(0x39D);
    p->stopFlags = flags;
    return 0;
}
