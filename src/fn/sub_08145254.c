#include "global.h"
#include "player.h"
#include "player_slot_status.h"

// confidence=low: interpolates from the saved origin to a scripted endpoint.
#define Player_UpdateInterpolatedMotion sub_08145254
s32 Player_ConsumeStateEntry(void *);
void Player_AddControlFlags(void *, u32);
s32 Div(s32, s32);
s32 ActorVariant_GetTableValue(u32, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
static inline s32 Scale8(s32 value)
{
    if (value >= 0)
        return value >> 8;
    return -((-value) >> 8);
}
static inline s32 ScaleStep(s32 value)
{
    s32 result = Scale8(value);
    return result;
}
static inline void SetMotionSize(struct Player *p)
{
    p->actionEighteenParameter = 500;
}
void Player_UpdateInterpolatedMotion(struct Player *p)
{
    if (Player_ConsumeStateEntry(p))
        p->movementStart = p->position;
    SetMotionSize(p);
    p->actionEighteenDuration = 32;
    Player_SetStatusBits(&p->statusFlags, 0x1100);
    Player_AddControlFlags(p, 8);
    {
        u16 *duration = &p->movementDuration;
        u16 *elapsed = &p->tetherFrame;
        u32 durationValue = *duration;
        if (durationValue <= *elapsed) {
            p->position = p->movementDestination;
        } else {
            s32 dx = (p->movementDestination.x - p->movementStart.x) << 8;
            s32 dy = (p->movementDestination.y - p->movementStart.y) << 8;
            s32 dz = (p->movementDestination.z - p->movementStart.z) << 8;
            s32 zstep;

            if (!durationValue)
                *duration = 1;
            dx = Div(dx, *duration);

            dy = Div(dy, *duration);

            dz = Div(dz, *duration);

            dx = ScaleStep(dx * *elapsed);

            dy = ScaleStep(dy * p->tetherFrame);

            zstep = Scale8(dz * p->tetherFrame);
            p->position.x = p->movementStart.x + dx;
            p->position.y = p->movementStart.y + dy;
            p->position.z = p->movementStart.z + zstep;
        }
    }
    Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 49), 64, 2);
    ++p->tetherFrame;
}
