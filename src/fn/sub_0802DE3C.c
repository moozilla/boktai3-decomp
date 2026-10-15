#include "global.h"
struct Actor {
    u8 pad0[0x5c];
    u8 motion[0x264];
    void *effect;
    u32 effectKind;
    s16 timer, duration;
};
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, void *, int, int, int);
void Sound_PlaySong(int);
void sub_0802DE3C(struct Actor *p, unsigned kind, int timer, int duration)
{
    switch (kind) {
        case 0:
            p->effectKind = 0;
            p->effect =
                ActorAttachment_UpdateFromDescriptor(p->effect, p->motion, 0, 1, 0, 0, 0, 100);
            Sound_PlaySong(0x1b1);
            break;
        case 2:
            p->effectKind = 0;
            p->effect =
                ActorAttachment_UpdateFromDescriptor(p->effect, p->motion, 0, 0, 0, 0, 0, 100);
            Sound_PlaySong(0x1ec);
            break;
        case 1:
            p->effectKind = 0;
            p->effect =
                ActorAttachment_UpdateFromDescriptor(p->effect, p->motion, 0, 3, 0, 0, 0, 100);
            Sound_PlaySong(0x1ec);
            break;
        case 3:
            p->effectKind = 0;
            p->effect =
                ActorAttachment_UpdateFromDescriptor(p->effect, p->motion, 0, 2, 0, 0, 0, 100);
            Sound_PlaySong(0x1ed);
            break;
        case 4:
            p->effectKind = 1;
            p->timer = timer;
            p->duration = duration;
            p->effect = ActorAttachment_UpdateFromDescriptor(p->effect, p->motion, 1, 0, &p->timer,
                                                             p->duration, 0, 100);
            break;
        case 5:
            p->effectKind = 3;
            p->timer = timer;
            p->duration = duration;
            p->effect = ActorAttachment_UpdateFromDescriptor(p->effect, p->motion, 3, 0, &p->timer,
                                                             p->duration, 0, 100);
            break;
        case 6:
            p->effectKind = 5;
            p->effect =
                ActorAttachment_UpdateFromDescriptor(p->effect, p->motion, 5, 0, 0, 0, 0, 100);
            break;
        case 7:
            p->effectKind = 4;
            p->effect =
                ActorAttachment_UpdateFromDescriptor(p->effect, p->motion, 4, 0, 0, 0, 0, 100);
            break;
        case 8:
            if (p->effect)
                p->effect = ActorAttachment_UpdateFromDescriptor(
                    p->effect, p->motion, (u8)p->effectKind, 127, 0, 0, 0, 100);
            break;
    }
}
