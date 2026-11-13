#include "player_core.h"

void ScreenMote_ResetManager(void)
{
    gScreenMoteManager = 0;
}

void Player_UpdateFacingSide(struct Player *player)
{
    u32 direction = player->facing;
    if (direction > 4) {
        u32 side = 8 - direction;
        player->heading = side;
        player->side = 1;
    } else {
        player->heading = direction;
        player->side = 0;
    }
}

s32 Player_SetAnimation(void *player, u32 index, u32 rate, u32 mode)
{
    struct Player *actor;
    struct PlayerAnimation *anim;
    void *data;
    u32 flags = 4;
    actor = player;
    if (actor->animationIndex == index)
        flags = 5;
    actor->animationIndex = index;
    anim = &actor->animation;
    data = actor->animationData;
    {
        u32 sum = index;
        sum += actor->heading;
        Anim_SetKeyframes(&actor->animation, data, (u16)sum, (u16)mode, flags);
    }
    if (actor->side)
        anim->flags |= 4;
    else
        (&actor->animation)->flags &= ~4;
    Anim_SetFrameDurationScale(&actor->animation, (u16)rate);
    return Anim_Step(&actor->animation, actor->animationData);
}

s32 PlayerAnimation_SelectSecondaryKeyframes(struct Player *player, s32 index, s32 rate)
{
    struct PlayerAnimation *anim;
    void *data;
    u32 flags = 0;
    if (player->animationIndex == index) {
        flags = 1;
    }
    player->animationIndex = index;
    anim = &player->secondaryAnimation;
    data = player->animationData;
    {
        s32 selected = index;
        selected = selected + player->heading;
        Anim_SetKeyframes(anim, data, (u16)selected, 1, flags);
    }
    if (player->side) {
        (&player->secondaryAnimation)->flags |= 4;
    } else {
        anim->flags &= ~4;
    }
    Anim_SetFrameDurationScale(&player->secondaryAnimation, (u16)rate);
    return Anim_Step(&player->secondaryAnimation, player->animationData);
}

static inline s32 scale12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -(-v >> 12);
}

static inline s32 scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -(-v >> 6);
}

void Player_SetDirectionalVelocity(struct Player *player, s32 speed)
{
    s32 angle = ((player->facing + 5) & 7) << 5;
    player->velocityX = scale12(gUnk_086149C4[(angle + 64) & 255] * speed);
    player->velocityZ = scale12(gUnk_086149C4[angle] * speed);
    if (player->velocityScaleAxis == 1) {
        if (player->velocityZ < 0)
            player->velocityZ = scale6(player->velocityZ * 40);
        else
            player->velocityZ = scale6(player->velocityZ * 52);
    } else if (player->velocityScaleAxis == 2) {
        if (player->velocityX < 0)
            player->velocityX = scale6(player->velocityX * 40);
        else
            player->velocityX = scale6(player->velocityX * 52);
    }
}

void ActorSequence_Begin(struct Player *actor, u32 sequence, u32 variant)
{
    actor->unk45C = 1;
    actor->tetherMode = sequence;
    actor->tetherPhase = variant;
    actor->tetherFrame = 0;
}

u32 Player_ConsumeStateEntry(struct Player *player)
{
    if (player->unk45C) {
        player->unk45C = 0;
        return 1;
    }
    return 0;
}

void Player_SetActionPhase(struct Player *player, u8 phase)
{
    u8 *currentPhase = &player->tetherPhase;
    u16 firstFrame = 0;
    *currentPhase = phase;
    player->tetherFrame = firstFrame;
}

void Player_SetPendingCallback(struct Player *player, void (*callback)(struct Player *))
{
    player->pendingCallback = callback;
}

void Player_RunPendingCallback(struct Player *player)
{
    void (**callback)(struct Player *) = &player->pendingCallback;
    if (*callback) {
        (*callback)(player);
        *callback = 0;
    }
}

static inline u32 HasFlagBits(u32 *flags, u32 mask)
{
    return *flags & mask;
}

static inline void SetFlagBits(u32 *flags, u32 mask)
{
    *flags |= mask;
}

static inline struct GameState *EventCounterBase(u32 slot, struct GameState *state)
{
    return (void *)((slot * sizeof(state->counters628[0])) + (u32)state);
}

