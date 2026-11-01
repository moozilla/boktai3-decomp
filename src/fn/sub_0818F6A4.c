#include "global.h"
#include "player_cue_sequence.h"
#include "display_animation.h"
#include "engine.h"
struct NidhoggrVector {
    s16 x, y, z, pad;
};
struct NidhoggrSprite {
    u32 flags;
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct NidhoggrVector position;
    u8 pad24[8];
};
struct NidhoggrEffect {
    void *update, *destroy;
    u8 animation[0x1c];
    struct NidhoggrSprite sprite;
    struct DisplaySequenceState sequence;
    u8 pad60[0x114];
    void *anchor;
    struct NidhoggrVector velocity;
    u8 state, age;
    s16 ground;
    u8 *boss;
};
void Nidhoggr_CreatePalette426Effect(void *, void *);
void Nidhoggr_CreatePalette424Effect(void *, void *);
void Nidhoggr_CreateTimedPositionEffect(void *, void *);
void EffectPool_CreateTimedEffect(void *, void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void ScaledShadow_Enable(void *);
void Sound_PlaySongChecked(u32);

u8 *TerrainModifiers_FindCellOverride(int, int);
static inline u8 *cell(int index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + index * 4;
}
// confidence=low: rising effect retargets the player before descending and emitting an impact.

void Nidhoggr_UpdateFallingAttackEffect(struct NidhoggrEffect *p)
{
    u8 *age;
    u8 *state;
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->sprite, &p->sequence);
    age = &p->age;
    if (!((*age) & 3))
        Nidhoggr_CreatePalette426Effect(p->boss + 0x54, &p->sprite.position);
    state = &p->state;
    switch (*state) {
        case 0:
            if ((*age) > 15) {
                *state = 1;
                p->anchor = 0;
                p->velocity.x = 0;
                p->velocity.y = 0x33;
                p->velocity.z = 0;
                p->ground = p->sprite.position.y;
            } else
                p->velocity.y += 0x33;
            break;

        case 1:
            if ((p->sprite.position.y - p->ground) > 0x1000) {
                u8 *boss = p->boss;
                struct NidhoggrVector *position;
                struct NidhoggrVector *savedPosition;
                u16 value;
                int x;
                int z;
                int index;
                int slope;
                int height;
                u8 *c;
                struct NidhoggrVector v;
                value = gGameState->positionX;
                p->sprite.position.x = value;
                value = gGameState->positionZ;
                p->sprite.position.z = value;
                p->velocity.y = -0x33;
                (*state)++;
                Anim_RestartSequence(&p->sequence, *((void **)(boss + 0x5400)), 4, 0, 0);
                position = &p->sprite.position;
                x = position->x >> 8;
                z = position->z >> 8;
                savedPosition = position;
                if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) ||
                    (((u32)z) >= gTerrainHeight))
                    index = 0;
                else
                    index = gTerrainMap->rows[z] + x;
                c = cell(index);
                slope = (*c) >> 4;
                height = ((*c) & 15) << 8;
                switch (slope) {
                    case 1:
                        height -= (u8)position->z;
                        break;

                    case 2:
                        height -= (u8)position->x;
                        break;
                }

                p->ground = height;
                {
                    struct EffectCollider {
                        u8 pad[6];
                        u16 flags;
                    };
                    struct EffectCollider *collider = (void *)(((u8 *)p) + 0xcc);
                    u32 mask = ~4;
                    collider->flags &= mask;
                }
                ScaledShadow_Enable(((u8 *)p) + 0x60);
                v.x = savedPosition->x;
                v.y = p->ground;
                v.z = *((u16 *)(&p->sprite.position.z));
                EffectPool_CreateTimedEffect(boss + 0x54, &v);
            }
            break;

        case 2:
            if ((p->sprite.position.y - p->ground) <= 0x87f) {
                Sound_PlaySongChecked(0x1b5);
                (*state)++;
            }

        case 3:
            if (p->sprite.position.y < (p->ground + 0x100)) {
                u8 *boss = p->boss;
                struct NidhoggrVector v;
                v.x = p->sprite.position.x;
                v.y = p->sprite.position.y;
                v.z = p->sprite.position.z;
                if (p->ground <= 0) {
                    v.y -= 0x100;
                    Nidhoggr_CreatePalette424Effect(boss + 0x54, &v);
                } else {
                    v.y -= 0x80;
                    Nidhoggr_CreateTimedPositionEffect(&v, boss);
                }
                goto destroy;
            }
            break;

        case 255: {
            u8 *boss = p->boss;
            Nidhoggr_CreateTimedPositionEffect(&p->sprite.position, boss);
        }
        destroy:
            p->update = 0;

            if (p->destroy) {
                Callback_InvokeActor(p, p->destroy);
                p->destroy = 0;
            }
            break;
    }
}
