#include "global.h"
#include "player.h"
struct PhaseSprite {
    u32 flags;
    u8 pad4[2];
    u8 field6, pad7;
    s8 scaleX, scaleY;
};
struct PhaseCollision {
    u8 pad0[6];
    u16 flags;
};
struct PhaseActor {
    u8 pad0[4];
    struct PhaseSprite *sprite;
    u8 pad8[0xfc];
    u32 f104;
    u8 pad108[0xc];
    u32 flags114;
    u8 pad118[0x1c];
    struct PhaseCollision collision134;
    u8 pad13c[0x4c];
    struct PhaseCollision collision188;
    u8 pad190[0x8c];
    void (*phase)(void);
    u8 pad220[0x90];
    u8 phaseState;
    u8 pad2b1;
    u8 entered;
    u8 pad2b3[2];
    u8 phaseId;
    u8 pad2b6[0xa];
    u16 flags2c0;
    u8 pad2c2[6];
    u16 timer;
    u8 pad2ca[0x106];
    u8 *data;
};
static inline u8 consumeEntry(struct PhaseActor *s)
{
    if (s->entered) {
        s->entered = 0;
        return 1;
    }
    return 0;
}
static inline void clearFlags(u32 *p, u32 mask)
{
    *p &= ~mask;
}

void ActorShadow_Disable(void *);
void ActorPhase_SelectDistantWaypoint(struct PhaseActor *, void *, void *, s32);
void ActorPhase_UpdateSpriteRestore(void);
void ActorPhase_WaitBeforeRelocation(struct PhaseActor *s, s32 frame)
{
    u8 *data = s->data;
    if (consumeEntry(s)) {
        s->sprite->flags &= ~2;
        {
            struct PhaseCollision *collision = &s->collision188;
            collision->flags |= 4;
        }
        {
            struct PhaseCollision *collision = &s->collision134;
            collision->flags |= 4;
        }
    }
    s->sprite->flags |= 1;
    ActorShadow_Disable((u8 *)s + 0x320);
    if (frame > data[0xe40]) {
        ActorPhase_SelectDistantWaypoint(s, (u8 *)s + 0x5c, (*(u8 **)gPlayers) + 0x30, 0x40000);
        {
            void (*next)(void) = ActorPhase_UpdateSpriteRestore;
            u32 id = 0x25;
            s->entered = 1;
            s->phaseState = 0;
            s->phaseId = id;
            s->phase = next;
            s->timer = 0;
            clearFlags(&s->flags114, 1);
            s->f104 = 0;
        }
    }
}
