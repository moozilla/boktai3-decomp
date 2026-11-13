#include "actor_marker.h"

void ActorMarker_InitOffsetPosition(struct SceneCombatActor *actor)
{
    SceneCombatActor_InitBouncingParticle(actor);
    actor->looseParticles[1].velocity.components[2] = 0;
    actor->looseParticles[1].velocity.components[1] = 0;
    actor->looseParticles[1].velocity.components[0] = 0;
    switch (((actor->heading + 0x60) >> 6) & 3) {
        case 1:
            actor->looseParticles[1].sprite.position.components[0] = actor->position.components[0] - 50;
            actor->looseParticles[1].sprite.position.components[1] = actor->position.components[1] + 100;
            actor->looseParticles[1].sprite.position.components[2] = actor->position.components[2] - 200;
            actor->looseParticles[1].sprite.phase = 0;
            break;

        case 2:
            actor->looseParticles[1].sprite.position.components[0] = actor->position.components[0] - 200;
            actor->looseParticles[1].sprite.position.components[1] = actor->position.components[1] + 100;
            actor->looseParticles[1].sprite.position.components[2] = actor->position.components[2] - 50;
            actor->looseParticles[1].sprite.phase = 0;
            break;

        case 3:
            actor->looseParticles[1].sprite.position.components[0] = actor->position.components[0] - 200;
            actor->looseParticles[1].sprite.position.components[1] = actor->position.components[1] + 100;
            actor->looseParticles[1].sprite.position.components[2] = actor->position.components[2] - 200;
            actor->looseParticles[1].sprite.phase = 0;
            break;

        case 0:
            actor->looseParticles[1].sprite.position.components[0] = actor->position.components[0] - 200;
            actor->looseParticles[1].sprite.position.components[1] = actor->position.components[1] + 100;
            actor->looseParticles[1].sprite.position.components[2] = actor->position.components[2] - 200;
            actor->looseParticles[1].sprite.phase = 0;
            break;
    }
}

void ActorMarker_InitMovingSprite(struct SceneCombatActor *actor)
{
    switch (actor->kind) {
        default:
            ActorMarker_InitFacingSprite(actor);
            actor->facingMarker.velocity.components[2] = 0;
            actor->facingMarker.velocity.components[1] = 0;
            actor->facingMarker.velocity.components[0] = 0;
            actor->looseParticles[1].velocity.components[1] = 30;
            actor->facingMarker.sprite.position.components[0] = actor->position.components[0];
            actor->facingMarker.sprite.position.components[1] = 400 + actor->position.components[1];
            actor->facingMarker.sprite.position.components[2] = actor->position.components[2];
            actor->facingMarker.sprite.phase = 0;
            switch (((actor->heading + 0x60) >> 6) & 3) {
                case 0:
                    actor->facingMarker.velocity.components[2] = 15;
                    break;
                case 1:
                    actor->facingMarker.velocity.components[0] = 0xfff1;
                    break;
                case 2:
                    actor->facingMarker.velocity.components[2] = 0xfff1;
                    break;
                case 3:
                    actor->facingMarker.velocity.components[0] = 15;
                    break;
            }
            break;
        case 3:
        case 5:
        case 7:
        case 9:
        case 13:
        case 15:
            break;
    }
}

void ActorMarker_EmitRingEffects(struct SceneCombatActor *actor)
{
    s32 row = 0;
    struct ActorMarkerEffectPosition position;
    s32 index;
    const u16 *actorPosition;
    const u16(*offsetTable)[4];
    switch (((actor->heading + 0x60) >> 6) & 3) {
        case 1:
        case 3:
            row = 0;
            break;
        case 0:
        case 2:
            row = 1;
            break;
    }
    index = 0;
    offsetTable = gUnk_0824E6F8;
    actorPosition = actor->position.components;
    if (index <= 6) {
        do {
            const u16 *offset = (const u16 *)(((row * 8 - row + index) * 8) + (u32)offsetTable);
            position.x = actorPosition[0] + offset[0];
            position.y = actor->position.components[1] + offset[1];
            position.z = actor->position.components[2] + offset[2];
            SparkParticle_Spawn(&position, 1, gUnk_0824E768, actor->heading + 0x80, 3, 8, 2, 2);
            index++;
        } while (index <= 6);
    }
}

