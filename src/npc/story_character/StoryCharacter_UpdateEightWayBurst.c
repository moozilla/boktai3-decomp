#include "global.h"
#include "engine/video/display_animation.h"
#include "engine/engine.h"
#include "engine/math/monster_particle_random.h"
#include "player/player.h"
#include "npc/manager/scene_actor_lifecycle.h"

s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);

s32 ImpactParticleGroup_SpawnRadial(s32, s32, struct ContactPosition *, s32, s32, s32, s32, s32,
                                    s32, s32, s32);
void PlayerCue_BeginModeSeven(struct Player *, s32);

static inline u8 Burst_RollSound(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u8 *)&gRandomTable[gRandomIndex];
}

static inline void Burst_EmitOuter(const u16 *table, u8 variance, s32 angle,
                                   struct ContactPosition *pos)
{
    s32 spread = ParticleRandom_ReadSample(table) & 63;
    ImpactParticleGroup_SpawnRadial(4, 0x26, pos, angle, spread, 12, 10, variance, 8, 8, variance);
}

static inline void Burst_EmitInner(const u16 *table, s32 angle, struct ContactPosition *pos)
{
    s32 spread = ParticleRandom_ReadSample(table) & 63;
    ImpactParticleGroup_SpawnRadial(2, 0x26, pos, angle, spread, 10, 4, 4, 4, 14, 4);
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

void StoryCharacter_UpdateEightWayBurst(u32 context, struct SceneActor *p, s32 tick)
{
    u8 variance = 5;
    u32 animation;
    if (p->animationEntered != 0) {
        p->animationEntered = 0;
        p->finished = 0;
        animation = Burst_SelectAnimation(p->variant, 8);

        Actor_PlayAnimationEntry(p, animation, 2, 0, 4);
        p->animationPhase = 0;
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
    if ((u8)Callback_InvokeActor(p, p->query) == 1)
        return;
    if ((p->animationPhase == 0) && (tick == 3)) {
        struct ContactPosition position = p->body.position;
        {
            u16 *x = (u16 *)&position.x;
            *x += 200;
        }
        Burst_EmitOuter(gRandomTable, variance, 0, &position);
        Burst_EmitOuter(gRandomTable, variance, 64, &position);
        Burst_EmitOuter(gRandomTable, variance, 128, &position);
        Burst_EmitOuter(gRandomTable, variance, 192, &position);

        Burst_EmitInner(gRandomTable, 0, &position);
        Burst_EmitInner(gRandomTable, 64, &position);
        Burst_EmitInner(gRandomTable, 128, &position);
        Burst_EmitInner(gRandomTable, 192, &position);
        PlayerCue_BeginModeSeven(gPlayers[0], 2);
    }
    if (DisplayAnimation_TestFinished(&p->drawable.common)) {
        switch (p->animationPhase) {
            case 0:
                animation = Burst_SelectAnimation(p->variant, 6);

                Actor_PlayAnimationEntry(p, animation, 2, 0, 4);
                p->animationPhase++;
                p->stateFrame = 0;
                break;
            case 1:
                p->finished = 1;
                p->animationPhase++;
                p->stateFrame = 0;
                break;
        }
    }
}
