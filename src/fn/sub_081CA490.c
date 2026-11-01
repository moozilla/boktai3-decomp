#include "global.h"
#include "display_animation.h"
#include "engine.h"
#include "monster_particle_random.h"
#include "player.h"
#include "state_actor.h"

s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);

#define ParticleGroup_SpawnRadial sub_080148CC
s32 ParticleGroup_SpawnRadial(s32, s32, struct StateActorPos *, s32, s32, s32, s32, s32, s32, s32,
                              s32);
void PlayerCue_BeginModeSeven(struct Player *, s32);

static inline u8 Burst_RollSound(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u8 *)&gRandomTable[gRandomIndex];
}

static inline void Burst_EmitOuter(const u16 *table, u8 variance, s32 angle,
                                   struct StateActorPos *pos)
{
    s32 spread = ParticleRandom_ReadSample(table) & 63;
    ParticleGroup_SpawnRadial(4, 0x26, pos, angle, spread, 12, 10, variance, 8, 8, variance);
}

static inline void Burst_EmitInner(const u16 *table, s32 angle, struct StateActorPos *pos)
{
    s32 spread = ParticleRandom_ReadSample(table) & 63;
    ParticleGroup_SpawnRadial(2, 0x26, pos, angle, spread, 10, 4, 4, 4, 14, 4);
}

static inline u32 Burst_SelectAnimation(u8 kind, u32 selected)
{
    u32 animation;
    switch (kind) {
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            animation = 0;
            break;
        case 15:
            animation = selected;
            break;
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
            animation = 0;
            break;

        default:
            animation = 0;
            break;
    }
    return animation;
}

void StateActor_UpdateEightWayBurst(u32 context, struct StateActor *p, s32 tick)
{
    u8 variance = 5;
    u32 animation;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        animation = Burst_SelectAnimation(p->kind, 8);

        Actor_PlayAnimationEntry(p, animation, 2, 0, 4);
        p->phase = 0;
        {
            s32 choice = Burst_RollSound();
            if (choice <= 84)
                Sound_PlaySong(0x2DE);
            else if (choice <= 169)
                Sound_PlaySong(0x2DF);
            else
                Sound_PlaySong(0x2E0);
        }
    }
    if ((u8)Callback_InvokeActor(p, p->animationCallback) == 1)
        return;
    if ((p->phase == 0) && (tick == 3)) {
        const u16 *table;
        struct StateActorPos position = p->position;
        {
            u16 *x = (u16 *)&position.x;
            u16 shifted = *x + 200;
            *x = shifted;
        }
        table = gRandomTable;
        Burst_EmitOuter(table, variance, 0, &position);
        Burst_EmitOuter(table, variance, 64, &position);
        Burst_EmitOuter(table, variance, 128, &position);
        Burst_EmitOuter(table, variance, 192, &position);

        Burst_EmitInner(table, 0, &position);
        Burst_EmitInner(table, 64, &position);
        Burst_EmitInner(table, 128, &position);
        Burst_EmitInner(table, 192, &position);
        PlayerCue_BeginModeSeven(gPlayers[0], 2);
    }
    if (DisplayAnimation_TestFinished((struct DisplayAnimation *)((u8 *)p + 0xD4))) {
        switch (p->phase) {
            case 0:
                animation = Burst_SelectAnimation(p->kind, 6);

                Actor_PlayAnimationEntry(p, animation, 2, 0, 4);
                p->phase++;
                p->stateFrame = 0;
                break;
            case 1:
                p->done = 1;
                p->phase++;
                p->stateFrame = 0;
                break;
        }
    }
}
