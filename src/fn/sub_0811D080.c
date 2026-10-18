#include "global.h"
#define SceneMotion_PlayKindSound sub_0811D080
void Sound_PlaySong(u32);
struct SceneMotionSoundSource {
    u8 unknown0[8];
    u32 kind;
};
static inline u8 IsKind(u32 kind, u32 expected)
{
    return kind == expected;
}
void sub_0811D080(struct SceneMotionSoundSource *s)
{
    if (IsKind(s->kind, 4) || IsKind(s->kind, 3))
        Sound_PlaySong(0x95);
    else if (IsKind(s->kind, 5))
        Sound_PlaySong(0x13e);
    else
        Sound_PlaySong(0x91);
}
