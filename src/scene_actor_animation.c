#include "scene_actor_actions.h"

struct ContactPosition *TetherActor_GetPosition(void);

#include "scene_actor_lifecycle.h"

#include "global.h"

#include "engine.h"

#include "display_animation.h"

#include "ram.h"

#include "input.h"

#include "player.h"

#include "scene_facing.h"

#include "engine_globals.h"

#include "scene_actor_route.h"

#include "display_keyframe.h"

u8 *TerrainModifiers_FindCellOverride(int, int);

void Obj_StepPosition(void *);

static inline u8 *SceneActor_GetTerrainCell(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}

int Actor_UpdateMovementAnimationAndEffects(void *unused, struct SceneActor *actor)
{
    s16 k2 = 0;
    struct SceneActorBody *body = &actor->body;
    void *drawable = &actor->drawable.common.kind;
    Obj_StepPosition(body);
    {
        u32 mask = 0x400;
        if (!(actor->flags & mask)) {
            struct ContactPosition *pos = &body->position;
            int x = pos->x >> 8, z = pos->z >> 8, index, SceneActor_TerrainHeight, slope;
            u8 *c;
            if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) ||
                (((u32)z) >= gTerrainHeight)) {
                index = 0;
            } else {
                index = gTerrainMap->rows[z] + x;
            }
            c = SceneActor_GetTerrainCell(index);
            slope = *c >> 4;
            SceneActor_TerrainHeight = (*c & 15) << 8;
            switch (slope) {
                case 1:
                    SceneActor_TerrainHeight -= (u8)pos->z;
                    break;
                case 2:
                    SceneActor_TerrainHeight -= (u8)pos->x;
                    break;
            }
            body->position.y = SceneActor_TerrainHeight;
        }
    }
    DisplayAnimation_SetPosition(drawable, (struct DisplayPosition *)&body->position);
    DisplayAnimation_UpdatePlayback(drawable);
    if (actor->timer > 0) {
        actor->timer--;
        if (actor->timer <= 3) {
            if (actor->flashing && (!(gEngineFlags & 0x200))) {
                actor->timer = actor->duration;
            } else {
                s16 *timer = &actor->timer;
                int zero;
                zero = k2;
                *timer = 0;
                Actor_SetAttachmentEffect(actor, 8, 180, 180);
                actor->flashing = zero;
            }
        }
    }
}

/* Actor_NoopUpdate: Empty unused hook provisionally associated with actor updates */

void Actor_NoopUpdate(void)
{
}

/* Actor_NoopRender: Empty unused hook provisionally associated with actor rendering */

void Actor_NoopRender(void)
{
}

void sub_0821DE30(void *);

int DisplayAnimation_SetFlags(void *, int);

int Actor_UpdateVisibilityAndShadow(void *unused, struct SceneActor *actor)
{
    int result;
    actor->positionLink.id = actor->body.id;
    actor->positionLink.position = &actor->body.position;
    sub_0821DE30(&actor->positionLink.id);
    {
        u32 mask = 0x100;
        if (actor->flags & mask) {
            DisplayAnimation_SetFlags(&actor->drawable.common.kind, 1);
            result = Actor_DisableShadow(actor);
        } else {
            DisplayAnimation_ClearFlags(&actor->drawable.common.kind, 1);
            {
                u32 mask = 0x200;
                if (actor->flags & mask)
                    result = Actor_DisableShadow(actor);
                else
                    result = Actor_EnableShadow(actor);
            }
        }
    }
    return result;
}

int Actor_SetMirroredFourWayAnimation(struct SceneActor *actor, int a, int b, int stride, int c,
                                      int d)
{
    void *draw = &actor->drawable.common.kind;
    int direction, flip;
    if (actor->drawable.common.kind == 1) {
        if (actor->facing4 > 1) {
            direction = 3 - actor->facing4;
            flip = 1;
        } else {
            direction = actor->facing4;
            flip = 0;
        }
    } else {
        u32 facing = actor->facing4;
        u8 range = facing - 1;
        direction = 0;
        if (range <= 1)
            direction = stride;
        flip = 0;
        if (facing > 1)
            flip = 1;
    }
    return DisplayAnimation_SetEntry(draw, a, direction, flip, b, c, d);
}

int Actor_SetFourWayAnimation(struct SceneActor *actor, int a, int b, int c, int d, int e)
{
    void *draw = &actor->drawable.common.kind;
    u32 direction, flip;
    if (actor->drawable.common.kind == 1) {
        direction = actor->facing4;
        flip = 0;
        if (direction > 1)
            flip = 1;
    } else {
        u32 facing;
        direction = 0;
        facing = actor->facing4;
        flip = 0;
        if (facing > 1)
            flip = 1;
    }
    return DisplayAnimation_SetEntry(draw, a, direction, flip, b, d, e);
}

int Actor_SetMirroredEightWayAnimation(struct SceneActor *actor, int a, int b, int stride, int c,
                                       int d)
{
    int direction, flip;
    if (actor->drawable.common.kind == 1) {
        if (actor->facing8 > 4) {
            direction = 8 - actor->facing8;
            flip = 1;
        } else {
            direction = actor->facing8;
            flip = 0;
        }
    } else {
        if (actor->facing8 > 4) {
            direction = 8 - actor->facing8;
            flip = 1;
        } else {
            direction = actor->facing8;
            flip = 0;
        }
        actor->facing8 = actor->facing8 * stride;
    }
    return DisplayAnimation_SetEntry(&actor->drawable.common, a, direction, flip, b, c, d);
}

int Actor_SetFrontBackAnimation(struct SceneActor *actor, int a, int b, int stride, int c, int d)
{
    int direction, flip;
    if (actor->drawable.common.kind == 1) {
        u32 facing = actor->facing8;
        direction = 0;
        if (facing == 4)
            direction = stride;
        flip = 0;
        if (facing > 4)
            flip = 1;
    } else {
        u32 facing = actor->facing8;
        direction = 0;
        if (facing == 4)
            direction = stride;
        flip = 0;
        if (facing > 4)
            flip = 1;
    }
    return DisplayAnimation_SetEntry(&actor->drawable.common, a, direction, flip, b, c, d);
}

u32 Actor_SetUndirectedAnimation(struct SceneActor *actor, u32 x, u32 y, u32 z, u32 s0, u32 s1)
{
    return DisplayAnimation_SetEntry(&actor->drawable.common, x, 0, 0, y, s0, s1);
}

int Actor_PlayAnimationEntry(void *object, int id, int a, int b, int c)
{
    struct SceneActor *actor = object;
    const struct SceneActorAnimationEntry *entry = &actor->animationEntries[id];
    int result;
    if (entry->kind == 1)
        result = Actor_SetFourWayAnimation(actor, entry->animation, a, entry->stride, b, c);
    else if (entry->kind == 3)
        result = Actor_SetMirroredFourWayAnimation(actor, entry->animation, a, entry->stride, b, c);
    else if (entry->kind == 2)
        result = Actor_SetFrontBackAnimation(actor, entry->animation, a, entry->stride, b, c);
    else if (entry->kind == 4)
        result =
            Actor_SetMirroredEightWayAnimation(actor, entry->animation, a, entry->stride, b, c);
    else
        result = Actor_SetUndirectedAnimation(actor, entry->animation, a, entry->stride, b, c);
    return result;
}

/* Actor_PlayFootstepSound: plays a step sound when the animation hits one of two marked frames */

struct StepSounds {
    u16 a, b;
};

extern const struct StepSounds gUnk_086055D4[];

void Actor_PlayFootstepSound(struct SceneActor *actor, u32 mode)
{
    u32 frame;
    u32 idx;
    if (!actor->soundEnabled)
        return;
    if (mode == 1)
        idx = 0;
    else if (mode == 3)
        idx = 2;
    else
        return;
    if (DisplayAnimation_GetFrameBoundary(&actor->drawable.common, &frame)) {
        const struct SceneActorResourceEntry *info = actor->resourceEntry;
        const s8 *fr = info->payload.footstepFrames;
        const s8 *f = fr + idx;
        s32 f0 = f[0];
        if (f0 == frame)
            Sound_PlaySong(gUnk_086055D4[info->sound].a);
        else if (f[1] == frame) {
            const u16 *sounds = (const u16 *)gUnk_086055D4;
            u32 index = info->sound * sizeof(struct StepSounds);
            sounds += 1;
            Sound_PlaySong(*(const u16 *)((const u8 *)sounds + index));
        }
    }
}

/* Actor_IsWithinHorizontalRange: Tests horizontal axis distances against a range with a fixed vertical tolerance */

static inline s32 Abs(s32 v)
{
    if (v < 0)
        v = -v;
    return v;
}

u32 Actor_IsWithinHorizontalRange(struct ContactPosition *a, struct ContactPosition *b, s32 range)
{
    if (Abs(b->x - a->x) < range && Abs(b->y - a->y) <= 255 && Abs(b->z - a->z) < range)
        return 1;
    return 0;
}

