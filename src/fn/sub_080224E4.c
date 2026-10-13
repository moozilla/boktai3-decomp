// MATCHING_CONCESSION: register-pinning
#include "global.h"
typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct {
        u32 x, y;
    } pos;
    u8 pad5c[0x4c];
    u8 facing, facing8, padAA[2], mode, state, phase, finished, enter;
    u8 padB1[0x13];
    u32 elapsed;
    u8 padC8[4];
    u8 requestedAngle;
    u8 padCD[0x2df];
    u32 graphicsMode;
} ActorAnimation;
static inline void setFacing(ActorAnimation *p, int angle)
{
    u32 value;
    register int shifted asm("r0");
    p->angle = angle;
    shifted = p->angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySongChecked(int);
ActorAnimation *sub_0802140C(int);
void sub_080224E4(u32 *position)
{
    ActorAnimation *p = sub_0802140C(0);
    if (p) {
        int sequence;
        register u32 one asm("r3");
        u32 zero = 0, phase = 13;
        p->mode = zero;
        p->state = zero;
        p->phase = phase;
        p->elapsed = zero;
        {
            u8 *dst = &p->enter;
            one = 1;
            *dst = one;
        }
        p->pos = *(typeof(p->pos) *)position;
        setFacing(p, p->requestedAngle);
        {
            int test = p->graphicsMode & one;
            sequence = 62;
            if (test)
                sequence = 24;
        }
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, one,
                                              one, 6);
        Sound_PlaySongChecked(839);
        Sound_PlaySongChecked(1335);
    }
}
