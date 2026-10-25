#include "global.h"

struct RatatoskPosition {
    u32 xy, z;
};
struct Ratatosk {
    u8 unk_00[0x54];
    struct RatatoskPosition position;
    u8 unk_5C[0xaa - 0x5c];
    u8 animation, unk_AB[3], state, ready, active;
    u8 unk_B1[0xc4 - 0xb1];
    s32 stateTimer;
    u8 unk_C8[0x180 - 0xc8];
    s32 sequence, complete;
    u8 unk_188[0xe5a - 0x188];
    u16 repeat;
    struct RatatoskPosition destination;
    u8 unk_E64[0xe84 - 0xe64];
    u16 timer;
    u8 unk_E86[0xede - 0xe86];
    s16 waitTimer;
};
s32 SunOrb_GetManagerStatus(void);
void Ratatosk_GetPlayerOffsetFarthestFromSelf(struct Ratatosk *, struct RatatoskPosition *);
u32 Ratatosk_IsPositionOutsideArena(struct RatatoskPosition *);

static inline u8 ConsumeActivation(struct Ratatosk *p)
{
    if (p->active) {
        p->active = 0;
        p->ready = 0;
        return 1;
    }
    return 0;
}

void Ratatosk_UpdateArenaCheckedSequence(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    if (ConsumeActivation(q)) {
        if (SunOrb_GetManagerStatus() == 0) {
            Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &p->destination);
            {
                u32 animation = 4;
                q->animation = animation;
            }
            p->sequence = 0;
            p->repeat = 0;
            p->waitTimer = 0;
            p->timer = 0;
        } else {
            u32 animation = 12;
            q->animation = animation;
            p->sequence = 2;
        }
    }
    {
        s32 *sequence = &p->sequence;
        switch (*sequence) {
            case 0:
                if (q->ready) {
                    if (Ratatosk_IsPositionOutsideArena(&q->position)) {
                        {
                            u32 state = 11;
                            q->state = state;
                        }
                        q->stateTimer = 0;
                        q->active = 1;
                    } else {
                        u32 animation = 11;
                        q->animation = animation;
                        *sequence = 1;
                    }
                }
                break;
            case 1:
                if (q->ready) {
                    u32 animation = 12;
                    q->animation = animation;
                    *sequence = 2;
                }
                break;
            case 2:
                if (q->ready)
                    p->complete = 1;
                p->repeat++;
                break;
        }
    }
    p->timer++;
}