void Player_ResetStatus(struct Player *player)
{
    player->statusFlags = 1;
    if (*(s32 *)(u32)&EventCounterBase(player->timedEventSlot, gGameState)->counters628[0] < 0)
        *(s32 *)(u32)&EventCounterBase(player->timedEventSlot, gGameState)->counters628[0] = 0;
    if (*(s32 *)(u32)&EventCounterBase(player->timedEventSlot, gGameState)->counters628[0] == 0 &&
        AreaEffect_TestPlayerCoverage((u8 *)player))
        SetFlagBits(&player->statusFlags, 0x10);
    if (player && HasFlagBits(&player->statBonusFlags, 0x2000))
        SetFlagBits(&player->statusFlags, 0x10000);
    player->resetValue6F8 = 0;
    player->resetValue6FA = 0;
    player->resetValue6FC = 0;
    player->resetValue41A = 0;
    player->resetState740 = 1;
    player->resetFlags &= ~2;
}

void Player_AddControlFlags(struct Player *player, u32 flags)
{
    player->resetValue41A |= flags;
}

u16 Player_GetControlFlags(struct Player *player, u16 mask)
{
    return player->resetValue41A & mask;
}

void Player_StopActionSounds(struct Player *player)
{
    if (player->actionMode != 6) {
        Sound_StopSong(0xd8);
    } else {
        Sound_StopSong(0x239);
        Sound_StopSong(0x202);
        Sound_StopSong(0x366);
    }
}

static inline s32 ImpactDirection(s32 angle)
{
    return (((angle & 255) >> 6) << 1) + 1;
}

s32 ActorDirection_ComputeRelativeCardinal(struct ContactCollider *source,
                                           struct ContactCollider *target)
{
    s32 angle;
    if (source->flags & 0x100) {
        u32 t = source->direction;
        angle = t + 0xe0;
        return ImpactDirection(angle);
    } else {
        s32 x = source->x - target->x;
        s32 z = source->z - target->z;
        if (x == 0 && z == 0) {
            angle = target->direction + 0xe0;
            return ImpactDirection(angle);
        } else {
            angle = Math_Atan2(x, z) + 0x60;
        }
    }
    return ImpactDirection(angle);
}

static inline u32 flagBits(u32 *flags, u32 mask)
{
    u32 bits = mask;
    return *flags & bits;
}

void Player_ApplyContactResponse(struct Player *player, struct ContactCollider *source,
                                 struct ContactCollider *target)
{
    s16 clearSpecial = 0;
    u16 *response;
    u16 *sourceResponse;
    u16 magnitude;
    u16 *targetResponse;
    s32 clearValue;
    if (player->positionResetState == 1) {
        target->responseParameter = 0;
    }
    sourceResponse = &source->responseParameter;
    magnitude = *sourceResponse;
    targetResponse = &target->responseParameter;
    clearValue = clearSpecial;
    *targetResponse = magnitude;
    magnitude = *sourceResponse;
    response = targetResponse;
    if (magnitude > 200) {
        *response = 200;
    }
    if (flagBits(&source->reactionFlags, 0x40000)) {
        player->contactSpecial = 1;
    } else {
        if (player->tetherMode == 5) {
            u32 flags = target->flags;
            target->flags = flags | 4;
            *response = 0;
            return;
        }
        if (flagBits(&player->statusFlags, 0x8000)) {
            s32 direction;
            if (source->flags & 0x100) {
                direction = ((source->direction + 0x70) >> 5) & 7;
                player->contactDirection = direction;
            } else {
                s32 deltaX = source->x - target->x;
                s32 deltaZ = source->z - target->z;
                if (deltaX == 0 && deltaZ == 0) {
                    player->contactDirection = player->facing;
                } else {
                    direction = (((u16)Math_Atan2(deltaX, deltaZ) + 0x70) >> 5) & 7;
                    player->contactDirection = (direction + 4) & 7;
                }
            }
            player->contactMagnitude = *response;
            *response = 0;
            target->flags |= 4;
            return;
        }
        player->contactSpecial = clearValue;
    }
    player->facing = ActorDirection_ComputeRelativeCardinal(source, target);
}

