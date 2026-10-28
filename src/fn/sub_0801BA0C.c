#include "global.h"
#include "player.h"
typedef struct {
    s16 x, y, z, pad;
} Vec;
typedef struct {
    u8 pad[48];
    Vec position;
} Actor;
int ResultMenu_IsPhaseStart(void *);
void ResultMenu_SendCommand(void *, int, int, void *);
void PlayerCue_BeginModeSeven(void *, int);
void Sound_PlaySong(int);
void AnimatedParticles_EmitChargeSparkle(int, int, int, int, void *);
void ResultMenu_SetPhase(void *, void (*)(void));
void Transformation_Reveal(void);
void Transformation_Charge(u8 *p)
{
    Actor *actor = (*(Actor **)gPlayers);
    Actor *positionActor = actor;
    u16 param;
    Vec pos;
    if (ResultMenu_IsPhaseStart(p)) {
        param = 2;
        ResultMenu_SendCommand(p, 1, 1, &param);
        PlayerCue_BeginModeSeven(actor, 3);
        Sound_PlaySong(0x2e1);
    }
    if (*(u32 *)(p + 0x3c) - 30 <= 69 && !(*(u32 *)(p + 0x3c) & 3)) {
        {
            Vec *out = &pos;
            pos = positionActor->position;
            out->x += 200;
            AnimatedParticles_EmitChargeSparkle(2, 0, 0x2000, 64, out);
        }
        Sound_PlaySong(0x1c5);
    }
    if (*(u32 *)(p + 0x3c) > 99)
        ResultMenu_SetPhase(p, Transformation_Reveal);
    else
        (*(u32 *)(p + 0x3c))++;
}
