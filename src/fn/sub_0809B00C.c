#define gPlayerSlots                          gUnk_02000580
#include "global.h"
#include "scene_actor_reset.h"
struct ActorParticipantProgress {
    u8 unknown0[0x3D8];
    u8 interval;
    u8 limit;
    u16 accumulated;
    u8 unknown3DC[0xA];
    u8 elapsed, start;
};
extern u8 *gUnk_02000580;
s32 Mod(s32, s32);
void SceneActorAnimation_SelectProfileEntry(void *, s32);
void PlayerContact_ApplySyntheticContact(void *, u32, u32, u32, u32, u32);
void sub_0813D548(void *, u32);
static inline s32 WrapCounter(s32 n)
{
    return n - 256;
}
static inline u8 TakePrepared(struct SceneActorRegistryResetView *a)
{
    if (a->field2B2) {
        a->field2B2 = 0;
        return 1;
    }
    return 0;
}
void ActorBehavior_TickParticipantProgress(struct SceneActorRegistryResetView *a)
{
    struct ActorParticipantProgress *s = a->context;
    s32 delta;
    u8 *elapsed;
    if (TakePrepared(a)) {
        s->start = gUnk_02000580[0x4F4];
        s->accumulated = 0;
    }
    SceneActorAnimation_SelectProfileEntry(a, 14);
    if (Mod(a->field2CA, s->interval) == 0) {
        u16 *mode = &a->paletteOffset;
        u32 value = *mode;
        if (!value) {
            u32 first = a->fieldB4;
            u32 second = a->fieldBC;
            u32 last = a->fieldCC;
            PlayerContact_ApplySyntheticContact(gUnk_02000580, first, second, 0, value, last);
        } else {
            {
                u32 increment = a->fieldB4;
                u16 *counter = &s->accumulated;
                *counter = *counter + increment;
            }
            if (s->accumulated > 100) {
                if (*mode == 1)
                    sub_0813D548(gUnk_02000580, 1);
                else
                    sub_0813D548(gUnk_02000580, 2);
                s->accumulated = 0;
            }
        }
    }
    {
        s32 start = s->start;
        s32 now = gUnk_02000580[0x4F4];
        if (now < start) {
            s32 wrapped = WrapCounter(start);
            delta = now - wrapped;
        } else
            delta = now - start;
    }
    elapsed = &s->elapsed;
    if (*elapsed != delta) {
        if (Mod(*elapsed, 4) == 0) {
            u32 mask = 0x20;
            u16 *flags = &a->flags2D6;
            mask |= *flags;
            *flags = mask;
        }
        *elapsed = delta;
    }
    if (s->elapsed > s->limit)
        a->field2B0 = 1;
}