s32 Player_IsActionBlocked(struct Player *player)
{
    s32 phase;
    if (((struct GameState *)(player->timedEventSlot * sizeof(s32) + (u32)gGameState))
                ->counters628[0] > 0 ||
        (player != 0 && HasFlagBits(&player->statBonusFlags, 0x4000) != 0) ||
        ((phase = SunClock_GetPhase()), (u32)(phase - 4) <= 1 || phase == 0))
        return 1;
    return 0;
}

void Player_CallEventScript(struct Player *player, u32 value)
{
    struct PlayerEventCall call;
    u32 procedure = player->eventProcedure;
    if (procedure != 0) {
        struct PlayerEventArguments *arguments;
        u32 countMask;
        countMask = 0xFFFF0000;
        call.arguments.header = (call.arguments.header & countMask) | 1;
        call.value = value;
        arguments = &call.arguments;
        arguments->values = &call;
        Script_Call(procedure, arguments);
    }
}

/* Player_AddEventFlags: or bits into the player event flags.
 * Address spelling retained for callers outside this unit. */

void sub_0813B3AC(struct Player *player, u32 bits)
{
    player->eventFlags = bits | player->eventFlags;
}

u16 Player_GetEventFlags(struct Player *player, u16 mask)
{
    return player->eventFlags & mask;
}

/* Player_CallCompletionScript: call the configured completion procedure when present.
 * Address spelling retained for callers outside this unit. */

void sub_0813B3CC(struct Player *player)
{
    if (player->completionProcedure != 0)
        Script_Call(player->completionProcedure, 0);
}

s32 Player_GetSelectedSlotActionScale(void)
{
    s32 scale;
    s32 selectedAction =
        ((struct GameState *)((u32)gGameState + gGameState->selected * sizeof(s16)))->slots88[0];
    scale = 4;
    if (selectedAction > 8) {
        scale = 0xc;
        if (selectedAction <= 0xa)
            scale = 6;
    }
    return scale;
}

static inline u32 ResponseFlags_Test(const u32 *flags, u32 mask)
{
    return *flags & mask;
}

static inline u32 ResponseActor_GetHalfFlag(struct Player *actor)
{
    u32 flags;

    if (!actor)
        return 0;
    flags = ResponseFlags_Test(&actor->actionFlags, 0x1000);
    return flags & 0x1000;
}

s32 ActorResponse_Apply(struct Player *actor, s32 value)
{
    if (actor && ResponseFlags_Test(&actor->actionFlags, 0x1000)) {
        if (ResponseFlags_Test(&actor->actionFlags, 0x800))
            return value;
    }
    if (ResponseActor_GetHalfFlag(actor))
        value -= Div(value, 2);
    else if (actor && ResponseFlags_Test(&actor->actionFlags, 0x800))
        value <<= 1;
    return value;
}

s32 ActorFlags_SelectResponse(void *actor)
{
    s32 flags = ActorFlags_GetResponseBits(actor);
    s32 response;
    if (flags & 1)
        response = 5;
    else if (flags & 4)
        response = 10;
    else if (flags & 8)
        response = 10;
    else if (flags & 0x10)
        response = 10;
    else if (flags & 0x20)
        response = 10;
    else
        response = 0;
    response = ActorResponse_Apply(actor, response);
    return response;
}

static inline void RecalculateBaseStats(struct Player *player)
{
    struct GameState *state = gGameState;
    s32 idx;
    for (idx = 0; idx < 3; idx++) {
        u16 *out = &player->derivedStats[idx];
        *out = state->baseStats[idx] + state->statBonuses[idx];
        if (*out > 120)
            *out = 120;
    }
}

void Player_RecalculateDerivedStats(struct Player *player)
{
    s32 i;
    if (player->actionMode != 6) {
        RecalculateBaseStats(player);
    } else {
        s32 level = gGameState->level + 10;
        s32 remainder, quotient, value;
        u16 *dest;
        const struct GameState *statCursor;
        u16 cap;
        if (level > 99)
            level = 99;
        remainder = level - 1;
        remainder *= 2;
        quotient = Div(remainder, 3);
        remainder -= quotient * 3;
        value = quotient + 10;
        dest = player->derivedStats;
        statCursor = gGameState;
        cap = 120;
        for (i = 0; i < 3; i++) {
            *dest = statCursor->statBonuses[0] + value;
            if (remainder > 0) {
                (*dest)++;
                remainder--;
            }
            if (*dest > 120)
                *dest = cap;
            dest++;
            statCursor = (void *)((const u8 *)statCursor + sizeof(statCursor->statBonuses[0]));
        }
    }
}

