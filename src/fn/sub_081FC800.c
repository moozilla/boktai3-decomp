#include "global.h"
#include "player.h"
#include "ram.h"
struct SceneProjectileState {
    u16 unknown;
    u16 lifetime;
    u8 pad04[0x4A];
    s16 mode;
    s16 timer;
};
struct SceneProjectile {
    u32 age;
    u8 pad04[0x24];
    struct SceneProjectileState state;
    u8 pad7A[0xFC];
    s16 contactCount;
};
struct ProjectileContact {
    u8 pad00[10];
    u16 kind;
};
struct ProjectilePoolState {
    u8 pad00[0x1C];
    u32 contactFlag;
};
s32 Player_StartForcedStun(u8 *, s32);
void Sound_PlaySong(u32);
void SceneProjectile_RespondToContact(void *unused, struct ProjectileContact *contact,
                                      struct SceneProjectile *p)
{
    struct SceneProjectileState *state = (void *)((u8 *)p + 0x28);
    if (p->state.mode == 1 && contact->kind == 1)
        ((struct ProjectilePoolState *)gSceneProjectilePool)->contactFlag = 1;
    if (p->state.mode == 0) {
        if ((*(u8 **)gPlayers)[0x457] != 26 && *(u16 *)((*(u8 **)gPlayers) + 0x4D8) == 0) {
            if (Player_StartForcedStun((*(u8 **)gPlayers), 90) == 1)
                Sound_PlaySong(1400);
        }
    }
    if (p->state.mode != 2) {
        p->age = state->lifetime;
    } else if (contact->kind == 32) {
        p->contactCount++;
        if (p->contactCount <= 1 && state->timer <= -10) {
            state->timer = 120;
            p->age = 0;
        }
    } else {
        if (p->age < state->lifetime)
            p->age = state->lifetime;
    }
}