void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, void *, int, int, int);

void Actor_SetAttachmentEffect(struct SceneActor *actor, unsigned kind, int timer, int duration)
{
    switch (kind) {
        case 0:
            actor->effectKind = 0;
            actor->attachment = ActorAttachment_UpdateFromDescriptor(
                actor->attachment, &actor->body, 0, 1, 0, 0, 0, 100);
            Sound_PlaySong(0x1b1);
            break;
        case 2:
            actor->effectKind = 0;
            actor->attachment = ActorAttachment_UpdateFromDescriptor(
                actor->attachment, &actor->body, 0, 0, 0, 0, 0, 100);
            Sound_PlaySong(0x1ec);
            break;
        case 1:
            actor->effectKind = 0;
            actor->attachment = ActorAttachment_UpdateFromDescriptor(
                actor->attachment, &actor->body, 0, 3, 0, 0, 0, 100);
            Sound_PlaySong(0x1ec);
            break;
        case 3:
            actor->effectKind = 0;
            actor->attachment = ActorAttachment_UpdateFromDescriptor(
                actor->attachment, &actor->body, 0, 2, 0, 0, 0, 100);
            Sound_PlaySong(0x1ed);
            break;
        case 4:
            actor->effectKind = 1;
            actor->timer = timer;
            actor->duration = duration;
            actor->attachment = ActorAttachment_UpdateFromDescriptor(
                actor->attachment, &actor->body, 1, 0, &actor->timer, actor->duration, 0, 100);
            break;
        case 5:
            actor->effectKind = 3;
            actor->timer = timer;
            actor->duration = duration;
            actor->attachment = ActorAttachment_UpdateFromDescriptor(
                actor->attachment, &actor->body, 3, 0, &actor->timer, actor->duration, 0, 100);
            break;
        case 6:
            actor->effectKind = 5;
            actor->attachment = ActorAttachment_UpdateFromDescriptor(
                actor->attachment, &actor->body, 5, 0, 0, 0, 0, 100);
            break;
        case 7:
            actor->effectKind = 4;
            actor->attachment = ActorAttachment_UpdateFromDescriptor(
                actor->attachment, &actor->body, 4, 0, 0, 0, 0, 100);
            break;
        case 8:
            if (actor->attachment)
                actor->attachment = ActorAttachment_UpdateFromDescriptor(
                    actor->attachment, &actor->body, (u8)actor->effectKind, 127, 0, 0, 0, 100);
            break;
    }
}

extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);

void Terrain_MoveFootprint(void *, struct ContactPosition *, struct ContactPosition *, u16, u16,
                           u8);

static inline s32 SceneActor_ScaleTrig(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}

static inline void SceneActor_OffsetAlongHeading(const s16 *table, struct ContactPosition *v,
                                                 u32 angle, const s32 radius)
{
    u32 heading = angle;
    s32 length = radius;
    v->x = SceneActor_ScaleTrig(table[(heading + 64) & 255] * length);
    v->y = 0;
    v->z = SceneActor_ScaleTrig(table[heading & 255] * length);
}

static inline void Subtract(struct ContactPosition *out, struct ContactPosition *a,
                            struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline u32 SceneActor_TestFlags(u16 *field, u32 mask)
{
    return *field & mask;
}

// confidence=low: emits toward flagged nearby actors using an outward radial offset.

static inline void Add(struct ContactPosition *out, struct ContactPosition *a,
                       struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = (u16)b->y + (u16)a->y;
    out->z = b->z + a->z;
}

void Actor_EmitTowardNearbyActors(struct SceneActorBody *emitter)
{
    struct SceneActor *actor;
    struct ContactPosition offset;
    const s16 *table;
    if ((!((struct SceneActorManager *)gSceneActorListManager)) || (gEngineFlags & 0x200))
        return;
    actor = ((struct SceneActorManager *)gSceneActorListManager)->head;
    if (!actor)
        return;
    table = gUnk_086149C4;
    do {
        if (SceneActor_TestFlags(&actor->flags, 0x1000)) {
            struct ContactPosition *position = &actor->body.position;
            struct ContactPosition *origin = &emitter->position;
            s32 distance;
            s32 dx = origin->x - position->x;
            s32 dz = origin->z - position->z;
            distance = (s32)((((u32)dx) * dx) + (((u32)dz) * dz));
            if (distance <= 9999) {
                SceneActor_OffsetAlongHeading(table, &offset, Math_Atan2(dx, dz), 100);
                Add(&offset, position, &offset);
                Subtract(&emitter->motion, &offset, &emitter->position);
                if (emitter->movementProbe)
                    Terrain_MoveFootprint(emitter->movementProbe, &emitter->position,
                                          &emitter->motion, emitter->probeExtentX,
                                          emitter->probeExtentZ, emitter->mode);
            }
        }
        actor = actor->previous;
    } while (actor);
}

s32 Scene_CanOpenMenu(void);

static inline u32 Flag(u32 *p, u32 mask)
{
    return *p & mask;
}

static inline s32 PlayerKind(void)
{
    return gPlayers[0]->tetherMode;
}

// confidence=low: highlights the nearest eligible actor and invokes its script on input.

// The mask as an inline argument matches (found by tools/fix.py).

static inline u32 ActorFlag(u32 mask, struct SceneActor *actor)
{
    return actor->flags & mask;
}

static inline s32 TargetAllowsInteraction(struct Player *p)
{
    if ((u8)(p->actionMode - 3) <= 2)
        return 0;
    if (Flag(&p->statusFlags, 0x100000))
        return 0;
    if (p->active && (u16)(p->sceneMode - 5) <= 1)
        return 0;
    {
        u8 mode = p->tetherMode;
        u8 allow = 1;
        if (mode > 1 && mode != 7)
            allow = 0;
        return allow;
    }
}

void Actor_UpdateInteractionCue(struct SceneActorManager *input)
{
    struct SceneActorManager *context = input;
    s32 buttons = gPadState[1];
    s32 pressed = 1;
    s32 kind;
    struct SceneActor *nearest;
    struct SceneActor *actor;
    s32 playerKind;
    {
        s32 scratch = pressed;
        scratch &= buttons;
        pressed = scratch;
    }
    playerKind = PlayerKind();
    if (playerKind == 10)
        goto disableInput;
    if (playerKind != 11)
        goto inputReady;
disableInput:
    pressed = 0;
inputReady:;
    {
        if (!context->player)
            goto blocked;
        if (!TargetAllowsInteraction(context->player))
            goto blocked;
        {
            u8 state = context->player->actionMode;
            kind = 0;
            if (state == 0)
                goto selected;
            kind = 1;
            if (state == 1)
                goto selected;
            kind = 3;
            if (state != 6)
                goto selected;
            kind = 2;
            goto selected;
        }
    }
blocked:
    kind = 3;
selected:
    nearest = 0;
    if (Scene_CanOpenMenu()) {
        s32 best = 1000000;
        actor = context->head;
        while (actor) {
            if (ActorFlag(0x800, actor) && actor->script) {
                struct ContactPosition *position = &actor->body.position;
                struct ContactPosition *target = context->target;
                s32 x = target->x - position->x;
                s32 z = target->z - position->z;
                s32 distance = (s32)((u32)x * x + (u32)z * z);
                if (distance < best) {
                    best = distance;
                    nearest = actor;
                }
            }
            actor = actor->previous;
        }
    }
    if (nearest && !pressed) {
        actor = context->head;
        if (actor) {
            s8 *base = nearest->interaction.cues;
            s32 index;
            s8 *cue;
            u32 blockedMask;
            index = kind;
            cue = base + index;
            blockedMask = 0x2000;

            do {
                if (actor == nearest && !(nearest->flags & blockedMask)) {
                    if (!nearest->flashing) {
                        Actor_SetAttachmentEffect(nearest, *cue, 180, 180);
                        nearest->flashing = 1;
                    }
                } else if (actor->flashing) {
                    Actor_SetAttachmentEffect(actor, 8, 180, 180);
                    actor->flashing = 0;
                }
                actor = actor->previous;
            } while (actor);
        }
    } else {
        actor = context->head;
        while (actor) {
            if (actor->flashing) {
                Actor_SetAttachmentEffect(actor, 8, 180, 180);
                actor->flashing = 0;
            }
            actor = actor->previous;
        }
    }
    if (nearest && pressed) {
        struct {
            u32 identity;
            s32 x, y, z;
            struct SceneActorParameters parameters;
        } operands;
        operands.identity = nearest->body.id;
        operands.x = nearest->body.position.x;
        operands.y = nearest->body.position.y;
        operands.z = nearest->body.position.z;
        {
            u32 *source = nearest->parameters.values;
            u32 *destination = operands.parameters.values;
            s32 count = 3;
            do {
                *destination++ = *source++;
            } while (--count >= 0);
        }
        // The prepared operands are retained even though dispatch uses a null argument block.
        Script_Call(nearest->script, 0);
    }
}

/* Message packet, as sent by SceneMessages_Send (see GridAttackActor_NotifyLinkedActorLatePhase). */

/* Two pages of up to eight queued packets; `page` selects the one being filled. */

void SceneMessages_Send(struct SceneActorPacket *);

/* Script: fills the next packet of the current page (-n id, -k kind, -f, -a command,
   -p up to eight arguments), sends it and counts it. The bounds check indexes
   counts[] with the count itself, as the original does. */

s32 Packet_QueueFromScript(void)
{
    struct SceneActorManager *queue = gSceneActorListManager;
    struct SceneActorPacket *packet;
    if (!queue || queue->count[queue->count[queue->buffer]] > 7)
        return -1;
    packet = &queue->packets[queue->buffer][queue->count[queue->buffer]];
    packet->id = Script_GetKeywordValue('n', 0);
    packet->kind = Script_GetKeywordValue('k', 0);
    packet->zero = 0;
    packet->reserved = Script_GetKeywordValue('f', 0);
    packet->command = Script_GetKeywordValue('a', 0);
    packet->count = 0;
    if (Script_SeekToKeyword('p')) {
        while (Script_GetPc()) {
            packet->args[packet->count++] = Script_GetValue();
            if (packet->count > 7)
                break;
        }
    }
    SceneMessages_Send(packet);
    queue->count[queue->buffer]++;
    return 0;
}

int Script_GetKeywordValue(int, int);

static inline void Actor_SetScriptState(struct SceneActor *actor, u8 state)
{
    int zero = 0;
    actor->stage = zero;
    actor->phase = zero;
    actor->state = state;
    actor->ticks = zero;
    actor->commandEntered = 1;
}

int Actor_BeginScriptControl(void)
{
    void *m = gSceneActorListManager;
    struct SceneActor *actor;
    if (!m)
        return -1;
    actor = Actor_FindById(m, Script_GetKeywordValue(0x6e, 0));
    if (!actor)
        return -1;
    if (Script_SeekToKeyword(0x64)) {
        int value = Script_GetValue();
        actor->body.direction = value;
        actor->facing8 = SceneFacing_EightWay(value);
        actor->facing4 = SceneFacing_FourWay(value);
    }
    if (Script_SeekToKeyword(0x72)) {
        actor->routeId = Script_GetValue();
        actor->routeMode = Script_GetValue();
        actor->routePoint = Script_GetValue();
        SceneBody_StartRoute(&actor->body, &actor->route, (u8)actor->routeId, (u8)actor->routeMode,
                             (u8)actor->routePoint);
        Path_GetPointPosition(&actor->body.position, actor->route.point, actor->routePoint);
    }
    if (Script_SeekToKeyword(0x62)) {
        actor->modeA = Script_GetValue();
        actor->modeB = Script_GetValue();
        actor->modeValue = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x74))
        actor->script = Script_GetValue();
    if (Script_SeekToKeyword(0x48)) {
        actor->interaction.turning.firstDirection = Script_GetValue();
        actor->interaction.turning.secondDirection = Script_GetValue();
        actor->interaction.turning.turnState = 255;
    }
    if (Script_SeekToKeyword(0x75))
        actor->enabled = Script_GetValue();
    if (actor->routeId >= 0)
        Actor_SetScriptState(actor, SceneCharacter_GetPathNodeState(actor));
    else
        Actor_SetScriptState(actor, 6);
    actor->controlled = 1;
    return 0;
}

