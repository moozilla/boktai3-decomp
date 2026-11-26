#include "engine/shared/global.h"
#include "field/weather_and_storm_controller/weather_state.h"
#include "engine/shared/engine.h"
#include "player/player.h"
#include "engine/shared/sprite_drawable_api.h"
#include "engine/shared/terrain_collision.h"
// confidence=low: creates weather particles with two terrain-adjusted endpoints.
#define Weather_SpawnTerrainParticles sub_080177AC
u32 Weather_TestFlags(u32, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Mod(s32, s32);
#define RANDOM_TABLE ((u16 *)0x0203B400)
static inline u32 Weather_DrawDelaySample(void)
{
    u32 n = (gRandomIndex + 1) & 0x3ff;
    gRandomIndex = n;
    return RANDOM_TABLE[n];
}
static inline s32 Weather_GetParticleGroundHeight(struct ContactPosition *position, s16 x, s16 z)
{
    s32 col = x >> 8, row = z >> 8;
    u32 cell;
    struct TerrainCell *tile;
    struct TerrainCellOverride *override;
    s32 slope, height;
    if (col < 0 || row < 0 || (u32)col >= gTerrainWidth || (u32)row >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[row] + col;
    override = (struct TerrainCellOverride *)TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        tile = &override->cell;
    else
        tile = &((struct TerrainGrid *)gTerrainMap->cells)->cells[cell];
    slope = tile->shapeAndHeight >> 4;
    height = (tile->shapeAndHeight & 15) << 8;
    switch (slope) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
static inline u32 Weather_DrawEndpointSample(const u16 *table)
{
    u32 n = (gRandomIndex + 1) & 0x3ff;
    gRandomIndex = n;
    return table[n];
}
static inline void Weather_SetParticleEndpoint(struct WeatherParticle *out,
                                               struct WeatherParticle *sample,
                                               struct Player *player, u32 index)
{
    union RandomTableView table = {gRandomTable};
    s32 x, z;
    x = (Weather_DrawEndpointSample(table.samples) & 0xfff) + (u16)player->position.x - 0x800;
    if (index == 0)
        out->positions[0].x = x;
    else
        out->positions[1].x = x;
    z = (Weather_DrawEndpointSample(table.samples) & 0xfff) + (u16)player->position.z - 0x800;
    if (index == 0)
        out->positions[0].z = z;
    else
        out->positions[1].z = z;
    if (index == 0)
        out->positions[0].y = Weather_GetParticleGroundHeight(&sample->positions[0], x, z);
    else
        out->positions[1].y = Weather_GetParticleGroundHeight(&sample->positions[1], x, z);
}
s32 Weather_SpawnTerrainParticles(struct WeatherState *p, s32 count, s32 delay, s32 delayRange)
{
    struct Player *player = gPlayers[0];
    s32 i;
    u16 *timer;
    u32 remaining;
    if (Weather_TestFlags(p->flags, 4))
        return 0;
    remaining = p->particleDelay;
    timer = &p->particleDelay;
    if (remaining) {
        *timer = remaining - 1;
        return 0;
    }
    for (i = 0; i < count;) {
        struct WeatherParticle *sample = p->particles;
        struct WeatherParticle *e;
        s32 j = 0;
        s32 next = i + 1;
        e = sample;
        for (; j < 32; j++, e++, sample++) {
            if (!e->active) {
                struct SpriteDrawable *drawable;
                e->state = 0;
                e->timer = 0;
                e->endpoint = 0;
                Weather_SetParticleEndpoint(e, sample, player, 0);
                Weather_SetParticleEndpoint(e, sample, player, 1);
                drawable = &sample->sprite;
                e->sprite.position = *(sample->positions + e->endpoint);
                SpriteDrawable_SelectTileFrame(drawable, p->particleResource, 30);
                drawable->priority = 1;
                e->sprite.flags &= ~1;
                Effect_LinkToList(drawable, 0);
                e->active = 1;
                break;
            }
        }
        i = next;
    }
    *timer = delay;
    if (delayRange)
        *timer += Mod(Weather_DrawDelaySample(), delayRange);
    return 0;
}
