#include "global.h"

#define WoodenCrate_TestFlag      WoodenCrate_HasFlags
#define WoodenCrate_SetFlag       WoodenCrate_SetFlags
#define WoodenCrate_StartSequence WoodenCrate_BeginToggleAnimation
struct WoodenCrate {
    u8 pad0[0xA4];
    u16 timer;
    u8 padA6[0xBC - 0xA6];
    u32 script;
    u8 padC0[0xCD - 0xC0];
    u8 state;
    u8 padCE[7];
    u8 shakeTimer;
};
struct CrateHit {
    u8 pad0[0x34];
    u32 flags;
};
u8 WoodenCrate_HasFlags(struct WoodenCrate *, s32);
void WoodenCrate_SetFlags(struct WoodenCrate *, s32);
void WoodenCrate_BeginToggleAnimation(struct WoodenCrate *);
void Sound_PlaySong(u32);
void Script_Call(u32, void *);
static inline u32 hitFlags(struct CrateHit *hit, u32 mask)
{
    return hit->flags & mask;
}
static inline void WoodenCrate_Shake(struct WoodenCrate *p)
{
    u8 *shake = &p->shakeTimer;
    if (!*shake)
        Sound_PlaySong(0x14D);
    *shake = 10;
}
void WoodenCrate_ReactToHit(struct CrateHit *hit, void *unused, struct WoodenCrate *p)
{
    struct WoodenCrate *crate = p;
    if (WoodenCrate_HasFlags(p, 16))
        WoodenCrate_Shake(p);
    else if (!hitFlags(hit, 0x17))
        WoodenCrate_Shake(p);
    else if (!p->timer) {
        p->state = 3;
        p->timer = 24;
        if (p->script)
            Script_Call(p->script, 0);
        WoodenCrate_BeginToggleAnimation(crate);
        if (WoodenCrate_HasFlags(crate, 8))
            WoodenCrate_SetFlags(crate, 16);
    }
}