int Actor_EndScriptControl(void)
{
    void *m = gSceneActorListManager;
    struct SceneActor *actor;
    int mode;
    struct SceneActor **alias;
    if (!m) {
        return -1;
    }
    actor = Actor_FindById(m, Script_GetKeywordValue(0x6e, 0));
    if (!actor) {
        return -1;
    }
    {
        u8 *controlled = &actor->controlled;
        int zero;
        zero = 0;
        *controlled = zero;
        mode = Script_GetKeywordValue(0x69, 0);
        {
            u32 command = mode;
            alias = &actor;
            switch (command) {
                default:
                    break;

                case 0:

                {
                    int one = 1;
                    (*alias)->stage = command;
                    (*alias)->phase = command;
                    actor->state = one;
                    actor->ticks = command;
                    actor->commandEntered = one;
                    break;
                }

                case 1:
                    (*alias)->stage = zero;
                    (*alias)->phase = zero;
                    (*alias)->state = zero;
                    actor->ticks = zero;
                    (*alias)->commandEntered = command;
                    break;
                    return mode;
            }
        }
    }
}

/* Actor_IsInReactionStateFromScript: Tentatively tests whether the script-selected actor has a reaction code from thirteen through fifteen */

u32 Actor_IsInReactionStateFromScript(void)
{
    struct SceneActorManager *g = gSceneActorListManager;
    if (g) {
        struct SceneActor *e = Actor_FindById(g, Script_GetKeywordValue(0x6e, 0));
        if (e && (u8)(e->savedState - 13) <= 2)
            goto yes;
    }
    return 0;
yes:
    return 1;
}

int Actor_InitBehaviorFromScript(void *unused, struct SceneActor *actor)
{
    if (Script_GetKeywordValue(0x43, 1))
        Actor_RegisterMotion(actor);
    {
        u8 angle = actor->body.direction;
        actor->savedAngle = angle;
    }
    actor->registered = Script_GetKeywordValue(0x43, 1);
    if (Script_SeekToKeyword(0x72)) {
        actor->routeId = Script_GetValue();
        actor->routeMode = Script_GetValue();
        actor->routePoint = Script_GetValue();
        SceneBody_StartRoute(&actor->body, &actor->route, (u8)actor->routeId, (u8)actor->routeMode,
                             (u8)actor->routePoint);
        Path_GetPointPosition(&actor->body.position, actor->route.point, actor->routePoint);
    } else
        actor->routeId = -1;
    if (Script_SeekToKeyword(0x68)) {
        actor->hasDestination = 1;
        actor->destination.x = Script_GetValue();
        actor->destination.y = Script_GetValue();
        actor->destination.z = Script_GetValue();
    } else {
        actor->hasDestination = 0;
        actor->destination = actor->body.position;
    }
    if (Script_SeekToKeyword(0x54)) {
        actor->hasParameters = 1;
        actor->paramA = Script_GetValue();
        actor->paramB = Script_GetValue();
        actor->paramC = Script_GetValue();
        actor->paramD = Script_GetValue();
    } else {
        int zero = 0;
        actor->hasParameters = zero;
        actor->paramA = 9;
        actor->paramB = zero;
        actor->paramC = 21;
        actor->paramD = zero;
    }
    if (Script_SeekToKeyword(0x62)) {
        actor->modeA = Script_GetValue();
        actor->modeB = Script_GetValue();
        actor->modeValue = Script_GetValue();
    } else {
        int zero = 0;
        actor->modeA = zero;
        actor->modeB = 1;
        actor->modeValue = zero;
    }
    if (Script_SeekToKeyword(0x77)) {
        actor->rateC = Script_GetValue();
        actor->rateA = Script_GetValue();
        actor->rateB = Script_GetValue();
    } else {
        int *dst = &actor->rateC;
        int rate;
        rate = 256;
        *dst = rate;
        actor->rateA = rate;
        actor->rateB = 512;
    }
    if (Script_SeekToKeyword(0x48)) {
        actor->interaction.turning.firstDirection = Script_GetValue();
        actor->interaction.turning.secondDirection = Script_GetValue();
        actor->interaction.turning.turnRate = 4;
        actor->interaction.turning.turnState = 255;
    } else {
        u8 *dst = &actor->interaction.turning.firstDirection;
        int six;
        six = 6;
        *dst = six;
        actor->interaction.turning.secondDirection = six;
        actor->interaction.turning.turnRate = 4;
        actor->interaction.turning.turnState = 255;
    }
    actor->interaction.turning.turnRate = Script_GetKeywordValue(0x42, 4);
    if (Script_SeekToKeyword(0x74))
        actor->script = Script_GetValue();
    if (Script_SeekToKeyword(0x73)) {
        int a = Script_GetValue();
        int b = Script_GetValue();
        actor->radiusA = a;
        actor->radiusB = b;
        actor->radiusA32 = a;
        actor->radiusB32 = b;
        actor->radiusASquared = a * a;
        actor->radiusBSquared = b * b;
    } else {
        int a = 80, b = 96;
        actor->radiusA = a;
        actor->radiusB = b;
        actor->radiusA32 = a;
        actor->radiusB32 = b;
        actor->radiusASquared = 6400;
        actor->radiusBSquared = 9216;
    }
    actor->distance = Script_GetKeywordValue(0x4d, 10);
    actor->distanceSquared = actor->distance * actor->distance;
    actor->enabled = Script_GetKeywordValue(0x75, 1);
    actor->soundEnabled = Script_GetKeywordValue(0x46, 1);
    return 0;
}

