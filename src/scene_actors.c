#include "scene_actors.h"

void SceneActors_Register(struct StateActor *actor, u32 slot)
{
    gSceneActorRegistry[slot] = (u8 *)actor;
}

void SceneActors_Unregister(u32 slot)
{
    gSceneActorRegistry[slot] = 0;
}

struct StateActor *SceneActors_GetBySlot(u32 slot)
{
    return (struct StateActor *)gSceneActorRegistry[slot];
}

struct StateActor *SceneActors_FindAvailable(void)
{
    struct StateActor *found = 0;
    struct StateActor **cursor;
    int i = 0;
    cursor = (struct StateActor **)gSceneActorRegistry;
    do {
        struct StateActor *actor;
        actor = *cursor;
        if (actor && !actor->locked) {
            if (found) {
                return found;
            }
            found = actor;
        }
        cursor++;
        i++;
    } while (i < 8);
    return found;
}

int SceneActors_ApplyDamage(struct ContactCollider *contact, int kind, int amount)
{
    struct StateActor *actor = contact->owner;
    int alive = 1;
    if (!kind) {
        int damage = amount - actor->defense;
        if (damage < 0)
            damage = 0;
        actor->health -= damage;
        if (actor->health <= 0) {
            if (actor->damageCallback)
                actor->damageCallback(actor, 1, damage);
            alive = 0;
        } else if (actor->damageCallback) {
            if (!actor->damageCallback(actor, 0, damage))
                actor->health += damage;
        }
    } else if (actor->damageCallback)
        actor->damageCallback(actor, 2, amount);
    return alive;
}

int SceneActors_Initialize(struct StateActor *actor, int id, int slot, void *position,
                           int facingArg)
{
    int firstId = id;
    int facing = facingArg;
    struct SceneActorVector offset;
    u8 *dst = &actor->registrySlot;
    int zero = 0;
    *dst = slot;
    SceneBody_Init(SceneActors_Body(actor), firstId, position, (u8)facing, 5, actor);
    offset.x = 0;
    offset.y = 256;
    offset.z = 0;
    SceneBody_SetPositionOffset(SceneActors_Body(actor), &offset);
    SceneActors_SetFacing(actor, facing);
    actor->savedFacing = facing;
    actor->damageKind = zero;
    actor->defense = zero;
    actor->damageCallback = zero;
    SceneMessages_RegisterReceiver(actor->receiver2, id, 4);
    return 0;
}

int SceneActors_Destroy(struct StateActor *actor)
{
    SceneLink_RemoveNode(&actor->damageEffect);
    SceneMessages_UnregisterReceiver(actor->receiver2);
    SceneBody_Unregister(SceneActors_Body(actor));
    SceneActors_Unregister(actor->registrySlot);
    return 0;
}

int SceneActors_SetHealthLimits(struct StateActor *actor, int current, int maximum, int extra)
{
    actor->maxHealth = maximum;
    actor->health = current;
    actor->extraHealth = extra;
    if (actor->health > actor->maxHealth)
        actor->health = actor->maxHealth;
    {
        int ratio;
        u8 *dst;
        if (actor->maxHealth) {
            ratio = Div(actor->health << 7, actor->maxHealth);
            dst = &actor->healthRatio;
        } else {
            dst = &actor->healthRatio;
            ratio = 0;
        }
        *dst = ratio;
    }
    {
        int *maximumOut = &gGameState->unk61C;
        *maximumOut = actor->maxHealth;
    }
    {
        int *healthOut = &gGameState->unk618;
        *healthOut = actor->health;
    }
    return 0;
}

int SceneActors_SetHealth(struct StateActor *actor, int health)
{
    actor->health = health;
    if (actor->health > actor->maxHealth)
        actor->health = actor->maxHealth;
    else if (actor->health < 0)
        actor->health = 0;
    if (actor->maxHealth)
        actor->healthRatio = Div(actor->health << 7, actor->maxHealth);
    else
        actor->healthRatio = 0;
    if (actor->healthRatio <= 63)
        actor->lowHealth = 1;
    else
        actor->lowHealth = 0;
    return 0;
}

void SceneActors_SetDamageCallback(struct StateActor *actor, int kind,
                                   int (*callback)(void *, int, int), int defense)
{
    actor->damageKind = kind;
    TargetRegistry_RegisterSceneActor(&actor->damageEffect, kind, SceneActors_ApplyDamage);
    actor->damageCallback = callback;
    actor->defense = defense;
}