void ActorMarker_EmitIndexedEffect(struct SceneCombatActor *actor, s32 index)
{
    s32 row = 0;
    struct ActorMarkerEffectPosition position;
    const u16 *offset;
    u32 value;
    const u16 *actorPosition;
    s32 direction;
    switch (((actor->heading + 0x60) >> 6) & 3) {
        case 1:
        case 3:
            row = 0;
            break;
        case 0:
        case 2:
            row = 1;
            break;
    }
    actorPosition = actor->position.components;
    {
        u32 table = (u32)gUnk_0824E778;
        u32 scaled = index * 8;
        scaled += row * 80;
        offset = (const u16 *)(scaled + table);
    }
    position.x = actorPosition[0] + offset[0];
    position.y = actor->position.components[1] + offset[1];
    position.z = actor->position.components[2] + offset[2];
    direction = Math_Atan2((s16)offset[0], (s16)offset[2]);
    SparkParticle_Spawn(&position, 1, gUnk_0824E770, direction, 3, 8, 2, 2);
}

void Armor_SpawnRandomDirectionalParticle(struct SceneCombatActor *actor)
{
    u32 randomIndex;
    gRandomIndex = randomIndex = (gRandomIndex + 1) & 0x3FF;
    ActorMarker_EmitIndexedEffect(actor, Mod(*(u16 *)((u8 *)gRandomTable + randomIndex * 2), 10));
}

void Armor_SpawnDirectionalParticleFan(struct SceneCombatActor *actor)
{
    s32 index = 0;
    do {
        ActorMarker_EmitIndexedEffect(actor, index);
        index++;
    } while (index <= 8);
}

static inline u16 NextFragmentLift(const u16 *table, u32 *index)
{
    u32 value = 1 + *index;
    u32 mask = 0x3ff;
    value &= mask;
    *index = value;
    value = table[value];
    mask = 31;
    return value & mask;
}

void ActorMarker_LaunchFragments(struct SceneCombatActor *actor, u32 fast)
{
    s32 row = 0;
    s32 index;
    const struct ActorMarkerFragmentOffset *rowStart;
    const struct ActorMarkerFragmentOffset *table;
    s32 row8;
    const u16 *randomTable;
    u32 *randomIndex;
    const u16 *posX;
    const u16 *posY;
    const u16 *posZ;
    switch (((actor->heading + 0x60) >> 6) & 3) {
        case 1:
            row = 0;
            break;
        case 2:
            row = 0;
            break;
        case 0:
        case 3:
            row = 1;
            break;
    }
    ActorMarker_InitBouncingParticles(actor);
    index = 0;
    row8 = row * 8;
    posY = &actor->position.components[1];
    posZ = &actor->position.components[2];
    posX = &actor->position.components[0];
    table = gUnk_0824E818;
    randomTable = gRandomTable;
    randomIndex = &gRandomIndex;
    rowStart = table + (row8 + row);
    /* Index the immutable offset table relative to the selected row. */
    for (; index < 9; index++) {
        struct BouncingSceneParticle *fragment = &actor->fragments[index];
        fragment->bounced = 0;
        fragment->sprite.position.components[0] = *posX + gUnk_0824E818[(rowStart + index) - table].x;
        fragment->sprite.position.components[1] = *posY + gUnk_0824E818[(rowStart + index) - table].y;
        fragment->sprite.position.components[2] = *posZ + gUnk_0824E818[(rowStart + index) - table].z;
        if (fast) {
            fragment->velocity.signedComponents[0] = gUnk_0824E818[(rowStart + index) - table].x >> 3;
            fragment->velocity.signedComponents[1] = NextFragmentLift(randomTable, randomIndex);
            fragment->velocity.signedComponents[2] = gUnk_0824E818[(rowStart + index) - table].z >> 3;
        } else {
            fragment->velocity.signedComponents[0] = gUnk_0824E818[(rowStart + index) - table].x >> 5;
            fragment->velocity.signedComponents[1] = NextFragmentLift(randomTable, randomIndex);
            fragment->velocity.signedComponents[2] = gUnk_0824E818[(rowStart + index) - table].z >> 5;
        }
    }
    Sound_PlaySongChecked(0xc4);
}