s32 Player_GetMaxLife(struct Player *player)
{
    s32 lifeStat;
    /* Keep the stat-state base used by the original signed bonus loads. */
    if (gEngineFlags & 0x1000)
        return gGameState->lifeMax;
    {
        u8 *statState = player->actionFlagState;
        lifeStat =
            *(s16 *)(statState + ((u32) & ((struct Player *)0)->menuSnapshotValues[0] - (u32) &
                                  ((struct Player *)0)->actionFlagState)) +
            player->derivedStats[0];
    }
    if (lifeStat > 0x78)
        lifeStat = 0x78;
    return lifeStat * 10;
}

s32 Player_GetMaxEnergy(struct Player *player)
{
    u8 *statState = player->actionFlagState;
    s32 energyStat;
    /* Keep the stat-state base used by the original signed bonus loads. */
    if (gEngineFlags & 0x1000)
        return 0xc;
    energyStat = *(s16 *)(statState + ((u32) & ((struct Player *)0)->menuSnapshotValues[1] - (u32) &
                                       ((struct Player *)0)->actionFlagState)) +
                 player->derivedStats[1];
    if (energyStat > 0x78)
        energyStat = 0x78;
    return energyStat * 5 + 100;
}

void Player_ClampStatsAndSyncStatus(struct Player *player)
{
    s32 v;
    v = Player_GetMaxLife(player);
    player->maximumStatA = v;
    if (player->currentStatA > (u16)v)
        player->currentStatA = v;
    v = Player_GetMaxEnergy(player);
    player->maximumStatB = v;
    if (player->currentStatB > (u16)v)
        player->currentStatB = v;
    player->statWhole = Div(player->statHundredths, 100);
    gGameState->playerStatusValue = player->statusValue;
}

void Player_InitializeStatsFromScript(struct Player *player)
{
    player->maximumStatA = Player_GetMaxLife(player);
    if (Script_GetKeywordValue('l', 0))
        player->currentStatA = player->maximumStatA;
    else if (player->actionMode == 6)
        player->currentStatA = gGameState->alternateStatA;
    else
        player->currentStatA = gGameState->life;
    if (!player->currentStatA)
        player->currentStatA = 1;
    else if (player->currentStatA > player->maximumStatA)
        player->currentStatA = player->maximumStatA;
    player->maximumStatB = Player_GetMaxEnergy(player);
    if (Script_GetKeywordValue('e', 0))
        player->currentStatB = player->maximumStatB;
    else if (player->actionMode == 6)
        player->currentStatB = gGameState->alternateStatB;
    else
        player->currentStatB = gGameState->unk2c;
    if (player->currentStatB > player->maximumStatB)
        player->currentStatB = player->maximumStatB;
    if (Script_GetKeywordValue('t', 0))
        player->statHundredths = 1000;
    else
        player->statHundredths = gGameState->statHundredths;
    player->statusValue = 0;
    player->statScale = 64;
}

static inline s32 IsSpecialScene(struct Player *player)
{
    u32 active = player->active;
    s32 result = 0;
    if (active) {
        if (player->sceneMode == 8)
            result = 1;
    }
    return result;
}

void Player_RefreshBodyCollision(struct Player *player)
{
    u32 flags;
    u32 category;
    /* The level and defense caps also occur in Boktai 2's player defense routine.
     * This version assigns zero defense to every form and passes zero to the collider. */
    s32 defense;
    if (player->actionMode == 6) {
        s32 level = gGameState->level + 10;
        if (level > 99)
            level = 99;
        defense = PLAYER_BODY_DEFENSE;
        flags = player->bodyCollisionFlags | 2;
        category = 1;
    } else if (player->actionMode == 0) {
        defense = PLAYER_BODY_DEFENSE;
        flags = player->bodyCollisionFlags | 1;
        category = 2;
    } else {
        defense = PLAYER_BODY_DEFENSE;
        flags = player->bodyCollisionFlags | 2;
        category = 1;
    }
    if ((player && IsSpecialScene(player)) || player->bodyCollisionFlags == 0x40)
        flags = 0x40;
    if (defense > 99)
        defense = 99;
    Collider_SetBodyProperties(&player->collider, PLAYER_BODY_DEFENSE, flags, category);
}

