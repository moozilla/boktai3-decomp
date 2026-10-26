// MATCHING_CONCESSION: register-pinning
#include "global.h"

// confidence=low: tentative cue role inferred from named callers (PlayerCue_RunTo).
struct PlayerRunTo {
    u8 pad0[0x30];
    s16 x, xf, y, yf, vx, vxf, vy, vyf;
    u8 pad1[0x28F - 0x40];
    u8 scaleAxis;
    u8 pad2[0x3A2 - 0x290];
    u8 facing, flip, dir;
    u8 pad3[0x418 - 0x3A5];
    u8 variant;
    u8 pad4[0x45A - 0x419];
    u16 frame;
    u8 pad5[0x58C - 0x45C];
    void (*fn)(struct PlayerRunTo *);
    s16 tx, txf, ty, tyf;
    u8 pad6[5];
    s8 finalDir;
    u8 pad7[0xAD6 - 0x59E];
    u16 overlayA, overlayB;
};
void Player_AddControlFlags(struct PlayerRunTo *, u32);
void PlayerCue_CompleteAction(struct PlayerRunTo *);
void PlayerCue_EnterStandUpdate(struct PlayerRunTo *);
void Player_SetAnimation(struct PlayerRunTo *, s32, s32, s32);
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
static inline s32 Mul32(s32 v)
{
    register s32 k asm("r3");
    k = 0x20;
    asm("" : "+r"(k));
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

void PlayerCue_UpdateRunTo(struct PlayerRunTo *p)
{
    s32 dx;
    s32 dy;
    s32 angle;
    s32 dist;
    if (p->frame == 0)
        Sound_PlaySong(0x298);
    dx = p->tx - p->x;
    dy = p->ty - p->y;
    angle = Math_Atan2(dx, dy);
    dist = (dx * dx) + (dy * dy);
    if (dist <= 0x400) {
        p->x = p->tx;
        p->y = p->ty;
        if (p->finalDir >= 0) {
            p->dir = p->finalDir;
            p->finalDir = -1;
        } else {
            {
                register s32 t asm("r0") = ((u16)angle) + 0x70;
                register s32 d;
                asm("" : "+r"(t));
                t >>= 5;
                d = t & 7;
                p->dir = d;
            }
        }
        if (p->dir > 4) {
            p->facing = 8 - p->dir;
            p->flip = 1;
        } else {
            p->facing = p->dir;
            p->flip = 0;
        }
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 0), 0x40, 1);
        PlayerCue_EnterStandUpdate(p);
        PlayerCue_CompleteAction(p);
        return;
    }
    Player_AddControlFlags(p, 4);
    if (p->frame <= 3) {
        p->overlayA = 0x174;
        p->overlayB = 0x18;
        Player_AddControlFlags(p, 2);
    } else if (dist <= 0x4000) {
        Player_AddControlFlags(p, 2);
    } else {
        p->overlayA = 0x174;
        p->overlayB = 0x18;
        Player_AddControlFlags(p, 1);
    }
    p->frame++;
    {
        s32 vy;
        p->vx = Truncate(Mul32(gUnk_086149C4[(angle + 64) & 255]));
        {
            register u32 scratch asm("r3");
            s32 sine;
            // Keep r3 live across the load; scratch is consumed only by empty constraints.
            asm("" : "=r"(scratch));
            sine = gUnk_086149C4[angle & 255];
            asm("" : : "r"(scratch));
            vy = Truncate(Mul32(sine));
        }
        p->vy = vy;
        {
            register u32 scratch asm("r3");
            u32 axis;
            // Keep r3 live across the load; scratch is consumed only by empty constraints.
            asm("" : "=r"(scratch));
            axis = p->scaleAxis;
            asm("" : : "r"(scratch));
            if (axis == 1) {
                if (vy < 0)
                    p->vy = Scale(p->vy, 40);
                else
                    p->vy = Scale(p->vy, 0x34);
            } else if (axis == 2) {
                if (p->vx < 0)
                    p->vx = Scale(p->vx, 40);
                else
                    p->vx = Scale(p->vx, 0x34);
            }
        }
    }
    p->dir = ((((angle + 16) & 255) >> 5) + 3) & 7;
    if (p->dir > 4) {
        p->facing = 8 - p->dir;
        p->flip = 1;
    } else {
        p->facing = p->dir;
        p->flip = 0;
    }
    Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 1), 0x20, 1);
}
