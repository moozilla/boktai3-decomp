#include "global.h"
#include "display_animation.h"
#define SceneSequence_UpdateCountdown sub_08204E5C
struct SceneCountdownSequence {
    u8 unknown0[0x108];
    s32 (*update)(struct SceneCountdownSequence *);
    struct DisplaySequenceState sequence;
    u8 unknown11C[0x10];
    s16 countdown;
    u8 unknown12E[4];
    u8 variant;
    u8 unknown133[0x25];
    void *resource;
};
void sub_08204C3C(struct SceneCountdownSequence *);
s32 sub_08204D84(struct SceneCountdownSequence *);
void Anim_RestartSequence(struct DisplaySequenceState *, void *, u32, u32, u32);
s32 sub_08204E5C(struct SceneCountdownSequence *p)
{
    s16 *count = &p->countdown;
    if (*count > 0) {
        if (*count == 40)
            sub_08204C3C(p);
        --*count;
    } else {
        Anim_RestartSequence(&p->sequence, p->resource, 2, p->variant, 0);
        p->update = sub_08204D84;
    }
    return 0;
}
