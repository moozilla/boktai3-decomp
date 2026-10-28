#include "global.h"
#include "player.h"
#include "ram.h"

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
extern u8 *gGameState;
static inline u32 Arg1(u32 k_, s32 green)
{
    return (green << 5) | k_;
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
                if (p->targetFacing < p->facing)
                    p->facing = (p->facing - 1) & 7;
                else
                    p->facing = (p->facing + 1) & 7;
                {

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

    if ((p->unk59F[1]) <= 7) {
        a = p->unk59F[1];
        b = 8 - a;
    } else {
        a = 16 - (p->unk59F[1]);
        b = (p->unk59F[1]) - 8;
    }
    red = ((30 * a) + (19 * b)) >> 3;
    green = ((13 * a) + (12 * b)) >> 3;
    green = Arg1(0x2C00, green) | red;
    p->visualParameterA = green;
    red = ((31 * a) + (25 * b)) >> 3;
    green = ((18 * a) + (12 * b)) >> 3;
    green = Arg1(0x2C00, green) | red;
    p->visualParameterB = green;
    red = (31 * (a + b)) >> 3;
    green = ((22 * a) + (13 * b)) >> 3;
    green = Arg1(0x2C00, green) | red;
    p->visualParameterC = green;
    if ((++(p->unk59F[1])) > 15)
        p->unk59F[1] = 0;
    if ((*((s16 *)(gGameState + 0x5A8))) != 2) {
        gPlayerCueTimer = 0;
        gPlayerActionPending = 0;
    }
    if (gPlayerActionPending) {
        if (gPlayerCueTimer)
            gPlayerCueTimer--;
        else
            gPlayerActionPending = 0;
    }
}