static inline int SceneActor_ScaleMotion(int v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

static inline int SceneActor_TerrainHeight(struct ContactPosition *pos)
{
    int x = pos->x >> 8, z = pos->z >> 8, index;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    return (*SceneActor_GetTerrainCell(index) & 15) << 8;
}

void Actor_OnHiddenIdleUpdate(void *unused, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        int zero = 0;
        actor->animationEntered = zero;
        actor->finished = zero;
        Actor_PlayAnimationEntry(actor, 0, 1, zero, 4);
    }
    {
        u32 mask = 256;
        actor->flags |= mask;
    }
    actor->query(actor);
}

void Actor_OnIdleUpdate(void *unused, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        int zero = 0;
        actor->animationEntered = zero;
        actor->finished = zero;
        Actor_PlayAnimationEntry(actor, 0, 1, zero, 4);
    }
    {
        u32 mask = 4096;
        actor->flags |= mask;
    }
    actor->query(actor);
}

// confidence=low: advances toward the scripted position using the current movement angle.

void Actor_UpdateWalkToPosition(void *unused, struct SceneActor *arg)
{
    struct SceneActor *actor = arg;
    if (actor->animationEntered) {
        int zero = 0;
        actor->animationEntered = zero;
        actor->finished = zero;
        Actor_PlayAnimationEntry(actor, 1, 1, zero, 4);
    }
    if ((u8)actor->query(actor) != 1) {
        struct ContactPosition *pos = &actor->body.position;
        struct ContactPosition *target = &actor->target;
        int dx = target->x - pos->x;
        int dz = target->z - pos->z;
        u32 distance = dx * dx + dz * dz;
        u32 limit = actor->distanceSquared;
        struct ContactPosition *savedPos = pos;
        struct ContactPosition *savedTarget = target;

        if (distance <= limit) {
            actor->body.position = actor->target;
            actor->finished = 1;
        } else {
            u8 *ap = &actor->body.direction;
            struct ContactPosition *vel = &actor->body.motion;
            int angle = *ap;
            int speed = actor->distance;

            s32 product = speed * gUnk_086149C4[(angle + 64) & 255];
            u8 *savedAngle = ap;

            vel->x = SceneActor_ScaleMotion(product);
            vel->y = 0;
            vel->z = SceneActor_ScaleMotion(speed * gUnk_086149C4[angle]);
            {
                struct ContactPosition *target = savedTarget;
                s32 x;
                struct ContactPosition *pos;
                s32 z;

                x = target->x;
                pos = savedPos;

                x -= pos->x;

                z = target->z - pos->z;
                actor->direction = Math_Atan2(x, z);
            }
            {
                int value = actor->direction;
                u8 *out = savedAngle;

                *out = value;
                actor->facing8 = SceneFacing_EightWay(value);
                actor->facing4 = SceneFacing_FourWay(value);
            }
            Actor_PlayAnimationEntry(actor, 1, 1, 0, 1);
        }
        Actor_PlayFootstepSound(actor, 1);
    }
}

static inline s8 *SceneActorAnimation_SetAngle(struct SceneActor *actor, s16 angle)
{
    u16 facing;
    actor->body.direction = angle;
    facing = angle;
    {
        s32 shifted = (facing + 112) >> 5;
        u32 mask = 7;
        mask &= shifted;
        actor->facing8 = mask;
    }
    {
        s32 shifted = (facing + 96) >> 6;
        u32 mask = 3;
        mask &= shifted;
        actor->facing4 = mask;
    }
    return (s8 *)&actor->body.direction;
}

// confidence=low: turns toward a scripted angle and signals completion when aligned.

void Actor_UpdateTurnToAngle(void *unused, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, 2, 1, 0, 4);
    }
    {
        u32 mask = 0x1000;
        actor->flags |= mask;
    }
    if (((u8)actor->query(actor)) != 1) {
        s8 *angle = (s8 *)&actor->body.direction;
        s8 current = *angle;
        s32 delta = (s8)(actor->direction - current);
        s8 *savedAngle;
        if (delta > 8)
            current = (s8)current + 8;
        else if (delta < -8)
            current = (s8)current - 8;
        else
            current = (s8)current + delta;
        savedAngle = SceneActorAnimation_SetAngle(actor, current);
        Actor_PlayAnimationEntry(actor, 2, 1, 0, 1);
        if ((u8)*savedAngle == actor->direction)
            actor->finished = 1;
    }
}

// Provisional name: Actor_OnDescendingFadeUpdate (tentative)

static inline int SceneActor_TerrainHeightLevel(struct ContactPosition *pos)
{
    int x = pos->x >> 8, z = pos->z >> 8, index;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    return *SceneActor_GetTerrainCell(index) & 15;
}

void Actor_OnDescendingFadeUpdate(void *unused, struct SceneActor *actor, int ticks)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        {
            void *anim = &actor->drawable.common.kind;
            DisplayAnimation_SetFlags(anim, 2);
            DisplayAnimation_SetPalette(anim, 0x40, 0x40, 0);
        }
        Sound_PlaySong(0x201);
    }
    if ((u8)actor->query(actor) != 1) {
        int a = 0x40;
        int b = 0;
        struct ContactPosition *dq;
        int h;
        switch (actor->animationPhase) {
            case 0:
                b = (8 - ticks) * 200 + 0xa0;
                h = SceneActor_TerrainHeightLevel(
                        (struct ContactPosition *)actor->drawable.common.position)
                    << 8;
                dq = (struct ContactPosition *)actor->drawable.common.position;
                dq->y = h + b;
                if (ticks > 7)
                    actor->animationPhase++;
                DisplayAnimation_SetPalette(&actor->drawable.common.kind, 1, 0x80, 0);
                break;
            case 1: {
                int d = 15 - ticks;
                int c = a - d * 4;
                if (ticks > 8) {
                    a = (24 - ticks) << 3;
                    b = (d * 4 + d) * 4;
                }
                if (ticks > 14)
                    actor->animationPhase = 2;
                h = SceneActor_TerrainHeightLevel(
                        (struct ContactPosition *)actor->drawable.common.position)
                    << 8;
                dq = (struct ContactPosition *)actor->drawable.common.position;
                dq->y = h + b;
                DisplayAnimation_SetPalette(&actor->drawable.common.kind, c, a, 0);
                break;
            }
            case 2:
                DisplayAnimation_ClearFlags(&actor->drawable.common.kind, 2);
                actor->finished = 1;
                break;
        }
        {
            int ang = actor->direction;
            actor->body.direction = ang;
            {
                s32 shifted = (ang + 112) >> 5;
                u32 mask = 7;
                mask &= shifted;
                actor->facing8 = mask;
            }
            {
                s32 shifted = (ang + 96) >> 6;
                u32 mask = 3;
                mask &= shifted;
                actor->facing4 = mask;
            }
            Actor_PlayAnimationEntry(actor, 0, 2, 0, 0);
        }
    }
}

// Provisional name: Actor_OnRisingFadeUpdate (tentative)

void Actor_OnRisingFadeUpdate(void *unused, struct SceneActor *actor, int ticks)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        {
            void *anim = &actor->drawable.common.kind;
            DisplayAnimation_SetFlags(anim, 2);
            DisplayAnimation_SetPalette(anim, 0x40, 0x40, 0);
        }
        Sound_PlaySong(0x15F);
    }
    {
        int a = 0x40;
        int b = 0;
        struct ContactPosition *dq;
        int h;
        switch (actor->animationPhase) {
            case 0: {
                int c = a - ticks * 4;
                if (ticks > 8) {
                    a = ticks << 3;
                    b = ((ticks - 8) * 5) * 4;
                }
                if (ticks > 14)
                    actor->animationPhase = 1;
                h = SceneActor_TerrainHeightLevel(
                        (struct ContactPosition *)actor->drawable.common.position)
                    << 8;
                dq = (struct ContactPosition *)actor->drawable.common.position;
                dq->y = h + b;
                DisplayAnimation_SetPalette(&actor->drawable.common.kind, c, a, 0);
                break;
            }
            case 1: {
                b = ticks * 200 + 0xa0;
                h = SceneActor_TerrainHeightLevel(
                        (struct ContactPosition *)actor->drawable.common.position)
                    << 8;
                dq = (struct ContactPosition *)actor->drawable.common.position;
                dq->y = h + b;
                if (ticks > 7) {
                    u32 mask = 256;
                    actor->flags |= mask;
                    actor->animationPhase++;
                }
                break;
            }
            case 2: {
                u32 one = 1;
                u32 mask;
                actor->finished = one;
                mask = one + 0xff;
                actor->flags |= mask;
            }
                DisplayAnimation_ClearFlags(&actor->drawable.common.kind, 2);
                break;
        }
        {
            int ang = actor->direction;
            actor->body.direction = ang;
            {
                s32 shifted = (ang + 112) >> 5;
                u32 mask = 7;
                mask &= shifted;
                actor->facing8 = mask;
            }
            {
                s32 shifted = (ang + 96) >> 6;
                u32 mask = 3;
                mask &= shifted;
                actor->facing4 = mask;
            }
            if (actor->variant != 5)
                Actor_PlayAnimationEntry(actor, 0, 2, 0, 0);
        }
        actor->query(actor);
    }
}

