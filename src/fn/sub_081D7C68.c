#include "task4e01_projectile.h"
void Task4E01_OnDefaultDirectionalAnimation(void *);
void Task4E01_OnType7DirectionalAnimation(void *);
void Task4E01_OnType4Animation(void *);
void Projectile_UpdateAnimation(void *);
void Projectile_SpawnImpactPair(void *);

s32 Task4E01_StartTerminalAnimation(struct Task4E01Projectile *p)
{
    struct Task4E01Params *config = &p->params;
    config->flags |= 0x1000;
    p->frame = 0;
    config->speed = 0;
    switch (config->type) {
        case 0:
        case 1:
        case 2:
        case 5:
        case 6:
        case 10: {
            struct ContactSwarmSprite *sprite = &p->sprite;
            u8 frame = 29;
            sprite->frame = frame;
        }
            Projectile_SetUpdateCallback(config, Task4E01_OnDefaultDirectionalAnimation);
            p->frame = 0;
            break;
        case 7: {
            struct ContactSwarmSprite *sprite = &p->sprite;
            u8 frame = 29;
            sprite->frame = frame;
        }
            Projectile_SetUpdateCallback(config, Task4E01_OnType7DirectionalAnimation);
            break;
        case 4:
            Projectile_SetUpdateCallback(config, Task4E01_OnType4Animation);
            break;
        case 3:
            if (config->flags & 0x10) {
                p->animation = 1;
                Anim_RestartSequence(&p->sequence, p->resource, 1, 0, 0);
                PlayerCue_StepSequenceEnded(&p->sprite, &p->sequence);
            } else {
                p->animation = 6;
                Anim_RestartSequence(&p->sequence, p->resource, 6, 0, 0);
                PlayerCue_StepSequenceEnded(&p->sprite, &p->sequence);
            }
            Projectile_SetUpdateCallback(config, Projectile_UpdateAnimation);
            break;
        case 11:
            if (!(config->flags & 0x2000)) {
                struct ContactSwarmSprite *sprite = &p->sprite;
                u8 frame = 29;
                sprite->frame = frame;
            }
            Projectile_SetUpdateCallback(config, Projectile_SpawnImpactPair);
            break;
        case 8:
        case 9:

        default:
            break;
    }

    {
        u32 set = 1;
        p->flags = p->flags | set;
    }
    return 0;
}