int SceneActors_SetShake(struct StateActor *actor, int frames, int strength)
{
    actor->shakeFrames = frames;
    actor->shakeStrength = strength;
    return 0;
}

void SceneActors_StartAnimation(struct StateActor *actor, struct SceneActorAnimationTile *animation,
                                int resource, int sequence, int speed, int flags)
{
    actor->animationOffset = 0;
    actor->animationSequence = sequence;
    Anim_SetKeyframes(animation, (void *)resource, actor->animationSequence, (u16)speed,
                      (u16)flags);
    flags &= 1;
    if (!flags)
        actor->animationCompletions = 0;
    if (actor->animationFacing4 > 1) {
        actor->animationMirrored = 1;
        animation->flags |= 4;
    } else {
        actor->animationMirrored = 0;
        animation->flags &= ~4;
    }
}

void SceneActors_StartDirectionalAnimation(struct StateActor *actor,
                                           struct SceneActorAnimationTile *animation, int resource,
                                           int sequence, int speed, int offset, int flags)
{
    int savedFlags = flags;
    flags = savedFlags;
    if (((u8)(actor->animationFacing4 - 1)) <= 1) {
        actor->animationOffset = offset;
    } else {
        actor->animationOffset = 0;
    }
    if (actor->animationFacing4 > 1) {
        actor->animationMirrored = 1;
    } else {
        actor->animationMirrored = 0;
    }
    {
        int selected = actor->animationOffset + sequence;
        actor->animationSequence = selected;
    }
    Anim_SetKeyframes(animation, (void *)resource, actor->animationSequence, (u16)speed,
                      (u16)flags);
    flags &= 1;
    savedFlags = 0;
    if (!flags) {
        actor->animationCompletions = savedFlags;
    }
    if (actor->animationMirrored) {
        animation->flags |= 4;
    } else {
        animation->flags &= ~4;
    }
}

void SceneActors_StartEightDirectionAnimation(struct StateActor *actor,
                                              struct SceneActorAnimationTile *animation,
                                              int resource, int sequence, int speed, int offset,
                                              int flags)
{
    int savedFlags = flags;
    flags = savedFlags;
    {
        int direction = actor->animationFacing8;
        if (((u8)(direction - 5)) <= 2) {
            {
                int value = 8 - direction;
                actor->animationOffset = value;
            }
            actor->animationMirrored = 1;
        } else {
            u8 *dst = &actor->animationOffset;
            int zero = 0;
            *dst = direction;
            actor->animationMirrored = zero;
        }
    }
    savedFlags = 0;
    {
        int selected = actor->animationOffset + sequence;
        actor->animationSequence = selected;
    }
    Anim_SetKeyframes(animation, (void *)resource, actor->animationSequence, (u16)speed,
                      (u16)flags);
    flags &= 1;
    if (!flags) {
        actor->animationCompletions = savedFlags;
    }
    if (actor->animationMirrored) {
        animation->flags |= 4;
    } else {
        animation->flags &= ~4;
    }
}

void SceneActors_StartUnmirroredAnimation(struct StateActor *actor,
                                          struct SceneActorAnimationTile *animation, int resource,
                                          int sequence, int speed, int flags)
{
    actor->animationOffset = 0;
    actor->animationMirrored = 0;
    actor->animationSequence = sequence;
    Anim_SetKeyframes(animation, (void *)resource, actor->animationSequence, (u16)speed,
                      (u16)flags);
    flags &= 1;
    if (!flags) {
        actor->animationCompletions = 0;
    }
    if (actor->animationMirrored) {
        animation->flags |= 4;
    } else {
        animation->flags &= ~4;
    }
}

int SceneActors_UpdateAnimation(struct StateActor *actor, void *animation, int delta)
{
    actor->animationFinished = Anim_Step(animation, (void *)delta);
    {
        int sequence = actor->animationSequence;
        actor->previousAnimationSequence = sequence;
    }
    if (actor->animationFinished)
        actor->animationCompletions++;
    return 0;
}