void Actor_OnConfiguredAnimationUpdate(void *unused, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        int zero = 0;
        actor->animationEntered = zero;
        actor->finished = zero;
        Actor_PlayAnimationEntry(actor, actor->modeA, actor->modeB, zero, 4);
    }
    {
        u32 mask = 4096;
        actor->flags |= mask;
    }
    actor->query(actor);
}

// Provisional name: Actor_OnPathFollowUpdate (tentative)

struct SceneActor;

u32 sub_0821E67C(void *, void *, void *, void *, u32, u32, u32);

void Actor_OnPathFollowUpdate(void *unused, struct SceneActor *actor)
{
    struct SceneActorBody *path = &actor->body;
    struct SceneActorRouteCursor *ctx = &actor->route;
    struct ContactPosition *pos = &actor->body.position;
    union SceneActorDisplay *anim = &actor->drawable;
    if (actor->animationEntered) {
        int zero = 0;
        actor->animationEntered = zero;
        actor->finished = zero;
        {
            struct SceneActorRouteCursor *c3 = ctx;
            u16 *t = (u16 *)c3->point;
            int r = Math_Atan2(t[0] - pos->x, t[1] - pos->z);
            actor->body.direction = r;
            actor->facing8 = SceneFacing_EightWay(r);
            actor->facing4 = SceneFacing_FourWay(r);
        }
        Actor_PlayAnimationEntry(actor, 1, 1, 0, 4);
        actor->routeProgress = zero;
        actor->routeStepFinished = zero;
    }
    {
        u32 mask = 4096;
        actor->flags |= mask;
    }
    if ((u8)actor->query(actor) != 1) {
        if (actor->routeStepFinished == 0) {
            if (anim->common.kind == 2) {
                struct DisplayKeyframeState *keyframes = &actor->drawable.keyframe.keyframes;
                if (keyframes->status & 4) {
                    u32 v;
                    if (keyframes->frame & 1)
                        v = (u16)actor->radiusA >> 1;
                    else
                        v = (u16)actor->radiusA;
                    actor->routeStepFinished = sub_0821E67C(ctx, &path->position, &path->motion,
                                                            &path->direction, v, v, v);
                }
            } else {
                u32 v = (u16)actor->radiusA;
                actor->routeStepFinished =
                    sub_0821E67C(ctx, pos, &actor->body.motion, &actor->body.direction, v, v, v);
            }
        }
        if (actor->routeStepFinished) {
            if (anim->common.kind == 2) {
                struct DisplayKeyframeState *keyframes = &anim->keyframe.keyframes;
                if (keyframes->status & 2)
                    actor->finished = 1;
            } else if (DisplayAnimation_TestFinished(&actor->drawable.common.kind))
                actor->finished = 1;
        }
        Actor_PlayFootstepSound(actor, 1);
        Actor_PlayAnimationEntry(actor, 1, 1, 0, 1);
    }
}

extern u32 gUnk_02000560;

static inline s32 SceneActor_Absolute(s32 x)
{
    if (x < 0)
        return -x;
    return x;
}

static inline u32 AttachmentFlag(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}

// confidence=low: tracks a target facing and enables contact response when positions and angles agree.

void Actor_UpdateTargetFacing(struct SceneActorManager *ctx, struct SceneActor *actor)
{
    struct ContactPosition *pos = &actor->body.position;
    s32 facing;
    if (ctx->target) {
        struct ContactPosition *target = ctx->target;
        s32 angle = Math_Atan2(target->x - pos->x, target->z - pos->z);
        if (actor->animationEntries->kind) {
            u8 *current = &actor->body.direction;
            s32 diff = (s8)(angle - (*current));
            s32 value;
            s32 octant;
            s32 mask;
            s32 quadrantMask;
            if (diff > 8) {
                value = *current;
                value += 8;
                *current = value;
                octant = value + 112;
            } else if (diff < -8) {
                value = *current;
                value -= 8;
                *current = value;
                value = (u32)value << 16;
                value = (u32)value >> 16;
                octant = value + 112;
            } else {
                value = *current;
                value += diff;
                *current = value;
                value = (u32)value << 16;
                value = (u32)value >> 16;
                octant = value + 112;
            }
            octant >>= 5;
            mask = 7;
            mask &= octant;
            actor->facing8 = mask;
            value += 96;
            value >>= 6;
            quadrantMask = 3;
            quadrantMask &= value;
            actor->facing4 = quadrantMask;
        }
        facing = (u8)ctx->player->unk2D;
    } else
        facing = 0;
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, 0, 1, 0, 4);
        actor->routeProgress = 1;
        {
            u8 angle = actor->body.direction;
            actor->savedAnimationAngle = angle;
        }
        if (((ctx->player && !TargetAllowsInteraction(ctx->player)) && (!(gEngineFlags & 0x200))) &&
            (actor->enabled)) {
            struct Player *target = ctx->player;
            u8 tether;
            if ((target->tetherMode == 6) && TetherActor_GetPosition())
                tether = 1;
            else
                tether = 0;
            if (tether || AttachmentFlag())
                Actor_SetAttachmentEffect(actor, 0, 0, 0);
            else
                Actor_SetAttachmentEffect(actor, 3, 0, 0);
        }
    } else
        Actor_PlayAnimationEntry(actor, 0, 1, 0, 1);
    {
        u32 mask = 0x1000;
        actor->flags |= mask;
    }
    if (((u8)actor->query(actor)) != 1) {
        struct ContactPosition *target = ctx->target;
        s32 radius = actor->rateC;
        u8 SceneActor_IsNearTarget;
        if (((SceneActor_Absolute(target->x - pos->x) < radius) &&
             (SceneActor_Absolute(target->y - pos->y) <= 255)) &&
            (SceneActor_Absolute(target->z - pos->z) < radius))
            SceneActor_IsNearTarget = 1;
        else
            SceneActor_IsNearTarget = 0;
        if (SceneActor_IsNearTarget) {
            struct ContactPosition *destination = ctx->target;
            s32 direction = (s8)Math_Atan2(pos->x - destination->x, pos->z - destination->z);
            s32 delta = facing - direction;
            s32 oppositeFacing = (actor->body.direction) + 128;
            u8 opposite;
            opposite = facing - oppositeFacing;
            if ((SceneActor_Absolute((s8)delta) <= 63) &&
                (SceneActor_Absolute((s8)opposite) <= 31)) {
                u32 mask = 0x800;
                actor->flags |= mask;
            }
        }
    }
}

void Actor_OnTimedIdleUpdate(void *unused, struct SceneActor *actor, int ticks)
{
    if (actor->animationEntered) {
        int zero = 0;
        actor->animationEntered = zero;
        actor->finished = zero;
        Actor_PlayAnimationEntry(actor, 0, 1, zero, 4);
        actor->routeProgress = 1;
    }
    {
        u32 mask = 4096;
        actor->flags |= mask;
    }
    if ((u8)actor->query(actor) != 1 && ticks > 7)
        actor->finished = 1;
}

s32 DisplayAnimation_GetFrameBoundary(void *, u32 *);

static inline u8 *TerrainResponse(struct SceneActor *actor)
{
    return (u8 *)&actor->destination;
}

static inline void ClearCounter(struct SceneActor *actor, s32 value)
{
    actor->probeReady = value;
}

static inline s32 Callback(struct SceneActor *actor)
{
    {
        int (**slot)(struct SceneActor *) = &actor->query;
        return (*slot)(actor);
    }
}

static inline struct ContactPosition *ProbePosition(struct SceneActor *actor)
{
    {
        struct ContactPosition *result = &actor->probe.current;
        return result;
    }
}

static inline s32 CurrentX(struct SceneActorNavigationProbe *p)
{
    struct SceneActorNavigationProbe *raw = p;
    return raw->current.x;
}

static inline s32 TargetX(struct SceneActorNavigationProbe *p)
{
    struct SceneActorNavigationProbe *raw = p;
    return raw->target.x;
}

static inline s32 TargetZ(struct SceneActorNavigationProbe *p)
{
    struct SceneActorNavigationProbe *raw = p;
    return raw->target.z;
}

static inline s32 PositionZ(struct ContactPosition *p)
{
    struct ContactPosition *raw = p;
    return raw->z;
}

static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline void Facing(struct SceneActor *actor, u16 angle)
{
    s32 value = angle;
    s32 shifted = value + 112;
    s32 facing8;
    s32 facing4;
    shifted >>= 5;
    facing8 = 7;
    facing8 &= shifted;
    actor->facing8 = facing8;
    value = (value + 96) >> 6;
    facing4 = 3;
    facing4 &= value;
    actor->facing4 = facing4;
}