void Player_InitVitalsAndCollisionFromScript(struct Player *player)
{
    Player_InitializeStatsFromScript(player);
    Player_RefreshBodyCollision(player);
}

void Player_RefreshVitalsAndCollision(struct Player *player)
{
    Player_ClampStatsAndSyncStatus(player);
    Player_RefreshBodyCollision(player);
}

void Player_RefreshStatsAndVitals(struct Player *player)
{
    Player_RecalculateDerivedStats(player);
    Player_ClampStatsAndSyncStatus(player);
    Player_RefreshBodyCollision(player);
    PlayerAction_RefreshModifiers(player);
}

void SceneActor_SpawnModeParticles(struct Player *player)
{
    struct SceneExtent extent;
    struct SceneVec position;
    if ((u8)(player->actionMode - 3) <= 1) {
        position = player->colliderPosition;
        extent.x = 32;
        extent.y = 32;
        extent.z = 32;
    } else {
        position = player->position;
        position.y += 80;
        extent.x = 64;
        extent.y = 60;
        extent.z = 64;
    }
    SparkParticle_Spawn(&position, 1, &extent, 0, 0, 12, 2, 2);
}

/* Player_UpdateTimedEffect: advance and hide the short timed player effect.
 * Address spelling retained for callers outside this unit. */

void sub_0813B950(struct Player *player)
{
    if (player->visualEffects748[0].active) {
        if (++player->visualEffects748[0].ticks > 5) {
            player->visualEffects748[0].sprite.flags |= 1;
            player->visualEffects748[0].active = 0;
        } else
            SpriteDrawable_SelectTileFrame(
                &player->visualEffects748[0].sprite, player->timedEffectGraphics,
                player->visualEffects748[0].baseFrame + (player->visualEffects748[0].ticks >> 2));
    }
}

void HiddenEffect_StartAtPosition(struct Player *player, union PlayerEffectPosition *position,
                                  s32 alternate)
{
    struct PlayerEffectSprite *sprite = &player->visualEffects748[0].sprite;
    if (alternate)
        player->visualEffects748[0].baseFrame = 0x48;
    else
        player->visualEffects748[0].baseFrame = 4;
    SpriteDrawable_SelectTileFrame(sprite, player->timedEffectGraphics,
                                   player->visualEffects748[0].baseFrame);
    sprite->position = *position;
    sprite->flags &= ~1;
    player->visualEffects748[0].active = 1;
    player->visualEffects748[0].ticks = 0;
}

void Player_InitTimedEffect(struct Player *player)
{
    struct PlayerEffectSprite *sprite;
    player->timedEffectGraphics = AnimData_FindEntry(0x1c1c);
    sprite = &player->visualEffects748[0].sprite;
    ParticleSprite_Initialize(sprite, player->timedEffectGraphics, 0);
    Effect_SetOrigin(sprite, -4, -4);
    SpriteDrawable_SelectTileFrame(sprite, player->timedEffectGraphics, 4);
    sprite->flags |= 1;
    sprite->mode = 1;
    sprite->size = 20;
    player->visualEffects748[0].active = 0;
    player->visualEffects748[0].ticks = 0;
}

void Player_SetDirectionalEffectFrame(struct PlayerEffectSprite *sprite, void *graphics, u32 frame)
{
    const struct DirectionalEffectFrame *t = &gUnk_086108F8[frame & 7];
    SpriteDrawable_SelectTileFrame(sprite, graphics, t->frame);
    if (t->flipX == 1)
        sprite->flags |= 4;
    else
        sprite->flags &= ~4;
    if (t->flipY == 1)
        sprite->flags |= 8;
    else
        sprite->flags &= ~8;
}

static inline s32 ScaleProjection(s32 value)
{
    s32 v = value * 3;
    if (v >= 0)
        v >>= 5;
    else
        v = -((-v) >> 5);
    return v;
}

static inline void AddEffectOffset(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = (u16)a->x + (u16)b->x;
    out->y = (u16)a->y + (u16)b->y;
    out->z = (u16)a->z + (u16)b->z;
}

