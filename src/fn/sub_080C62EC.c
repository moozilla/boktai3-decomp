#include "global.h"
#include "scene_actor_animation.h"

#define Skeleton_ClearDirectionalEntryFlag sub_080C62EC

struct Seq {
    u8 pad0[8];
    u16 h8;
    u8 padA[4];
    u16 hE;
};

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void sub_080C62EC(struct SceneActorAnimationView *s)
{
    struct Seq *seq = (struct Seq *)((u8 *)s->drawable + 0x48);

    if (TakeFlag(&s->directionalEntryFlag)) {
        SceneActorAnimation_SelectDirectionalEntry(s, 0xE);
        s->restart = 0;
    }
    if (seq->h8 == 1)
        seq->hE = 0;
}