static inline void Polar(struct ContactPosition *v, s32 angle, s32 speed)
{
    v->x = Scale(speed * gUnk_086149C4[(angle + 64) & 255]);
    v->y = 0;
    v->z = Scale(speed * gUnk_086149C4[angle]);
}

// confidence=low: advances a navigation probe toward its target at animation frame boundaries.

void Actor_UpdateNavigationWalk(void *unused, struct SceneActor *actor)
{
    struct SceneActorBody *motion = (void *)&actor->body;
    struct SceneActorNavigationProbe *probe = (void *)&actor->probe;
    u32 *anim = (void *)&actor->drawable.common.kind;
    if (actor->animationEntered) {
        s32 zero = 0;
        struct ContactPosition *pos;
        actor->animationEntered = zero;
        actor->finished = zero;
        Actor_PlayAnimationEntry(actor, 3, 1, zero, 4);
        actor->routeProgress = 1;
        pos = (void *)&actor->body.position;
        ProximityVolume_TranslateBounds(probe, pos);
        Terrain_ApplyBlocked(probe, pos, TerrainResponse(actor));
        ClearCounter(actor, zero);
        Actor_SetAttachmentEffect(actor, 0, 0, 0);
    }
    {
        u32 mask = 0x1000;
        actor->flags |= mask;
    }
    if ((u8)Callback(actor) != 1) {
        struct ContactPosition *pos = (void *)&actor->body.position;
        struct ContactPosition *probePos = ProbePosition(actor);
        s32 x = CurrentX(probe) - motion->position.x;
        s32 z = probePos->z - pos->z;
        s32 distance = (s32)((u32)x * x + (u32)z * z);
        s32 limit = 0x1103;
        struct ContactPosition *savedPos = pos;
        if (distance <= limit) {
            struct SceneActorNavigationProbe *read = probe;
            motion->position.x = read->target.x;
            motion->position.z = read->target.z;
            actor->finished = 1;
        } else {
            u32 flags;
            if (DisplayAnimation_GetFrameBoundary(anim, &flags)) {
                u32 *reader = anim;
                s32 speed;
                s32 targetX;
                s32 angle;
                struct ContactPosition step;
                if (*reader == 2) {
                    s32 flag = flags & 1;
                    speed = 64;
                    if (flag)
                        speed = 32;
                } else
                    speed = 16;
                targetX = TargetX(probe);
                targetX -= motion->position.x;
                angle = Math_Atan2(targetX, TargetZ(probe) - PositionZ(savedPos));
                actor->body.direction = angle;
                Facing(actor, angle);
                Polar(&step, motion->direction, speed);
                motion->position.x += step.x;
                motion->position.z += step.z;
            }
        }
        NavMesh_UpdateProbe(probe, savedPos);
        Actor_PlayFootstepSound(actor, 3);
        Actor_PlayAnimationEntry(actor, 3, 1, 0, 1);
    }
}

static inline s32 RouteWalk_Scale12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline void SceneActor_FixedNavigationStep(struct ContactPosition *step, s32 angle)
{
    s32 speed = 48;
    step->x = RouteWalk_Scale12(speed * gUnk_086149C4[(angle + 64) & 255]);
    step->y = 0;
    step->z = RouteWalk_Scale12(speed * gUnk_086149C4[angle]);
}

// confidence=low: walks toward the navigation target while the animation permits a fixed motion step.

void Actor_UpdateNavigationStep(void *unused, struct SceneActor *actor)
{
    struct SceneActorBody *motion = &actor->body;
    struct SceneActorNavigationProbe *probe = &actor->probe;
    u32 *animType = &actor->drawable.common.kind;
    if (actor->animationEntered) {
        s32 zero = 0;
        struct ContactPosition *pos;
        actor->animationEntered = zero;
        actor->finished = zero;
        Actor_PlayAnimationEntry(actor, 1, 1, zero, 4);
        actor->routeProgress = zero;
        pos = &actor->body.position;
        ProximityVolume_TranslateBounds(probe, pos);
        Terrain_ApplyBlocked(probe, pos, &actor->destination);
        actor->probeReady = 1;
    }
    {
        u32 mask = 0x1000;
        actor->flags |= mask;
    }
    if ((u8)actor->query(actor) != 1) {
        struct ContactPosition *pos = &actor->body.position;
        struct ContactPosition *probePos = &actor->probe.current;
        s32 x = probe->current.x - motion->position.x;
        s32 z = probePos->z - pos->z;
        s32 distance = (s32)((u32)x * x + (u32)z * z);
        s32 limit = 0x9C3;
        struct ContactPosition *current = pos;
        if (distance <= limit) {
            motion->position.x = actor->probe.target.x;
            motion->position.z = actor->probe.target.z;
            actor->finished = 1;
        } else if (*animType == 2) {
            struct DisplayKeyframeState *anim = &actor->drawable.keyframe.keyframes;
            if (anim->status & 4) {
                struct ContactPosition *target = &actor->probe.target;
                s32 angle =
                    Math_Atan2(probe->target.x - motion->position.x, target->z - current->z);
                struct ContactPosition step;
                actor->body.direction = angle;
                actor->facing8 = SceneFacing_EightWay(angle);
                actor->facing4 = SceneFacing_FourWay(angle);
                SceneActor_FixedNavigationStep(&step, motion->direction);
                motion->position.x += step.x;
                motion->position.z += step.z;
            }
        }
        NavMesh_UpdateProbe(probe, current);
        Actor_PlayFootstepSound(actor, 1);
        Actor_PlayAnimationEntry(actor, 1, 1, 0, 1);
    }
}

static inline void RouteWalk_Polar(struct ContactPosition *step, s32 angle, s32 speed)
{
    step->x = RouteWalk_Scale12(speed * gUnk_086149C4[(angle + 64) & 255]);
    step->y = 0;
    step->z = RouteWalk_Scale12(speed * gUnk_086149C4[angle]);
}

static inline void RouteWalk_SetFacing(struct SceneActor *actor, u16 angle)
{
    s32 value = angle;
    s32 shifted = value + 112;
    s32 facing8;
    s32 facing4;
    shifted >>= 5;
    facing8 = 7;
    facing8 &= shifted;
    actor->facing8 = facing8;
    value = (value + 96) >> 6;
    facing4 = 3;
    facing4 &= value;
    actor->facing4 = facing4;
}

void SceneActorAnimation_UpdateRouteWalk(void *unused, struct SceneActor *actor)
{
    struct SceneActorBody *motion = &actor->body;
    struct SceneActorNavigationProbe *probe = &actor->probe;
    struct SceneActorRouteCursor *route = &actor->route;
    u32 *animType = &actor->drawable.common.kind;
    if (actor->animationEntered) {
        struct ContactPosition path;
        struct ContactPosition *pos;
        struct ContactPosition *target;
        s32 angle;
        actor->animationEntered = 0;
        actor->finished = 0;
        actor->routeProgress = 0;
        pos = &actor->body.position;
        ProximityVolume_TranslateBounds(probe, pos);
        Route_Start(route, actor->routeId, actor->routeMode, actor->routePoint);
        Path_GetPointPosition(&path, actor->route.point, 0);
        Terrain_ApplyBlocked(probe, pos, &path);
        target = &actor->probe.target;
        angle = Math_Atan2(probe->target.x - motion->position.x, target->z - pos->z);
        actor->body.direction = angle;
        RouteWalk_SetFacing(actor, angle);
        Actor_PlayAnimationEntry(actor, 1, 1, 0, 4);
    }
    {
        u32 mask = 0x1000;
        actor->flags |= mask;
    }
    if ((u8)actor->query(actor) != 1) {
        struct ContactPosition *pos = &actor->body.position;
        struct ContactPosition *probePos = &actor->probe.current;
        s32 x = probe->current.x - motion->position.x;
        s32 z = probePos->z - pos->z;
        s32 distance = (s32)((u32)x * x + (u32)z * z);
        s32 limit = 0x9C3;
        struct ContactPosition *current = pos;
        if (distance <= limit) {
            motion->position.x = actor->probe.target.x;
            motion->position.z = actor->probe.target.z;
            actor->finished = 1;
        } else if (*animType == 2) {
            struct DisplayKeyframeState *anim = &actor->drawable.keyframe.keyframes;
            if (anim->status & 4) {
                struct ContactPosition *target = &actor->probe.target;
                s32 angle =
                    Math_Atan2(probe->target.x - motion->position.x, target->z - current->z);
                s32 speed;
                struct ContactPosition step;
                actor->body.direction = angle;
                RouteWalk_SetFacing(actor, angle);
                speed = (anim->frame & 1) ? 24 : 48;
                RouteWalk_Polar(&step, motion->direction, speed);
                motion->position.x += step.x;
                motion->position.z += step.z;
            }
        }
        NavMesh_UpdateProbe(probe, current);
        Actor_PlayFootstepSound(actor, 1);
        Actor_PlayAnimationEntry(actor, 1, 1, 0, 1);
    }
}

