#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
struct ActorParticipantProgress {
    u8 unknown0[0x3D8];
    u8 interval;
    u8 limit;
    u16 accumulated;
    u8 unknown3DC[0xA];
    u8 elapsed, start;
};
s32 Mod(s32, s32);
void SceneActorAnimation_SelectProfileEntry(void *, s32);
void PlayerContact_ApplySyntheticContact(void *, u32, u32, u32, u32, u32);
void Player_QueueStatusEffect(void *, u32);
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
        s->start = (*(u8 **)gPlayers)[0x4F4];
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
            PlayerContact_ApplySyntheticContact((*(u8 **)gPlayers), first, second, 0, value, last);
        } else {
            {
                u32 increment = a->fieldB4;
                u16 *counter = &s->accumulated;
                *counter = *counter + increment;
            }
            if (s->accumulated > 100) {
                if (*mode == 1)
                    Player_QueueStatusEffect((*(u8 **)gPlayers), 1);
                else
                    Player_QueueStatusEffect((*(u8 **)gPlayers), 2);
                s->accumulated = 0;
            }
        }
    }
    {
        s32 start = s->start;
        s32 now = (*(u8 **)gPlayers)[0x4F4];
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
