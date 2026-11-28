#include "monsters/muspell_head.h"
#include "scene/terrain/terrain_cell_probe.h"
#include "objects/hazards/animated_hazard.h"
#include "scene/collision/contact_sample_vector_layout.h"
u32 MuspellHead_UpdateProjectileAttack(void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
s32 Div(s32, s32);
static inline void subtract(struct ContactPosition *v, struct ContactPosition *a,
                            struct ContactPosition *b)
{
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}
static inline s32 distanceSquared(struct ContactPosition *a, struct ContactPosition *b)
{
    s32 x = a->x - b->x;
    s32 z = a->z - b->z;
    return x * x + z * z;
}
u32 MuspellHead_TryProjectileAttack(struct MuspellHead *head)
{
    struct ContactPosition delta;
    struct ContactSampleVector sample;
    s32 i;
    if (head->enabled) {
        if (distanceSquared(Player_GetEffectivePosition(0), &head->sprite.position) <
            head->attackDistance) {
            subtract(&delta, Player_GetEffectivePosition(0), &head->sprite.position);
            for (i = 1; i <= 4; i++) {
                u8 *cell;
                sample.x = head->sprite.position.x + Div(delta.x * i, 4);
                sample.y = head->sprite.position.y + Div(delta.y * i, 4);
                sample.z = head->sprite.position.z + Div(delta.z * i, 4);
                cell = cellProps(cellIndexAt((struct Pos16 *)&sample));
                if (((cell[0] & 15u) << 8) > head->sprite.position.y + 128)
                    break;
                if (i == 4) {
                    head->action = (void (*)(void))MuspellHead_UpdateProjectileAttack;
                    Anim_RestartSequence(&head->playback, head->sequence, 1, 0, 0);
                    head->timer = head->duration;
                    head->phase = 0;
                    Sound_PlaySong(0x51f);
                }
            }
        }
    }
    return 0;
}
