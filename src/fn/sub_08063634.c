#include "global.h"

struct C {
    u8 f[8];
    s16 v;
};

void Sound_PlaySong(s32);
void Trigger_ConsumePending(void *, s32);

void sub_08063634(void *a, void *b, struct C *c)
{
    Sound_PlaySong(c->v);
    Trigger_ConsumePending(b, 1);
}
