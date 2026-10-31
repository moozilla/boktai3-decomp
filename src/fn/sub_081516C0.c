#include "player_cue.h"

// confidence=low: tentative cue role inferred from named callers (PlayerCue_RunTo).

void Player_AddControlFlags(struct Player *, u32);
void PlayerCue_CompleteAction(struct Player *);
void PlayerCue_EnterStandUpdate(struct Player *);
void Player_SetAnimation(struct Player *, s32, s32, s32);
s32 ActorVariant_GetTableValue(s32, s32);
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);

static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline s32 Multiply(s32 v, s32 k)
{
    return v * k;
}
static inline s32 Scale(s32 v, s32 k)
{
    s32 product;
    if (k == 40)
        product = ((v << 2) + v) << 3;
    else
        product = v * k;
    if (product >= 0)
        return product >> 6;
    return -((-product) >> 6);
}

void PlayerCue_UpdateRunTo(struct Player *p)
{
    s32 dx;
    s32 dy;
    s32 angle;
    s32 dist;
    s32 speed = 32;
    if (p->tetherFrame == 0)
        Sound_PlaySong(0x298);
    dx = PlayerCue_Motion(p)->destination.x - p->position.x;
    dy = PlayerCue_Motion(p)->destination.z - p->position.z;
    angle = Math_Atan2(dx, dy);
    dist = (dx * dx) + (dy * dy);
    if (dist <= 0x400) {
        p->position.x = PlayerCue_Motion(p)->destination.x;
        p->position.z = PlayerCue_Motion(p)->destination.z;
        if (p->targetFacing >= 0) {
            p->facing = p->targetFacing;
            p->targetFacing = -1;
        } else {
            {
                s32 t = ((u16)angle) + 0x70;
                u32 mask;
                t >>= 5;
                mask = 7;
                mask &= t;
                p->facing = mask;
            }
        }
        if (p->facing > 4) {
            p->heading = 8 - p->facing;
            p->side = 1;
        } else {
            p->heading = p->facing;
            p->side = 0;
        }
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40, 1);
        PlayerCue_EnterStandUpdate(p);
        PlayerCue_CompleteAction(p);
        return;
    }
    Player_AddControlFlags(p, 4);
    if (p->tetherFrame <= 3) {
        p->actionEighteenParameter = 0x174;
        p->actionEighteenDuration = 0x18;
        Player_AddControlFlags(p, 2);
    } else if (dist <= 0x4000) {
        Player_AddControlFlags(p, 2);
    } else {
        p->actionEighteenParameter = 0x174;
        p->actionEighteenDuration = 0x18;
        Player_AddControlFlags(p, 1);
    }
    p->tetherFrame++;
    {
        s32 vy;
        p->velocityX = Truncate(Multiply(gUnk_086149C4[(angle + 64) & 255], speed));
        {
            s32 sine;

            sine = gUnk_086149C4[angle & 255];

            vy = Truncate(Multiply(sine, speed));
        }
        p->velocityZ = vy;
        {
            u32 axis;

            axis = p->velocityScaleAxis;

            if (axis == 1) {
                if (vy < 0)
                    p->velocityZ = Scale(p->velocityZ, 40);
                else
                    p->velocityZ = Scale(p->velocityZ, 0x34);
            } else if (axis == 2) {
                if (p->velocityX < 0)
                    p->velocityX = Scale(p->velocityX, 40);
                else
                    p->velocityX = Scale(p->velocityX, 0x34);
            }
        }
    }
    p->facing = ((((angle + 16) & 255) >> 5) + 3) & 7;
    if (p->facing > 4) {
        p->heading = 8 - p->facing;
        p->side = 1;
    } else {
        p->heading = p->facing;
        p->side = 0;
    }
    Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 1), 0x20, 1);
}