void Actor_UpdateRunToPosition(void *unused, struct SceneActor *arg)
{
    struct SceneActor *actor = arg;
    if (actor->animationEntered) {
        int zero = 0;
        actor->animationEntered = zero;
        actor->finished = zero;
        Actor_PlayAnimationEntry(actor, 3, 1, zero, 4);
    }
    if ((u8)actor->query(actor) != 1) {
        struct ContactPosition *pos = &actor->body.position;
        struct ContactPosition *target = &actor->target;
        int dx = target->x - pos->x;
        int dz = target->z - pos->z;
        u32 distance = dx * dx + dz * dz;
        u32 limit = actor->distanceSquared;
        struct ContactPosition *savedPos = pos;
        struct ContactPosition *savedTarget = target;

        if (distance <= limit) {
            actor->body.position = actor->target;
            actor->finished = 1;
        } else {
            u8 *ap = &actor->body.direction;
            struct ContactPosition *vel = &actor->body.motion;
            int angle = *ap;
            int speed = actor->distance;

            s32 product = speed * gUnk_086149C4[(angle + 64) & 255];
            u8 *savedAngle = ap;

            vel->x = SceneActor_ScaleMotion(product);
            vel->y = 0;
            vel->z = SceneActor_ScaleMotion(speed * gUnk_086149C4[angle]);
            {
                struct ContactPosition *target = savedTarget;
                s32 x;
                struct ContactPosition *pos;
                s32 z;

                x = target->x;
                pos = savedPos;

                x -= pos->x;

                z = target->z - pos->z;
                actor->direction = Math_Atan2(x, z);
            }
            {
                int value = actor->direction;
                u8 *out = savedAngle;

                *out = value;
                actor->facing8 = SceneFacing_EightWay(value);
                actor->facing4 = SceneFacing_FourWay(value);
            }
            Actor_PlayAnimationEntry(actor, 3, 1, 0, 1);
        }
        Actor_PlayFootstepSound(actor, 3);
    }
}

int Path_GetPointFacing(struct SceneActorRoutePoint *, int);

int Actor_SelectPathNodeAction(struct SceneActor *actor, struct SceneActorRoutePoint *node)
{
    u32 value = node->attributes;
    int kind;
    do {
        kind = value >> 4;
        actor->nodeId = Path_GetPointNodeId(node, 0);
        switch (kind) {
            case 1: {
                int angle = (u16)Path_GetPointFacing(node, 0);
                int facing = (angle + 112) >> 5;
                int mask;
                mask = 7;
                actor->nodeFacing = facing & mask;
                if ((((actor->body.direction + 112) >> 5) & mask) != actor->nodeFacing) {
                    return 8;
                }
                break;
            }
            case 2:
                return 9;
            case 0:
            default:
                break;
        }
    } while (0);
    return 7;
}

int SceneCharacter_GetPathNodeState(struct SceneActor *actor)
{
    struct SceneActorRouteCursor *path = &actor->route;
    struct SceneActorRoutePoint *node;
    int dx, result;
    node = path->point;
    dx = (u16)node->x - actor->body.position.x;
    if (dx < 0)
        dx = -dx;
    if (dx <= 40) {
        int dz = (u16)node->z - actor->body.position.z;
        if (dz < 0)
            dz = -dz;
        if (dz <= 40)
            result = Actor_SelectPathNodeAction(actor, node);
        else
            result = 9;
    } else
        result = 9;
    return result;
}

void SceneCharacter_UpdateIdleState(u32 a, struct SceneActor *actor)
{
    if (actor->commandEntered) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 1;
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    actor->ticks++;
}

void SceneCharacter_UpdateStandState(u32 a, struct SceneActor *actor)
{
    if (actor->commandEntered != 0) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 0x2;
        Trigger_ConsumePending(&actor->messageReceiver, 1);
    }
    actor->ticks++;
}

void SceneCharacter_UpdateOneShotActionState(void *unused, struct SceneActor *actor)
{
    u32 one;
    u32 zero;
    if (actor->commandEntered != 0) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 3;
    }
    if (actor->finished != 0) {
        one = 1;
        zero = 0;
        actor->state = one;
        actor->ticks = zero;
        actor->commandEntered = one;
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    actor->ticks += 1;
}

static inline int SceneActor_IsNearTarget(struct SceneActor *actor, struct ContactPosition *b)
{
    struct ContactPosition *a = &actor->body.position;
    int limit;
    int x;
    int result;
    limit = actor->rateA;
    x = b->x - a->x;
    if (x < 0)
        x = -x;
    if (x >= limit)
        goto fail;
    {
        int y = b->y - a->y;
        if (y < 0)
            y = -y;
        if (y > 255)
            goto fail;
    }
    {
        int z = b->z - a->z;
        if (z < 0)
            z = -z;
        result = 1;
        if (z < limit)
            goto done;
    }
fail:
    result = 0;
done:
    return result;
}

static inline int SceneActor_HasNearbyTarget(struct SceneActorManager *manager,
                                             struct SceneActor *actor)
{
    int result;
    if (manager->target) {
        int close = SceneActor_IsNearTarget(actor, manager->target);
        result = 1;
        if (close)
            goto done;
    }
    result = 0;
done:
    return result;
}

