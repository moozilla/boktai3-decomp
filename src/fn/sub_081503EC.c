// confidence=low: tentative cue role inferred from named callers and callback behavior.
#include "global.h"
#include "player.h"
// confidence=low: cue callbacks select departure and arrival phases with scale changes.
void Player_AddControlFlags(void *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Sound_PlaySong(u32);
void ActorSequence_Begin(void *, s32, s32);
void PlayerCue_CompleteAction(void *);
void PlayerCue_UpdateHidden(void *);
void PlayerCue_EnterStandUpdate(void *);
void Player_UpdateFacingSide(void *);
struct PlayerCueFields {
    u8 pad0[0xC0];
    u8 fC0;
    u8 fC1;
    u8 padC2[0x398];
    u8 f45A;
};
#define B(o) (*(u8 *)(p + (o)))
#define H(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
#define ANIM(n)                                                                                    \
    Player_SetAnimation(p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, n), 64, 1)
static inline void putH(u8 *p, u32 o, u32 v)
{
    *(u16 *)(p + o) = v;
}
void PlayerCue_UpdateScaleTransition(u8 *p)
{
    u16 *t;
    switch (((struct Player *)p)->tetherPhase) {
        case 0:
            H(0x6FC) = 15;
            if (ANIM(8))
                ActorSequence_Begin(p, 7, 1);
            break;
        case 1:
            H(0x6FC) = 30;
            ANIM(10);
            if (++((struct Player *)p)->tetherFrame > 15)
                ActorSequence_Begin(p, 7, 2);
            break;
        case 2:
            if (((struct Player *)p)->actionMode == 6 && !((struct Player *)p)->tetherFrame)
                Sound_PlaySong(0x327);
            H(0x6FC) = 30;
            W(0x94) |= 2;
            t = (u16 *)(p + 0x45A);
            ((struct PlayerCueFields *)p)->fC0 = 64 - (*(u8 *)t << 2);
            if (*t > 8) {
                ((struct PlayerCueFields *)p)->fC1 = *t << 3;
                {
                    s32 frame = *t - 8;
                    putH(p, 0x4E8, ((frame << 2) + frame) << 2);
                }
                Player_AddControlFlags(p, 4);
            }
            if (++*t > 15)
                ActorSequence_Begin(p, 7, 3);
            break;

        case 3:
            Player_AddControlFlags(p, 4);
            putH(p, 0x4E8, ((struct Player *)p)->tetherFrame * 200 + 160);
            if (++((struct Player *)p)->tetherFrame > 7) {
                Player_AddControlFlags(p, 1);
                ActorSequence_Begin(p, 0, 0);
                *(void (**)(void *))(p + 0x58C) = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(p);
            }
            break;
        case 4:
            if (((struct Player *)p)->actionMode == 6 && !((struct Player *)p)->tetherFrame)
                Sound_PlaySong(0x201);
            ANIM(0);
            Player_AddControlFlags(p, 4);
            W(0x94) |= 2;
            ((struct PlayerCueFields *)p)->fC0 = 4;
            ((struct PlayerCueFields *)p)->fC1 = 120;
            putH(p, 0x4E8, (8 - ((struct Player *)p)->tetherFrame) * 200 + 160);
            if (++((struct Player *)p)->tetherFrame > 7)
                ActorSequence_Begin(p, 7, 5);
            break;
        case 5:
            ANIM(0);
            W(0x94) |= 2;
            ((struct PlayerCueFields *)p)->fC0 =
                64 - ((15 - ((struct PlayerCueFields *)p)->f45A) << 2);
            if (((struct Player *)p)->tetherFrame <= 7) {
                ((struct PlayerCueFields *)p)->fC1 = (15 - ((struct Player *)p)->tetherFrame) << 3;
                putH(p, 0x4E8, (8 - ((struct Player *)p)->tetherFrame) * 20);
            } else {
                ((struct PlayerCueFields *)p)->fC1 = 64;
                putH(p, 0x4E8, 0);
                Player_AddControlFlags(p, 4);
            }
            if (++((struct Player *)p)->tetherFrame > 15)
                ActorSequence_Begin(p, 7, 6);
            break;
        case 6:
            W(0x94) &= ~2;
            ANIM(0);
            if (++((struct Player *)p)->tetherFrame > 3) {
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 7:
            if (((struct Player *)p)->actionMode == 6 && !((struct Player *)p)->tetherFrame)
                Sound_PlaySong(0x327);
            ANIM(0);
            W(0x94) |= 2;
            t = (u16 *)(p + 0x45A);
            ((struct PlayerCueFields *)p)->fC0 = 64 - (*(u8 *)t << 2);
            if (*t > 8) {
                ((struct PlayerCueFields *)p)->fC1 = *t << 3;
                {
                    s32 frame = *t - 8;
                    putH(p, 0x4E8, ((frame << 2) + frame) << 2);
                }
                Player_AddControlFlags(p, 4);
            }
            if (++*t > 15)
                ActorSequence_Begin(p, 7, 3);
            break;
        case 8:
            Player_AddControlFlags(p, 4);
            ANIM(0);
            putH(p, 0x4E8, ((struct Player *)p)->tetherFrame * 200 + 160);
            if (++((struct Player *)p)->tetherFrame > 7) {
                Player_AddControlFlags(p, 1);
                ActorSequence_Begin(p, 0, 0);
                *(void (**)(void *))(p + 0x58C) = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
