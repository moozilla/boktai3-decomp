#include "global.h"
#include "contact_swarm.h"
#include "engine.h"
#include "ram.h"
#include "player_cue_sequence.h"
struct RegionSprite {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x90 - 0x2C];
    struct DisplaySequenceState sequence;
    struct ContactPosition origin;
    u8 unknownA8[0xEB - 0xA8];
    u8 hit;
    u8 unknownEC[0xFC - 0xEC];
    s16 health, maxHealth;
    s32 threshold;
    s16 resourceId, kind;
    u32 behaviorFlags;
    u8 entered, mode;
    u16 frames;
    u8 overlayActive, variant, unknown112[2];
    s32 region;
    u8 terrainCell[0x10];
};
struct RegionSpriteManager {
    u8 unknown0[0x18];
    const void *sequenceResource, *alternateResource;
};
void ProximityActor_UpdateDamageAnimation(struct RegionSprite *);
void sub_0810638C(struct RegionSprite *);
static inline u8 Test(u32 *flags, u32 bit)
{
    if (*flags & bit)
        return 1;
    return 0;
}
struct RegionSprite *Pool128_Allocate(struct RegionSpriteManager *);
void ProximityActor_InitializeCollider(struct RegionSprite *, s16, s16);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
s32 Region_FindContaining(void *);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
// confidence=low: script-created region actor selects one of two sprite resources and registers its terrain cell.
void RegionActor_CreateFromScript(void)
{
    struct RegionSpriteManager *manager = gRegionActorPool;
    struct RegionSprite *p;
    s32 width, height, x, z, cell;
    struct ContactSwarmSprite *sprite;
    u32 one;
    u32 *flags;
    if (!manager)
        return;
    p = Pool128_Allocate(manager);
    if (!p)
        return;
    p->behaviorFlags = 1;
    if (Script_SeekToKeyword('p')) {
        p->origin.x = Script_GetValue();
        p->origin.y = Script_GetValue();
        p->origin.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        width = Script_GetValue();
        height = Script_GetValue();
    } else {
        width = 0;
        height = 0;
    }
    p->health = Script_GetKeywordValue('h', 0);
    p->maxHealth = p->health;
    sprite = &p->sprite;
    sprite->position = p->origin;
    Sprite_Init(p, (u8 *)p + 0x2C, 0);
    PlayerCue_StepSequenceEnded(sprite, &p->sequence);
    {
        struct ContactSwarmSprite *s = sprite;
        u32 f = s->flags;
        one = 1;
        s->flags = f | one;
    }
    p->variant = Script_GetKeywordValue('m', 0);
    switch (p->variant) {
        case 0:
            AnimData_Load((u8 *)p + 0x2C, 0x4FDA);
            Anim_SetSequence(&p->sequence, manager->sequenceResource, 0, 0, 0);
            p->kind = one;
            sprite->priority = 3;
            flags = &p->behaviorFlags;
            break;
        case 1:
            AnimData_Load((u8 *)p + 0x2C, 0x863A);
            Anim_SetSequence(&p->sequence, manager->alternateResource, 2, 0, 0);
            p->kind = 2;
            {
                u32 bit = 4;
                p->behaviorFlags |= bit;
            }
            sprite->priority = 2;
            flags = &p->behaviorFlags;
            break;
        default:
            flags = &p->behaviorFlags;
            break;
    }
    p->mode = 0;
    p->threshold = Div(p->health, 3);
    ProximityActor_InitializeCollider(p, width, height);
    x = p->sprite.position.x >> 8;
    z = p->sprite.position.z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    TerrainModifiers_RegisterCell(p->terrainCell, cell, 0, p->sprite.position.y >> 8, 0, 128);
    {
        u32 bit = 0x20;
        *flags |= bit;
    }
    if (Script_GetKeywordValue('P', 0))
        *flags |= 0x80;
    p->region = Region_FindContaining(&p->origin);
    if (Script_GetKeywordValue('s', 0)) {
        p->overlayActive = 1;
        SceneBody_Init((u8 *)p + 0x48, p->resourceId + 0x6F, &p->origin, 0, 7, p);
        SceneBody_SetSprite((u8 *)p + 0x48, p);
        TerrainOverlay_QueueAddActor((u8 *)p + 0x48);
    } else
        p->overlayActive = 0;
}
