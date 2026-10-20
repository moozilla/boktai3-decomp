#include "global.h"
/* TentacleBody_UpdateReactionTransition: plays the hurt entry or, after a long wait, restores the saved snapshot */
extern void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
extern void Actor_RestoreSnapshot(void);
static inline u8 take(u8 *s)
{
    if (s[0x2B2]) {
        s[0x2B2] = 0;
        return 1;
    }
    return 0;
}
void TentacleBody_UpdateReactionTransition(u8 *s, s32 arg)
{
    u32 queued = take(s);
    if (queued || arg == 0x1E || arg == 0x37) {
        SceneActorAnimation_SelectDirectionalEntry(s, 0x12);
    } else if (arg > 0x5F) {
        void (*fn)(void);
        u32 code;
        u32 *flags;
        u32 mask;
        fn = Actor_RestoreSnapshot;
        code = 0xD;
        s[0x2B2] = 1;
        s[0x2B0] = queued;
        s[0x2B5] = code;
        *(void (**)(void))(s + 0x21C) = fn;
        *(u16 *)(s + 0x2C8) = queued;
        flags = (u32 *)(s + 0x114);
        mask = ~1;
        *flags &= mask;
        *(u32 *)(s + 0x104) = queued;
    }
}