inline static void ProjectEffectPosition(struct SceneVec *screen, struct ContactPackedVector *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 depth;
    s32 height;
    s32 y, zscreen;
    screen->x = ScaleProjection(x - z);
    depth = ScaleProjection(x + z);
    height = ScaleProjection(world->y);
    y = depth - height;
    zscreen = depth + height;
    screen->x = (((u16)screen->x) - ((u16)(*(struct SceneVec *)gCameraOrigin).x)) + 120;
    screen->y = ((y) - ((u16)(*(struct SceneVec *)gCameraOrigin).y)) + 80;
    screen->z = (zscreen) - ((u16)(*(struct SceneVec *)gCameraOrigin).z);
}

inline static struct SceneVec *SetEffectOffset(struct SceneVec *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
    return v;
}

/* Player_UpdateDirectionalEffectPair: project and position the two direction-dependent player effects.
 * Address spelling retained for callers outside this unit. */

void sub_0813BAD8(struct Player *player)
{
    struct SceneVec offsets[2];
    struct ContactPackedVector position;
    struct SceneVec projected;
    s32 direction;
    if (player->visualEffects7A0[0].pending != 1) {
        player->visualEffects778[0].flags |= 1;
        player->visualEffects7A0[0].sprite.flags |= 1;
        return;
    }
    player->visualEffects7A0[0].pending = 0;
    player->visualEffects778[0].flags &= ~1;
    player->visualEffects7A0[0].sprite.flags &= ~1;
    direction = (((u8)player->unk2D + 0x70) >> 5) & 7;
    switch (direction) {
        case 1:
            Player_SetDirectionalEffectFrame(&player->visualEffects778[0],
                                             player->visualEffects748[0].pairedGraphics,
                                             15 - ((s16)gGameState->values08[4]));
            Player_SetDirectionalEffectFrame(&player->visualEffects7A0[0].sprite,
                                             player->visualEffects748[0].pairedGraphics,
                                             13 - ((s16)gGameState->values08[4]));
            SetEffectOffset(&offsets[0], -28, -44, 0);
            SetEffectOffset(&offsets[1], 12, -4, 0);
            break;

        case 3:
            Player_SetDirectionalEffectFrame(&player->visualEffects778[0],
                                             player->visualEffects748[0].pairedGraphics,
                                             9 - ((s16)gGameState->values08[4]));
            Player_SetDirectionalEffectFrame(&player->visualEffects7A0[0].sprite,
                                             player->visualEffects748[0].pairedGraphics,
                                             15 - ((s16)gGameState->values08[4]));
            SetEffectOffset(&offsets[0], 16, -32, 0);
            SetEffectOffset(&offsets[1], -24, 8, 0);
            break;

        case 5:
            Player_SetDirectionalEffectFrame(&player->visualEffects778[0],
                                             player->visualEffects748[0].pairedGraphics,
                                             11 - ((s16)gGameState->values08[4]));
            Player_SetDirectionalEffectFrame(&player->visualEffects7A0[0].sprite,
                                             player->visualEffects748[0].pairedGraphics,
                                             9 - ((s16)gGameState->values08[4]));
            SetEffectOffset(&offsets[0], 16, 8, 0);
            SetEffectOffset(&offsets[1], -24, -32, 0);
            break;

        case 7:
            Player_SetDirectionalEffectFrame(&player->visualEffects778[0],
                                             player->visualEffects748[0].pairedGraphics,
                                             13 - ((s16)gGameState->values08[4]));
            Player_SetDirectionalEffectFrame(&player->visualEffects7A0[0].sprite,
                                             player->visualEffects748[0].pairedGraphics,
                                             11 - ((s16)gGameState->values08[4]));
            SetEffectOffset(&offsets[0], -20, -4, 0);
            SetEffectOffset(&offsets[1], 20, -44, 0);
            break;

        default:
            player->visualEffects778[0].flags |= 1;
            player->visualEffects7A0[0].sprite.flags |= 1;
            return;
    }

    position.x = player->position.x;
    position.z = player->position.z;
    position.y = player->position.y;
    ProjectEffectPosition(&projected, &position);
    offsets[0].x -= 4;
    offsets[0].y -= 4;
    offsets[1].x -= 4;
    offsets[1].y -= 4;
    AddEffectOffset(&player->visualEffects778[0].position.coordinates, &projected, &offsets[0]);
    AddEffectOffset(&player->visualEffects7A0[0].sprite.position.coordinates, &projected,
                    &offsets[1]);
    return;
}

