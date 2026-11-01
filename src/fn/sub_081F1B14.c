#include "global.h"
#include "scene_pool.h"

// MATCHING_CONCESSION: register-pinning

struct SingletonSlot7Actor {
    u8 pad0[0x90];
    u32 callbackTimer;
    u8 pad94[8];
    u32 effectFlags;
    u8 padA0[2];
    u8 flagA2, padA3;
    u32 valueA4;
    u8 padA8[9];
    u8 callbackEntered, padB2, motionMode;
    u8 padB4[0x100 - 0xB4];
    void (*callback)(void);
    u8 pad104[0x2A0 - 0x104];
    u32 scrollX;
    s32 position[3];
    u32 unknown2B0;
    s32 velocity;
    u8 pad2B8[0x3A0 - 0x2B8];
    s32 player;
};
struct SingletonSlot7Player {
    u8 pad0[0x8F4];
    u16 flags;
};
struct SingletonSlot7Scroll {
    u32 a, b, c, d;
};
/* Declaration aliases support the checker's ELF from before name promotion. */
s32 Player_GetHorizontalVelocity(s32) __asm__("Player_GetHorizontalVelocity");
void Player_GetWorldPosition(s32, s32 *) __asm__("Player_GetWorldPosition");
void SceneTilemap_CopyScrollSourceState(struct SingletonSlot7Scroll *) __asm__(
    "SceneTilemap_CopyScrollSourceState");
void SingletonSlot7_ResetDisplayState(struct SingletonSlot7Actor *) __asm__(
    "SingletonSlot7_ResetDisplayState");
void SingletonSlot7_DispatchUpdateCallback(struct SingletonSlot7Actor *) __asm__(
    "SingletonSlot7_DispatchUpdateCallback");
void SceneActors_TickStateCallback(struct SingletonSlot7Actor *) __asm__(
    "SceneActors_TickStateCallback");
void SceneActors_UpdateTargetMetrics(struct SingletonSlot7Actor *);
void SceneActors_UpdateEffects(struct SingletonSlot7Actor *);
void Sound_StopSong(u32);
void SceneActors_UpdateIdleDisplay(void);
void SingletonSlot7_UpdatePresentation(struct SingletonSlot7Actor *);

static inline u8 PlayerHasFlag(void)
{
    u32 flags;
    if ((*(struct SingletonSlot7Player **)gParticipantActors)) {
        flags = (*(struct SingletonSlot7Player **)gParticipantActors)->flags;
        if (flags & 1)
            return 1;
    }
    return 0;
}

s32 SingletonSlot7_Update(void *arg)
{
    struct SingletonSlot7Actor *actor = arg;
    register struct SingletonSlot7Actor *p asm("r5") = actor;
    struct SingletonSlot7Scroll scroll[2];
    if (actor->motionMode == 0) {
        u32 flagged = PlayerHasFlag();
        if (flagged == 1 && p->callback != SceneActors_UpdateIdleDisplay) {
            p->callback = SceneActors_UpdateIdleDisplay;
            p->callbackTimer = 0;
            p->callbackEntered = flagged;
            p->flagA2 = 0;
            p->valueA4 = 0;
            Sound_StopSong(0x590);
        }
        {
            s32 *pl = &actor->player;
            Player_GetWorldPosition(*pl, actor->position);
            actor->velocity = Player_GetHorizontalVelocity(*pl);
        }
        SceneTilemap_CopyScrollSourceState(&scroll[1]);
        actor->scrollX = scroll[1].a;
    }
    SceneActors_UpdateTargetMetrics(p);
    SingletonSlot7_ResetDisplayState(actor);
    SingletonSlot7_DispatchUpdateCallback(actor);
    SceneActors_TickStateCallback(actor);
    if (p->motionMode != 0) {
        u32 mask = 0x40;
        u32 *fp = &p->effectFlags;
        *fp |= mask;
    }
    SceneActors_UpdateEffects(p);
    SingletonSlot7_UpdatePresentation(actor);
    return 0;
}