void SceneActors_UpdateTargetMetrics(struct StateActor *actor)
{
    int x, z;
    struct SceneActorBody *body;
    actor->actionAnimation = 0;
    actor->targetMetricsValid = 0;
    body = SceneActors_Body(actor);
    x = actor->targetPosition.x - body->position.x;
    z = actor->targetPosition.z - body->position.z;
    actor->targetHeading = (u8)Math_Atan2(x, z);
    actor->targetDistanceSquared = x * x + z * z;
    actor->targetMetricsValid = 1;
    if (actor->maxHealth)
        actor->healthRatio = Div(actor->health << 7, actor->maxHealth);
    else
        actor->healthRatio = 0;
    if (actor->healthRatio <= 63)
        actor->lowHealth = 1;
    else
        actor->lowHealth = 0;
}

void SceneActors_UpdateEffects(struct StateActor *actor)
{
    struct StateActorDamageEffect *damage;
    if (actor->health > 0)
        gGameState->unk618 = actor->health;
    else
        gGameState->unk618 = 0;
    if (actor->shakeFrames) {
        Camera_RequestShake(actor->shakeStrength);
        actor->shakeFrames--;
    }
    if (actor->ambientParticle && (actor->effectTimer & 3) == 0)
        SceneActors_SpawnAmbientParticle(actor);
    if (actor->intermittentEffect && (actor->effectTimer & 3) <= 1) {
        u32 effectFlag = 0x10;
        actor->actionFlags = effectFlag | actor->actionFlags;
    }
    if (actor->continuousEffect) {
        u32 effectFlag = 0x10;
        actor->actionFlags = effectFlag | actor->actionFlags;
    }
    {
        u32 disableDamage = 0x40;
        if (disableDamage & actor->actionFlags) {
            damage = &actor->damageEffect;
            {
                u32 clear = ~1u;
                damage->flags = clear & damage->flags;
            }
        } else {
            damage = &actor->damageEffect;
            {
                u32 set = 1;
                damage->flags = set | damage->flags;
            }
        }
    }
}

void SceneActors_RunStateCallback(struct StateActor *actor)
{
    int tick;
    actor->actionFlags = 0;
    actor->actionFinished = 0;
    tick = (actor->callbackFrame)++;
    if (actor->actionCallback)
        actor->actionCallback(actor, tick);
}

void SceneActors_SetEventScripts(struct StateActor *actor, u32 first, u32 second)
{
    actor->firstEventScript = first;
    actor->secondEventScript = second;
}

void SceneActors_RunFirstEventScript(struct StateActor *actor, s16 *position)
{
    int args[8];
    struct ScriptUtilityArguments call;
    u32 script = actor->firstEventScript;
    if (script) {
        int id = script;
        int mask = 4;
        if (!(gUnk_02000560 & mask)) {
            int *in, *out, i;
            struct ScriptUtilityArguments *packet;
            args[0] = position[0];
            args[1] = position[1];
            args[2] = position[2];
            packet = &call;
            in = actor->eventArguments;
            out = args + 3;
            i = 3;
            do {
                *out++ = *in++;
                i--;
            } while (i >= 0);
            {
                u32 mask = 0xffff0000;
                u32 header;
                header = *(u32 *)&call;
                header &= mask;
                header |= 7;
                *(u32 *)&call = header;
            }
            packet->values = args;
            Script_Call(id, packet);
        }
    }
}

void SceneActors_RunSecondEventScript(struct StateActor *actor, s16 *position)
{
    int args[8];
    struct ScriptUtilityArguments call;
    u32 script = actor->secondEventScript;
    if (script) {
        int id = script;
        int mask = 4;
        if (!(gUnk_02000560 & mask)) {
            int *in, *out, i;
            struct ScriptUtilityArguments *packet;
            args[0] = position[0];
            args[1] = position[1];
            args[2] = position[2];
            packet = &call;
            in = actor->eventArguments;
            out = args + 3;
            i = 3;
            do {
                *out++ = *in++;
                i--;
            } while (i >= 0);
            {
                u32 mask = 0xffff0000;
                u32 header;
                header = *(u32 *)&call;
                header &= mask;
                header |= 7;
                *(u32 *)&call = header;
            }
            packet->values = args;
            Script_Call(id, packet);
        }
    }
}

int SceneActors_CopyAvailablePosition(int *out)
{
    struct StateActor *actor = SceneActors_FindAvailable();
    int result;
    if (actor) {
        *(struct StateActorPos *)out = actor->target;
        result = 0;
    } else
        result = -1;
    return result;
}