void Player_InitDirectionalEffectPair(struct Player *player)
{
    struct PlayerEffectSprite *sprite;
    s32 i;
    player->visualEffects748[0].pairedGraphics = AnimData_FindEntry(0x1c1c);
    sprite = &player->visualEffects778[0];
    for (i = 1; i >= 0; sprite++, i--) {
        ParticleSprite_Initialize(sprite, player->visualEffects748[0].pairedGraphics, 17);
        Effect_SetOrigin(sprite, 4, 4);
        SpriteDrawable_SelectTileFrame(sprite, player->timedEffectGraphics, 74);
        sprite->mode = 1;
        sprite->size = 20;
    }
    player->visualEffects7A0[0].pending = 0;
    player->visualEffects7A0[0].ticks = 0;
}

void Player_UpdateOrbitingEffect(struct Player *player)
{
    if (player->visualEffects7D0[0].active) {
        s32 angle;
        if (player->kind != 1 || !player->scriptStatus[1]) {
            player->visualEffects7D0[0].sprite.flags |= 1;
            player->visualEffects7D0[0].active = 0;
            return;
        }
        angle = (player->visualEffects7D0[0].ticks * 5) & 255;
        player->visualEffects7D0[0].sprite.position.coordinates.x =
            player->position.x + scale12(56 * gUnk_086149C4[(angle + 64) & 255]);
        player->visualEffects7D0[0].sprite.position.unsignedCoordinates.y =
            scale12(70 * gUnk_086149C4[angle]) + 350 + *(u16 *)&player->position.y;
        player->visualEffects7D0[0].sprite.position.coordinates.z =
            player->position.z - scale12(56 * gUnk_086149C4[(angle + 64) & 255]);
        player->visualEffects7D0[0].ticks++;
    } else if (player->kind == 1 && player->scriptStatus[1]) {
        player->visualEffects7D0[0].sprite.position.coordinates = player->position;
        player->visualEffects7D0[0].sprite.position.coordinates.x += 56;
        player->visualEffects7D0[0].sprite.position.coordinates.y += 350;
        player->visualEffects7D0[0].sprite.flags &= ~1;
        SpriteDrawable_SelectTileFrame(&player->visualEffects7D0[0].sprite,
                                       player->visualEffects7A0[0].orbitGraphics, 16);
        player->visualEffects7D0[0].ticks = 0;
        player->visualEffects7D0[0].active = 1;
    }
}

void Player_InitOrbitingEffect(struct Player *player)
{
    struct PlayerEffectSprite *sprite;
    player->visualEffects7A0[0].orbitGraphics = AnimData_FindEntry(0x1c1c);
    sprite = &player->visualEffects7D0[0].sprite;
    ParticleSprite_Initialize(sprite, player->visualEffects7A0[0].orbitGraphics, 0);
    Effect_SetOrigin(sprite, -4, -4);
    SpriteDrawable_SelectTileFrame(sprite, player->visualEffects7A0[0].orbitGraphics, 16);
    sprite->flags |= 1;
    sprite->mode = 1;
    sprite->size = 20;
    player->visualEffects7D0[0].active = 0;
    player->visualEffects7D0[0].ticks = 0;
}

void Player_OnOrbitingEffectIdle(struct PlayerDirectionalEffect *effect)
{
    if (effect->active != 0)
        effect->active = 0;
    else
        effect->sprite.flags |= 1;
}

void DirectionalEffect_GetAnimationParams(u32 mode, u32 *variant, u32 *kind)
{
    switch ((s32)(mode & 3)) {
        case 0:
            *variant = 0;
            break;
        case 1:
            *variant = 1;
            break;
        case 2:
            *variant = 2;
            break;
        default:
            *variant = 1;
            break;
    }
    if (mode <= 2)
        *kind = 0;
    else if (mode - 3 <= 1)
        *kind = 2;
    else if (mode == 5)
        *kind = 3;
    else
        *kind = 1;
}