void SceneCharacter_UpdateTurnState(void *unused, struct SceneActor *actor)
{
    if (actor->commandEntered) {
        int zero = 0;
        actor->commandEntered = zero;
        actor->finished = zero;
        actor->action = 4;
    }
    if (actor->finished) {
        int controlled = actor->controlled;
        if (controlled) {
            int state = actor->savedState;
            int zero = 0;
            actor->state = state;
            actor->ticks = zero;
            actor->commandEntered = 1;
        } else {
            actor->state = 1;
            actor->ticks = controlled;
            actor->commandEntered = 1;
        }
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    actor->ticks++;
}

void SceneCharacter_UpdateAlternateOneShotActionState(void *unused, struct SceneActor *actor)
{
    u32 one;
    u32 zero;
    if (actor->commandEntered != 0) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 5;
    }
    if (actor->finished != 0) {
        one = 1;
        zero = 0;
        actor->state = one;
        actor->ticks = zero;
        actor->commandEntered = one;
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    actor->ticks += 1;
}

void SceneCharacter_UpdateStopState(void *unused, struct SceneActor *actor)
{
    if (actor->commandEntered) {
        int zero = 0;
        actor->commandEntered = zero;
        actor->finished = zero;
        actor->action = 6;
    }
    if (actor->finished) {
        int zero = 0;
        actor->state = zero;
        actor->ticks = zero;
        actor->commandEntered = 1;
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    actor->ticks++;
}

void SceneCharacter_UpdateProximityWaitState(struct SceneActorManager *manager,
                                             struct SceneActor *actor)
{
    int close;
    if (actor->commandEntered) {
        int zero = 0;
        actor->commandEntered = zero;
        actor->finished = zero;
        actor->action = 2;
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    close = SceneActor_HasNearbyTarget(manager, actor);
    if (close) {
        int state = actor->state;
        u8 *dst = &actor->savedState;
        int zero;
        zero = 0;
        *dst = state;
        actor->state = 10;
        actor->ticks = zero;
        actor->commandEntered = 1;
    } else
        actor->ticks++;
}

void SceneCharacter_UpdatePathWaitState(struct SceneActorManager *manager, struct SceneActor *actor)
{
    int close;
    void *path = &actor->route;
    int nextState;
    int zero;
    if (actor->commandEntered) {
        int zero = 0;
        actor->commandEntered = zero;
        actor->finished = zero;
        actor->action = 2;
    }
    if (!(actor->ticks & 15)) {
        s8 *firstMode = (s8 *)&actor->hasDestination;
        int flag = *firstMode;
        s8 *mode = firstMode;
        if (flag) {
            struct ContactPosition *q = TetherActor_GetPosition();
            int nearby = 0;
            if (q) {
                struct ContactPosition *pos = &actor->body.position;
                int dx = pos->x - q->x;
                int dz = pos->z - q->z;
                int distance = dx * dx + dz * dz;
                if (distance <= 159999)
                    nearby = 1;
            }
            {
                if (nearby)
                    actor->cooldown = 1800;
                else {
                    if (!*mode || !manager->weather)
                        goto checkPhase;
                    actor->cooldown = 300;
                }
                nextState = 12;
                zero = 0;
                actor->state = nextState;
                actor->ticks = zero;
                actor->commandEntered = 1;
                goto done;
            }
        }
    checkPhase:
        if (actor->hasParameters && manager->minuteChanged) {
            u8 *phase = &manager->hour;
            u8 *last = &actor->paramC;
            if (*phase == *last && manager->minute >= actor->paramD)
                goto state13;
            if (*phase - 1 != *last)
                goto afterPhase;
            if (manager->minute > actor->paramD)
                goto afterPhase;
        state13:
            nextState = 13;
            zero = 0;
            goto changeState;
        }
    }
afterPhase:
    close = SceneActor_HasNearbyTarget(manager, actor);
    if (close) {
        int state = actor->state;
        u8 *dst = &actor->savedState;
        int zero;
        zero = 0;
        *dst = state;
        actor->state = 10;
        actor->ticks = zero;
        actor->commandEntered = 1;
        goto done;
    }
    if (actor->routeId >= 0) {
        u16 *timer = &actor->nodeId;
        s32 ticks = *timer;
        u16 remaining = ticks;
        if (!remaining) {
            Route_AdvancePoint(path);
            actor->state = SceneCharacter_GetPathNodeState(actor);
            actor->ticks = remaining;
            actor->commandEntered = 1;
        } else {
            *timer = ticks - 1;
        }
    }
    actor->ticks++;
    goto done;
changeState:
    actor->state = nextState;
    actor->ticks = zero;
    actor->commandEntered = 1;
done:;
}

void SceneCharacter_UpdatePathTurnState(struct SceneActorManager *manager, struct SceneActor *actor)
{
    int close;
    if (actor->commandEntered) {
        int zero = 0;
        actor->commandEntered = zero;
        actor->finished = zero;
        actor->action = 4;
        actor->direction = (actor->nodeFacing + 5) << 5;
    }
    close = SceneActor_HasNearbyTarget(manager, actor);
    if (close) {
        int state = actor->state;
        u8 *dst = &actor->savedState;
        int zero;
        zero = 0;
        *dst = state;
        actor->state = 10;
        actor->ticks = zero;
        actor->commandEntered = 1;
    } else if (actor->finished) {
        actor->state = 7;
        actor->ticks = close;
        actor->commandEntered = 1;
    } else
        actor->ticks++;
}

void SceneCharacter_UpdatePathActionState(struct SceneActorManager *manager,
                                          struct SceneActor *actor)
{
    int close;
    int nextState;
    int zero;
    if (actor->commandEntered) {
        int zero = 0;
        actor->commandEntered = zero;
        actor->finished = zero;
        actor->action = 8;
    }
    if (!(actor->ticks & 15)) {
        s8 *firstMode = (s8 *)&actor->hasDestination;
        int flag = *firstMode;
        s8 *mode = firstMode;
        if (flag) {
            struct ContactPosition *q = TetherActor_GetPosition();
            int nearby = 0;
            if (q) {
                struct ContactPosition *pos = &actor->body.position;
                int dx = pos->x - q->x;
                int dz = pos->z - q->z;
                int distance = dx * dx + dz * dz;
                if (distance <= 159999)
                    nearby = 1;
            }
            {
                if (nearby)
                    actor->cooldown = 1800;
                else {
                    if (!*mode || !manager->weather)
                        goto checkPhase;
                    actor->cooldown = 300;
                }
                nextState = 12;
                zero = 0;
                goto changeState;
            }
        }
    checkPhase:
        if (actor->hasParameters && manager->minuteChanged) {
            u8 *phase = &manager->hour;
            u8 *last = &actor->paramC;
            if (*phase == *last && manager->minute >= actor->paramD)
                goto state13;
            if (*phase - 1 != *last)
                goto afterPhase;
            if (manager->minute > actor->paramD)
                goto afterPhase;
        state13:
            nextState = 13;
            zero = 0;
            goto changeState;
        }
    }
afterPhase:
    close = SceneActor_HasNearbyTarget(manager, actor);
    if (close) {
        int state = actor->state;
        u8 *dst = &actor->savedState;
        int zero;
        zero = 0;
        *dst = state;
        actor->state = 10;
        actor->ticks = zero;
        actor->commandEntered = 1;
        goto done;
    } else if (actor->finished) {
        actor->state = 7;
        actor->ticks = close;
        actor->commandEntered = 1;
        goto done;
    }
    goto tick;
changeState:
    actor->state = nextState;
    actor->ticks = zero;
    actor->commandEntered = 1;
    goto done;
tick:
    actor->ticks++;
done:;
}

static inline int SceneActor_IsNearBodyPosition(struct SceneActor *actor, struct ContactPosition *b,
                                                struct SceneActorBody *body)
{
    struct ContactPosition *a = &actor->body.position;
    int limit = actor->rateB;
    int x = b->x - body->position.x;
    int result;
    if (x < 0)
        x = -x;
    if (x >= limit)
        goto fail;
    {
        int y = b->y - a->y;
        if (y < 0)
            y = -y;
        if (y > 255)
            goto fail;
    }
    {
        int z = b->z - a->z;
        if (z < 0)
            z = -z;
        result = 1;
        if (z < limit)
            goto done;
    }
fail:
    result = 0;
done:
    return result;
}

void SceneCharacter_UpdateTargetProximityState(struct SceneActorManager *manager,
                                               struct SceneActor *actor)
{
    struct SceneActorBody *body = (struct SceneActorBody *)&actor->body;
    if (actor->commandEntered) {
        int zero = 0;
        actor->commandEntered = zero;
        actor->finished = zero;
        actor->action = 9;
    }
    if (manager->player && manager->target) {
        int close = SceneActor_IsNearBodyPosition(actor, manager->target, body);
        if (!close) {
            int registered = actor->registered;
            if (registered) {
                actor->direction = actor->savedAngle;
                actor->state = 3;
                actor->ticks = close;
            } else {
                actor->state = actor->savedState;
                actor->ticks = registered;
            }
            actor->commandEntered = 1;
        } else
            actor->ticks++;
    }
}

void SceneCharacter_UpdateTetherReactionState(u8 *a, struct SceneActor *actor)
{
    if (actor->commandEntered != 0) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 0xa;
    }
    if (actor->finished != 0) {
        u32 c = 0xc;
        u32 z = 0;
        actor->state = c;
        actor->ticks = z;
        actor->commandEntered = 1;
    }
    actor->ticks += 1;
}

void SceneCharacter_UpdatePhaseReactionState(u8 *a, struct SceneActor *actor)
{
    if (actor->commandEntered != 0) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 0xB;
    }
    if (actor->finished != 0) {
        u32 c = 0xE;
        u32 z = 0;
        actor->state = c;
        actor->ticks = z;
        actor->commandEntered = 1;
    }
    actor->ticks += 1;
}

void SceneCharacter_UpdatePhaseTransitionState(struct SceneActorManager *manager,
                                               struct SceneActor *actor)
{
    int close;
    if (actor->commandEntered) {
        int zero = 0;
        actor->commandEntered = zero;
        actor->finished = zero;
        actor->action = 12;
    }
    close = SceneActor_HasNearbyTarget(manager, actor);
    if (close) {
        int state = actor->state;
        u8 *dst = &actor->savedState;
        int zero;
        zero = 0;
        *dst = state;
        actor->state = 10;
        actor->ticks = zero;
        actor->commandEntered = 1;
    } else {
        if (actor->finished) {
            actor->state = 14;
            actor->ticks = close;
            actor->commandEntered = 1;
        }
        actor->ticks++;
    }
}

void SceneCharacter_UpdatePhaseWindowWaitState(struct SceneActorManager *manager,
                                               struct SceneActor *actor)
{
    if (actor->commandEntered) {
        int zero = 0;
        actor->commandEntered = zero;
        actor->finished = zero;
        actor->action = 1;
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    if (actor->cooldown)
        goto decrement;
    if (manager->weather)
        goto tick;
    {
        u8 *phasePtr = &manager->hour;
        u8 *firstPtr = &actor->paramA;
        unsigned phase, first;
        phase = *phasePtr;
        first = *firstPtr;
        if (phase == first && manager->minute >= actor->paramB)
            goto transition;
        if (phase > first && phase < actor->paramC)
            goto transition;
        if (phase != actor->paramC)
            goto tick;
        if (manager->minute >= actor->paramD)
            goto tick;
    }
transition: {
    int state = 15;
    int zero = 0;
    actor->state = state;
    actor->ticks = zero;
    actor->commandEntered = 1;
}
    goto done;
decrement:
    actor->cooldown--;
tick:
    actor->ticks++;
done:;
}

void SceneCharacter_UpdatePathResumeState(struct SceneActorManager *manager,
                                          struct SceneActor *actor)
{
    int close;
    if (actor->commandEntered) {
        int zero = 0;
        actor->commandEntered = zero;
        actor->finished = zero;
        actor->action = 13;
    }
    close = SceneActor_HasNearbyTarget(manager, actor);
    if (close) {
        int state = actor->state;
        u8 *dst = &actor->savedState;
        int zero;
        zero = 0;
        *dst = state;
        actor->state = 10;
        actor->ticks = zero;
        actor->commandEntered = 1;
    } else {
        if (actor->finished) {
            actor->state = SceneCharacter_GetPathNodeState(actor);
            actor->ticks = close;
            actor->commandEntered = 1;
        }
        actor->ticks++;
    }
}
