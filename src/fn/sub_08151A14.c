#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: tentative cue role inferred from named callers and callback behavior.

void Sound_PlaySong(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void Player_UpdateFacingSide(void *);
extern const s16 gUnk_086149C4[];

// confidence=low: motion seven rotates toward a target and pulses three palette colors.
extern u8 *gUnk_02000710;
extern u16 gUnk_02000518, gUnk_02000528;
static inline u32 Arg1(u32 k_, s32 green)
{
    return (green << 5) | k_;
}

static inline u32 Arg2(u32 k_, struct Player *p)
{
    return p->facing - k_;
}

void PlayerCue_UpdateTurningPalette(struct Player *p)
{
    s32 a;
    s32 b;
    s32 red;
    s32 green;
    switch (p->tetherPhase) {
        case 0:
            if (p->tetherFrame > 1) {
                s32 dir;
                if (p->targetFacing < p->facing)
                    dir = Arg2(1, p);
                else
                    dir = p->facing + 1;
                {
                    typeof(p->facing) *q1 = &p->facing;
                    *q1 = dir & 7;
                    Player_UpdateFacingSide(p);
                    p->heading--;
                    p->tetherFrame = 0;
                }
            } else
                p->tetherFrame++;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x37), 0x40, 1);
            if (p->facing == p->targetFacing) {
                if (p->facing == 1)
                    p->tetherPhase = 1;
                else if (p->facing == 3)
                    p->tetherPhase = 6;
                else
                    p->tetherPhase = 4;
            }
            break;

        case 1:
            p->heading = 0;
            p->side = 0;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x35), 0x40, 1))
                p->tetherPhase = 2;
            break;

        case 2:
            p->heading = 0;
            p->side = 0;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x36), 0x40, 1);
            break;

        case 3:
            p->heading = 0;
            p->side = 0;
            if (Player_SetAnimation(p, 0x157, 0x40, 1)) {
                Player_UpdateFacingSide(p);
                p->heading--;
                p->tetherPhase = 0;
                p->tetherFrame = 0;
            }
            break;

        case 4:
            p->heading = 0;
            p->side = 1;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x38), 0x40, 1))
                p->tetherPhase = 7;
            break;

        case 5:
            p->heading = 0;
            p->side = 1;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x39), 0x40, 1)) {
                if ((++p->cuePhase) > 1)
                    p->tetherPhase = 7;
            }
            break;

        case 6:
            p->heading = 0;
            p->side = 0;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x38), 0x40, 1))
                p->tetherPhase = 7;
            break;

        case 7:
            break;
    }

    if ((*(((u8 *)p) + 0x5A0)) <= 7) {
        a = *(((u8 *)p) + 0x5A0);
        b = 8 - a;
    } else {
        a = 16 - (*(((u8 *)p) + 0x5A0));
        b = (*(((u8 *)p) + 0x5A0)) - 8;
    }
    red = ((30 * a) + (19 * b)) >> 3;
    green = ((13 * a) + (12 * b)) >> 3;
    *((u16 *)(((u8 *)p) + 0x36A)) = Arg1(0x2C00, green) | red;
    red = ((31 * a) + (25 * b)) >> 3;
    green = ((18 * a) + (12 * b)) >> 3;
    *((u16 *)(((u8 *)p) + 0x36C)) = ((green << 5) | 0x2C00) | red;
    red = (31 * (a + b)) >> 3;
    green = ((22 * a) + (13 * b)) >> 3;
    *((u16 *)(((u8 *)p) + 0x37A)) = ((green << 5) | 0x2C00) | red;
    if ((++(*(((u8 *)p) + 0x5A0))) > 15)
        *(((u8 *)p) + 0x5A0) = 0;
    if ((*((s16 *)(gUnk_02000710 + 0x5A8))) != 2) {
        gUnk_02000518 = 0;
        gUnk_02000528 = 0;
    }
    if (gUnk_02000528) {
        u16 n = gUnk_02000518;
        if (n)
            gUnk_02000518 = n - 1;
        else
            gUnk_02000528 = n;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", PlayerCue_UpdateTurningPalette);
#endif
