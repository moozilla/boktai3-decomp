#include "global.h"
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

void Sound_PlaySong(u32);
void ActorShadow_Disable(void *);
void ActorPhase_WaitBeforeRelocation(void);
void ActorPhase_UpdateSpriteCollapse(struct PhaseActor *s)
{
    u8 *data = s->data;
    if (consumeEntry(s)) {
        s->sprite->flags |= 2;
        {
            u32 mask = 4;
            *(u32 *)(data + 0xe4c) |= mask;
        }
        Sound_PlaySong(0x16b);
        {
            u32 mask = 0x1800;
            s->flags2c0 = mask | s->flags2c0;
        }
    }
    if (s->sprite->scaleX > 32) {
        s->sprite->scaleX -= 4;
    } else if (s->sprite->scaleX > 8) {
        s->sprite->scaleX -= 2;
        s->sprite->scaleY += 4;
        ActorShadow_Disable((u8 *)s + 0x320);
    } else if (s->sprite->scaleY <= 119) {
        s->sprite->scaleY += 8;
        ActorShadow_Disable((u8 *)s + 0x320);
    } else {
        void (*next)(void);
        u32 id;
        s->sprite->flags = 1;
        next = ActorPhase_WaitBeforeRelocation;
        id = 0x24;
        s->entered = 1;
        s->phaseState = 0;
        s->phaseId = id;
        s->phase = next;
        s->timer = 0;
        clearFlags(&s->flags114, 1);
        s->f104 = 0;
        ActorShadow_Disable((u8 *)s + 0x320);
        {
            u32 mask = 0x40;
            *(u32 *)(data + 0xe4c) |= mask;
        }
    }
}
