#include "global.h"

#include "contact_swarm.h"
#include "player_cue_sequence.h"

// Drawable layout shared with ActorDisplay_UpdateForegroundSprite.
struct Drawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 unknown3C;
    s32 ended;
    u32 mode;
};
static inline void SetFrame(struct Drawable *owner, u16 frame)
{
    owner->sprite.frame = frame;
}

void Anim_RestartSequence(void *, void *, u16, u8, u8);
void Projectile_SetUpdateCallback(void *, void *);
void Projectile_SetCollisionSize(void *, s32, s32);
u32 sub_08249498(u32, u32);
void Task4E01_SpawnImpactChildren(void *);
void Task4E01_UpdateExpandingImpact(void *);
void Task4E01_OnDefaultDirectionalAnimation(void *);
void Task4E01_OnType7DirectionalAnimation(void *);
void Task4E01_OnType4Animation(void *);
void Projectile_UpdateAnimation(void *);
void Projectile_SpawnImpactPair(void *);
// confidence=low: resets projectile time and chooses the terminal animation by projectile kind.
struct ImpactConfig {
    u16 flags;
    u8 pad[6];
    u32 time;
    u8 padC[0x48 - 12];
    u32 kind;
};
struct ImpactHeader {
    u32 time, flags;
    u8 pad[16];
    struct ImpactConfig config;
};
s32 Task4E01_StartTerminalAnimation(u8 *input)
{
    u8 *p = input;
    u8 *config = input + 0x18;
    ((struct ImpactHeader *)input)->config.flags |= 0x1000;
    ((struct ImpactHeader *)input)->time = 0;
    ((struct ImpactConfig *)config)->time = 0;
    switch (((struct ImpactConfig *)config)->kind) {
        case 0:

        case 1:

        case 2:

        case 5:

        case 6:

        case 10:
            SetFrame((struct Drawable *)(input + 0x8c), 29);
            Projectile_SetUpdateCallback(config, Task4E01_OnDefaultDirectionalAnimation);
            ((struct ImpactHeader *)input)->time = 0;
            break;

        case 7:
            ((struct Drawable *)(input + 0x8c))->sprite.frame = 29;
            Projectile_SetUpdateCallback(config, Task4E01_OnType7DirectionalAnimation);
            break;

        case 4:
            Projectile_SetUpdateCallback(config, Task4E01_OnType4Animation);
            break;

        case 3:
            if ((*((u16 *)config)) & 0x10) {
                *((u32 *)(input + 0x194)) = 1;
                Anim_RestartSequence(input + 0x180, *((void **)(input + 0x19c)), 1, 0, 0);
                PlayerCue_StepSequence((struct ContactSwarmSprite *)(input + 0x8c),
                                       (struct DisplaySequenceState *)(input + 0x180));
            } else {
                *((u32 *)(input + 0x194)) = 6;
                Anim_RestartSequence(input + 0x180, *((void **)(input + 0x19c)), 6, 0, 0);
                PlayerCue_StepSequence((struct ContactSwarmSprite *)(input + 0x8c),
                                       (struct DisplaySequenceState *)(input + 0x180));
            }
            Projectile_SetUpdateCallback(config, Projectile_UpdateAnimation);
            break;

        case 11: {
            if (!(*(u16 *)config & 0x2000))
                ((struct Drawable *)(input + 0x8c))->sprite.frame = 29;
            Projectile_SetUpdateCallback(config, Projectile_SpawnImpactPair);
            break;
        }

        case 8:

        case 9:

        default:
            break;
    }

    {
        u8 k1 = 1;
        ((struct ImpactHeader *)input)->flags |= k1;
    }
    return 0;
}