struct StateActorPos *SceneActors_GetAvailablePosition(void)
{
    struct StateActor *actor = SceneActors_FindAvailable();
    if (!actor)
        return 0;
    return &actor->target;
}

void SceneActors_ConfigureEffect(struct StateActor *actor, unsigned mode, int first, int second)
{
    switch (mode) {
        case 0:
            actor->attachmentEffect = ActorAttachment_UpdateFromDescriptor(
                actor->attachmentEffect, (struct SceneActorBody *)&actor->bodyId, 0, 1, 0, 0, 0,
                100);
            Sound_PlaySong(0x1b1);
            break;
        case 2:
            actor->attachmentEffect = ActorAttachment_UpdateFromDescriptor(
                actor->attachmentEffect, (struct SceneActorBody *)&actor->bodyId, 0, 0, 0, 0, 0,
                100);
            Sound_PlaySong(0x1ec);
            break;
        case 1:
            actor->attachmentEffect = ActorAttachment_UpdateFromDescriptor(
                actor->attachmentEffect, (struct SceneActorBody *)&actor->bodyId, 0, 3, 0, 0, 0,
                100);
            Sound_PlaySong(0x1ec);
            break;
        case 3:
            actor->attachmentEffect = ActorAttachment_UpdateFromDescriptor(
                actor->attachmentEffect, (struct SceneActorBody *)&actor->bodyId, 0, 2, 0, 0, 0,
                100);
            Sound_PlaySong(0x1ed);
            break;
        case 4: {
            u16 *out = &actor->effectParameter;
            int zero = 0;
            *out = first;
            actor->effectDuration = second;
            actor->attachmentEffect = ActorAttachment_UpdateFromDescriptor(
                actor->attachmentEffect, (struct SceneActorBody *)&actor->bodyId, 1, zero, out,
                actor->effectDuration, zero, 100);
            break;
        }
        case 5: {
            u16 *out = &actor->effectParameter;
            int zero = 0;
            *out = first;
            actor->effectDuration = second;
            actor->attachmentEffect = ActorAttachment_UpdateFromDescriptor(
                actor->attachmentEffect, (struct SceneActorBody *)&actor->bodyId, 3, zero, out,
                actor->effectDuration, zero, 100);
            break;
        }
        case 6:
            actor->attachmentEffect = ActorAttachment_UpdateFromDescriptor(
                actor->attachmentEffect, (struct SceneActorBody *)&actor->bodyId, 5, 0, 0, 0, 0,
                100);
            break;
        case 7:
            actor->attachmentEffect = ActorAttachment_UpdateFromDescriptor(
                actor->attachmentEffect, (struct SceneActorBody *)&actor->bodyId, 4, 0, 0, 0, 0,
                100);
            break;
        case 8:
        default:
            break;
    }
}

void SceneActors_SpawnAmbientParticle(struct StateActor *actor)
{
    s32 horizontalMask;
    struct SceneActorVector position;
    int r;
    int cursor;
    u16 *table = gRandomTable;
    *(&cursor) = gRandomIndex + 1;
    cursor &= 0x3ff;
    r = table[cursor] & (horizontalMask = 0x7f);
    position.x = actor->target.x + r;
    cursor = (cursor + 1) & 0x3ff;
    gRandomIndex = cursor;
    cursor = table[cursor];
    {
        u16 *y = (u16 *)&actor->target.y;
        cursor &= 0x1f;
        {
            int height = *y;
            cursor += height;
        }
        position.y = cursor;
    }
    {
        u16 *z = (u16 *)&actor->target.z;
        r -= 0x7f;
        position.z = *z - r;
    }
    AnimatedParticles_EmitBurst(&position, 1, 0, 0x40, 12, 12, 4, 4);
}

void SceneActors_ScriptSetEffects(void)
{
    struct StateActor *actor = SceneActors_FindAvailable();
    if (actor) {
        if (Script_SeekToKeyword('s')) {
            actor->ambientParticle = 1;
            Sound_PlaySong(0x1c3);
        }
        if (Script_SeekToKeyword('e'))
            actor->ambientParticle = 0;
        if (Script_SeekToKeyword('b'))
            actor->intermittentEffect = 1;
        if (Script_SeekToKeyword('i'))
            actor->continuousEffect = 1;
    }
}
