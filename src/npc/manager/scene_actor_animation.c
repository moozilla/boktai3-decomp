#include "player/player_slot_status.h"
#include "npc/manager/scene_actor_actions.h"
#include "bosses/vanargand/vanargand_parts.h"
#include "monsters/ghoul_disc.h"

struct ContactPosition *TetherActor_GetPosition(void);

#include "npc/manager/scene_actor_lifecycle.h"

#include "global.h"

#include "engine/engine.h"

#include "engine/video/display_animation.h"

#include "engine/ram.h"

#include "engine/input.h"

#include "player/player.h"

#include "scene/motion/scene_facing.h"

#include "engine/engine_globals.h"

#include "scene/motion/scene_actor_route.h"
#include "scene/motion/scene_actor_charge_offset.h"

#include "engine/video/display_keyframe.h"
#include "scene/motion/scene_actor_chase_shared.h"

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
    struct SceneActorBody *body = &actor->body;
    void *drawable = &actor->drawable.common.kind;
    Obj_StepPosition(body);
    if (!SceneActor_TestFlags(actor, 0x400)) {
        struct ContactPosition *pos = &body->position;
        int x = pos->x >> 8, z = pos->z >> 8;
        int index, slope, height;
        u8 *cell;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        cell = SceneActor_GetTerrainCell(index);
        slope = *cell >> 4;
        height = (*cell & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)pos->z;
                break;
            case 2:
                height -= (u8)pos->x;
                break;
        }
        body->position.y = height;
    }
    DisplayAnimation_SetPosition(drawable, (struct DisplayPosition *)&body->position);
    DisplayAnimation_UpdatePlayback(drawable);
    if (actor->timer > 0) {
        actor->timer--;
        if (actor->timer <= 3) {
            if (actor->flashing && !(gEngineFlags & 0x200)) {
                actor->timer = actor->duration;
            } else {
                actor->timer = 0;
                Actor_SetAttachmentEffect(actor, 8, 180, 180);
                actor->flashing = 0;
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

void Trap_UpdateProbeContacts(void *);

int DisplayAnimation_SetFlags(void *, int);

int Actor_UpdateVisibilityAndShadow(void *unused, struct SceneActor *actor)
{
    int result;
    actor->positionLink.id = actor->body.id;
    actor->positionLink.position = &actor->body.position;
    Trap_UpdateProbeContacts(&actor->positionLink.id);
    if (SceneActor_TestFlags(actor, 0x100)) {
        DisplayAnimation_SetFlags(&actor->drawable.common.kind, 1);
        result = Actor_DisableShadow(actor);
    } else {
        DisplayAnimation_ClearFlags(&actor->drawable.common.kind, 1);
        if (SceneActor_TestFlags(actor, 0x200))
            result = Actor_DisableShadow(actor);
        else
            result = Actor_EnableShadow(actor);
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
        direction = actor->facing4 == 1 || actor->facing4 == 2 ? stride : 0;
        flip = actor->facing4 > 1;
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
        direction = actor->facing8 == 4 ? stride : 0;
        flip = actor->facing8 > 4;
    } else {
        direction = actor->facing8 == 4 ? stride : 0;
        flip = actor->facing8 > 4;
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

/* Two step sounds per surface, indexed by the resource entry's sound. */
extern const u16 gUnk_086055D4[][2];

void Actor_PlayFootstepSound(struct SceneActor *actor, u32 mode)
{
    u32 frame;
    u32 foot;
    if (!actor->soundEnabled)
        return;
    if (mode == 1)
        foot = 0;
    else if (mode == 3)
        foot = 2;
    else
        return;
    if (DisplayAnimation_GetFrameBoundary(&actor->drawable.common, &frame)) {
        const struct SceneActorResourceEntry *info = actor->resourceEntry;
        const s8 *table = info->payload.footstepFrames;
        const s8 *frames = table + foot;
        if (frames[0] == frame)
            Sound_PlaySong(gUnk_086055D4[info->sound][0]);
        else if (frames[1] == frame)
            Sound_PlaySong(gUnk_086055D4[info->sound][1]);
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

static inline void SceneActor_OffsetAlongHeading(struct ContactPosition *v, u32 angle,
                                                 const s32 radius)
{
    v->x = SceneActor_ChargeComponent(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = SceneActor_ChargeComponent(gUnk_086149C4[angle & 255] * radius);
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
    struct SceneActorManager *manager = gSceneActorListManager;
    struct SceneActor *actor;
    struct ContactPosition offset;
    if (!manager || (gEngineFlags & 0x200))
        return;
    for (actor = manager->head; actor; actor = actor->previous) {
        if (SceneActor_TestFlags(actor, 0x1000)) {
            struct ContactPosition *position = &actor->body.position;
            struct ContactPosition *origin = &emitter->position;
            s32 dx = origin->x - position->x;
            s32 dz = origin->z - position->z;
            if (dx * dx + dz * dz < 10000) {
                SceneActor_OffsetAlongHeading(&offset, Math_Atan2(dx, dz), 100);
                Add(&offset, position, &offset);
                Vanargand_Difference(&emitter->motion, &offset, &emitter->position);
                if (emitter->movementProbe)
                    Terrain_MoveFootprint(emitter->movementProbe, &emitter->position,
                                          &emitter->motion, emitter->probeExtentX,
                                          emitter->probeExtentZ, emitter->mode);
            }
        }
    }
}

s32 Scene_CanOpenMenu(void);

static inline s32 PlayerKind(void)
{
    return gPlayers[0]->tetherMode;
}

// confidence=low: highlights the nearest eligible actor and invokes its script on input.

static inline s32 TargetAllowsInteraction(struct Player *p)
{
    if ((u8)(p->actionMode - 3) <= 2)
        return 0;
    if (SceneActorChase_Bits32(&p->statusFlags, 0x100000))
        return 0;
    if (p->active && (u16)(p->sceneMode - 5) <= 1)
        return 0;
    if (p->tetherMode > 1 && p->tetherMode != 7)
        return 0;
    return 1;
}

void Actor_UpdateInteractionCue(struct SceneActorManager *manager)
{
    s32 pressed = gPadState[1] & 1;
    s32 kind;
    struct SceneActor *nearest;
    struct SceneActor *actor;
    if (PlayerKind() == 10 || PlayerKind() == 11)
        pressed = 0;
    if (manager->player && TargetAllowsInteraction(manager->player)) {
        if (manager->player->actionMode == 0)
            kind = 0;
        else if (manager->player->actionMode == 1)
            kind = 1;
        else if (manager->player->actionMode == 6)
            kind = 2;
        else
            kind = 3;
    } else {
        kind = 3;
    }
    nearest = 0;
    if (Scene_CanOpenMenu()) {
        s32 best = 1000000;
        for (actor = manager->head; actor; actor = actor->previous) {
            if (SceneActor_TestFlags(actor, 0x800) && actor->script) {
                struct ContactPosition *position = &actor->body.position;
                struct ContactPosition *target = manager->target;
                s32 x = target->x - position->x;
                s32 z = target->z - position->z;
                s32 distance = x * x + z * z;
                if (distance < best) {
                    best = distance;
                    nearest = actor;
                }
            }
        }
    }
    if (nearest && !pressed) {

        for (actor = manager->head; actor; actor = actor->previous) {
            if (actor == nearest && !(SceneActor_TestFlags(nearest, 0x2000))) {
                if (!nearest->flashing) {
                    Actor_SetAttachmentEffect(nearest, nearest->interaction.cues[kind], 180, 180);
                    nearest->flashing = 1;
                }
            } else if (actor->flashing) {
                Actor_SetAttachmentEffect(actor, 8, 180, 180);
                actor->flashing = 0;
            }
        }
    } else {
        for (actor = manager->head; actor; actor = actor->previous) {
            if (actor->flashing) {
                Actor_SetAttachmentEffect(actor, 8, 180, 180);
                actor->flashing = 0;
            }
        }
    }
    if (nearest && pressed) {
        struct {
            u32 identity;
            s32 x, y, z;
            struct SceneActorParameters parameters;
        } operands;
        s32 i;
        operands.identity = nearest->body.id;
        operands.x = nearest->body.position.x;
        operands.y = nearest->body.position.y;
        operands.z = nearest->body.position.z;
        for (i = 0; i < 4; i++)
            operands.parameters.values[i] = nearest->parameters.values[i];
        // The prepared operands are retained even though dispatch uses a null argument block.
        Script_Call(nearest->script, 0);
    }
}

/* Message packet, as sent by SceneMessages_Send (see NidhoggrDuel_NotifyLinkedActorLatePhase). */

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
    actor->stage = 0;
    actor->phase = 0;
    actor->state = state;
    actor->ticks = 0;
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
    struct SceneActorManager *manager = gSceneActorListManager;
    struct SceneActor *actor;
    int mode;
    if (!manager)
        return -1;
    actor = Actor_FindById(manager, Script_GetKeywordValue('n', 0));
    if (!actor)
        return -1;
    actor->controlled = 0;
    mode = Script_GetKeywordValue('i', 0);
    switch (mode) {
        case 0:
            Actor_SetScriptState(actor, 1);
            break;
        case 1:
            Actor_SetScriptState(actor, 0);
            break;
    }
}

/* Actor_IsInReactionStateFromScript: Tentatively tests whether the script-selected actor has a reaction code from thirteen through fifteen */

u32 Actor_IsInReactionStateFromScript(void)
{
    struct SceneActorManager *manager = gSceneActorListManager;
    struct SceneActor *actor;
    if (!manager)
        return 0;
    actor = Actor_FindById(manager, Script_GetKeywordValue('n', 0));
    if (!actor || (u8)(actor->savedState - 13) > 2)
        return 0;
    return 1;
}

int Actor_InitBehaviorFromScript(void *unused, struct SceneActor *actor)
{
    if (Script_GetKeywordValue('C', 1))
        Actor_RegisterMotion(actor);
    actor->savedAngle = actor->body.direction;
    actor->registered = Script_GetKeywordValue('C', 1);
    if (Script_SeekToKeyword('r')) {
        actor->routeId = Script_GetValue();
        actor->routeMode = Script_GetValue();
        actor->routePoint = Script_GetValue();
        SceneBody_StartRoute(&actor->body, &actor->route, (u8)actor->routeId, (u8)actor->routeMode,
                             (u8)actor->routePoint);
        Path_GetPointPosition(&actor->body.position, actor->route.point, actor->routePoint);
    } else
        actor->routeId = -1;
    if (Script_SeekToKeyword('h')) {
        actor->hasDestination = 1;
        actor->destination.x = Script_GetValue();
        actor->destination.y = Script_GetValue();
        actor->destination.z = Script_GetValue();
    } else {
        actor->hasDestination = 0;
        actor->destination = actor->body.position;
    }
    if (Script_SeekToKeyword('T')) {
        actor->hasParameters = 1;
        actor->paramA = Script_GetValue();
        actor->paramB = Script_GetValue();
        actor->paramC = Script_GetValue();
        actor->paramD = Script_GetValue();
    } else {
        actor->hasParameters = 0;
        actor->paramA = 9;
        actor->paramB = 0;
        actor->paramC = 21;
        actor->paramD = 0;
    }
    if (Script_SeekToKeyword('b')) {
        actor->modeA = Script_GetValue();
        actor->modeB = Script_GetValue();
        actor->modeValue = Script_GetValue();
    } else {
        actor->modeA = 0;
        actor->modeB = 1;
        actor->modeValue = 0;
    }
    if (Script_SeekToKeyword('w')) {
        actor->rateC = Script_GetValue();
        actor->rateA = Script_GetValue();
        actor->rateB = Script_GetValue();
    } else {
        actor->rateC = 256;
        actor->rateA = 256;
        actor->rateB = 512;
    }
    if (Script_SeekToKeyword('H')) {
        actor->interaction.turning.firstDirection = Script_GetValue();
        actor->interaction.turning.secondDirection = Script_GetValue();
        actor->interaction.turning.turnRate = 4;
        actor->interaction.turning.turnState = 255;
    } else {
        actor->interaction.turning.firstDirection = 6;
        actor->interaction.turning.secondDirection = 6;
        actor->interaction.turning.turnRate = 4;
        actor->interaction.turning.turnState = 255;
    }
    actor->interaction.turning.turnRate = Script_GetKeywordValue('B', 4);
    if (Script_SeekToKeyword('t'))
        actor->script = Script_GetValue();
    if (Script_SeekToKeyword('s')) {
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
    actor->distance = Script_GetKeywordValue('M', 10);
    actor->distanceSquared = actor->distance * actor->distance;
    actor->enabled = Script_GetKeywordValue('u', 1);
    actor->soundEnabled = Script_GetKeywordValue('F', 1);
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
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, 0, 1, 0, 4);
    }
    SceneActor_SetFlags(actor, 256);
    actor->query(actor);
}

void Actor_OnIdleUpdate(void *unused, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, 0, 1, 0, 4);
    }
    SceneActor_SetFlags(actor, 4096);
    actor->query(actor);
}

static inline void SceneActorAnimation_SetAngle(struct SceneActor *actor, int angle)
{
    actor->body.direction = angle;
    actor->facing8 = SceneFacing_EightWay(angle);
    actor->facing4 = SceneFacing_FourWay(angle);
}

// confidence=low: advances toward the scripted position using the current movement angle.

void Actor_UpdateWalkToPosition(void *unused, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, 1, 1, 0, 4);
    }
    if ((u8)actor->query(actor) != 1) {
        struct ContactPosition *pos = &actor->body.position;
        struct ContactPosition *target = &actor->target;
        int dx = target->x - pos->x;
        int dz = target->z - pos->z;
        u32 distance = dx * dx + dz * dz;
        u32 limit = actor->distanceSquared;
        struct ContactPosition *from = pos;
        struct ContactPosition *to = target;
        if (distance <= limit) {
            actor->body.position = actor->target;
            actor->finished = 1;
        } else {
            SceneActor_OffsetAlongHeading(&actor->body.motion, actor->body.direction,
                                          actor->distance);
            actor->direction = Math_Atan2(to->x - from->x, to->z - from->z);
            SceneActorAnimation_SetAngle(actor, actor->direction);
            Actor_PlayAnimationEntry(actor, 1, 1, 0, 1);
        }
        Actor_PlayFootstepSound(actor, 1);
    }
}

// confidence=low: turns toward a scripted angle and signals completion when aligned.

void Actor_UpdateTurnToAngle(void *unused, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, 2, 1, 0, 4);
    }
    SceneActor_SetFlags(actor, 0x1000);
    if ((u8)actor->query(actor) != 1) {
        s8 angle = actor->body.direction;
        s32 delta = (s8)(actor->direction - angle);
        if (delta > 8)
            angle += 8;
        else if (delta < -8)
            angle -= 8;
        else
            angle += delta;
        SceneActorAnimation_SetAngle(actor, angle);
        Actor_PlayAnimationEntry(actor, 2, 1, 0, 1);
        if (actor->body.direction == actor->direction)
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
        DisplayAnimation_SetFlags(&actor->drawable.common.kind, 2);
        DisplayAnimation_SetPalette(&actor->drawable.common.kind, 0x40, 0x40, 0);
        Sound_PlaySong(0x201);
    }
    if ((u8)actor->query(actor) != 1) {
        int a = 0x40;
        int b = 0;
        struct ContactPosition *position;
        int height;
        switch (actor->animationPhase) {
            case 0:
                b = (8 - ticks) * 200 + 0xa0;
                height = SceneActor_TerrainHeightLevel(
                             (struct ContactPosition *)actor->drawable.common.position)
                         << 8;
                position = (struct ContactPosition *)actor->drawable.common.position;
                position->y = height + b;
                if (ticks > 7)
                    actor->animationPhase++;
                DisplayAnimation_SetPalette(&actor->drawable.common.kind, 1, 0x80, 0);
                break;
            case 1: {
                int d = 15 - ticks;
                int c = a - d * 4;
                if (ticks > 8) {
                    a = (24 - ticks) * 8;
                    b = d * 20;
                }
                if (ticks > 14)
                    actor->animationPhase = 2;
                height = SceneActor_TerrainHeightLevel(
                             (struct ContactPosition *)actor->drawable.common.position)
                         << 8;
                position = (struct ContactPosition *)actor->drawable.common.position;
                position->y = height + b;
                DisplayAnimation_SetPalette(&actor->drawable.common.kind, c, a, 0);
                break;
            }
            case 2:
                DisplayAnimation_ClearFlags(&actor->drawable.common.kind, 2);
                actor->finished = 1;
                break;
        }
        SceneActorAnimation_SetAngle(actor, actor->direction);
        Actor_PlayAnimationEntry(actor, 0, 2, 0, 0);
    }
}

// Provisional name: Actor_OnRisingFadeUpdate (tentative)

void Actor_OnRisingFadeUpdate(void *unused, struct SceneActor *actor, int ticks)
{
    int a;
    int b;
    struct ContactPosition *position;
    int height;
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        DisplayAnimation_SetFlags(&actor->drawable.common.kind, 2);
        DisplayAnimation_SetPalette(&actor->drawable.common.kind, 0x40, 0x40, 0);
        Sound_PlaySong(0x15F);
    }
    a = 0x40;
    b = 0;
    switch (actor->animationPhase) {
        case 0: {
            int c = a - ticks * 4;
            if (ticks > 8) {
                a = ticks * 8;
                b = (ticks - 8) * 20;
            }
            if (ticks > 14)
                actor->animationPhase = 1;
            height = SceneActor_TerrainHeightLevel(
                         (struct ContactPosition *)actor->drawable.common.position)
                     << 8;
            position = (struct ContactPosition *)actor->drawable.common.position;
            position->y = height + b;
            DisplayAnimation_SetPalette(&actor->drawable.common.kind, c, a, 0);
            break;
        }
        case 1:
            b = ticks * 200 + 0xa0;
            height = SceneActor_TerrainHeightLevel(
                         (struct ContactPosition *)actor->drawable.common.position)
                     << 8;
            position = (struct ContactPosition *)actor->drawable.common.position;
            position->y = height + b;
            if (ticks > 7) {
                SceneActor_SetFlags(actor, 0x100);
                actor->animationPhase++;
            }
            break;
        case 2:
            actor->finished = 1;
            SceneActor_SetFlags(actor, 0x100);
            DisplayAnimation_ClearFlags(&actor->drawable.common.kind, 2);
            break;
    }
    SceneActorAnimation_SetAngle(actor, actor->direction);
    if (actor->variant != 5)
        Actor_PlayAnimationEntry(actor, 0, 2, 0, 0);
    actor->query(actor);
}

void Actor_OnConfiguredAnimationUpdate(void *unused, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, actor->modeA, actor->modeB, 0, 4);
    }
    SceneActor_SetFlags(actor, 4096);
    actor->query(actor);
}

// Provisional name: Actor_OnPathFollowUpdate (tentative)

struct SceneActor;

u32 Route_GetMovementTowardCurrentPoint(void *, void *, void *, void *, u32, u32, u32);

void Actor_OnPathFollowUpdate(void *unused, struct SceneActor *actor)
{
    struct SceneActorBody *body = &actor->body;
    struct SceneActorRouteCursor *route = &actor->route;
    struct ContactPosition *position = &actor->body.position;
    union SceneActorDisplay *drawable = &actor->drawable;
    if (actor->animationEntered) {
        u16 *point;
        actor->animationEntered = 0;
        actor->finished = 0;
        point = (u16 *)route->point;
        SceneActorAnimation_SetAngle(actor,
                                     Math_Atan2(point[0] - position->x, point[1] - position->z));
        Actor_PlayAnimationEntry(actor, 1, 1, 0, 4);
        actor->routeProgress = 0;
        actor->routeStepFinished = 0;
    }
    SceneActor_SetFlags(actor, 0x1000);
    if ((u8)actor->query(actor) != 1) {
        if (actor->routeStepFinished == 0) {
            if (drawable->common.kind == 2) {
                struct DisplayKeyframeState *keyframes = &actor->drawable.keyframe.keyframes;
                if (keyframes->status & 4) {
                    u32 speed;
                    if (keyframes->frame & 1)
                        speed = (u16)actor->radiusA >> 1;
                    else
                        speed = (u16)actor->radiusA;
                    actor->routeStepFinished =
                        Route_GetMovementTowardCurrentPoint(route, &body->position, &body->motion,
                                                            &body->direction, speed, speed, speed);
                }
            } else {
                u32 speed = (u16)actor->radiusA;
                actor->routeStepFinished = Route_GetMovementTowardCurrentPoint(
                    route, position, &actor->body.motion, &actor->body.direction, speed, speed,
                    speed);
            }
        }
        if (actor->routeStepFinished) {
            if (drawable->common.kind == 2) {
                struct DisplayKeyframeState *keyframes = &drawable->keyframe.keyframes;
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

static inline u32 AttachmentFlag(void)
{
    return PlayerSlot_ReadStatus(0, 4);
}

// confidence=low: tracks a target facing and enables contact response when positions and angles agree.

void Actor_UpdateTargetFacing(struct SceneActorManager *manager, struct SceneActor *actor)
{
    struct ContactPosition *pos = &actor->body.position;
    s32 facing;
    if (manager->target) {
        s32 angle = Math_Atan2(manager->target->x - pos->x, manager->target->z - pos->z);
        if (actor->animationEntries->kind) {
            s32 diff = (s8)(angle - actor->body.direction);
            if (diff > 8)
                SceneActorAnimation_SetAngle(actor, actor->body.direction + 8);
            else if (diff < -8)
                SceneActorAnimation_SetAngle(actor, actor->body.direction - 8);
            else
                SceneActorAnimation_SetAngle(actor, actor->body.direction + diff);
        }
        facing = (u8)manager->player->unk2D;
    } else
        facing = 0;
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, 0, 1, 0, 4);
        actor->routeProgress = 1;
        actor->savedAnimationAngle = actor->body.direction;
        if (manager->player && !TargetAllowsInteraction(manager->player) &&
            !(gEngineFlags & 0x200) && actor->enabled) {
            struct Player *player = manager->player;
            u8 tether;
            if (player->tetherMode == 6 && TetherActor_GetPosition())
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
    SceneActor_SetFlags(actor, 0x1000);
    if ((u8)actor->query(actor) != 1) {
        struct ContactPosition *target = manager->target;
        s32 radius = actor->rateC;
        u8 near;
        if (Ghoul_Abs(target->x - pos->x) < radius && Ghoul_Abs(target->y - pos->y) <= 255 &&
            Ghoul_Abs(target->z - pos->z) < radius)
            near = 1;
        else
            near = 0;
        if (near) {
            struct ContactPosition *from = manager->target;
            s32 direction = (s8)Math_Atan2(pos->x - from->x, pos->z - from->z);
            s32 delta = facing - direction;
            s32 behind = actor->body.direction + 128;
            u8 opposite = facing - behind;
            if (Ghoul_Abs((s8)delta) <= 63 && Ghoul_Abs((s8)opposite) <= 31)
                SceneActor_SetFlags(actor, 0x800);
        }
    }
}

void Actor_OnTimedIdleUpdate(void *unused, struct SceneActor *actor, int ticks)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, 0, 1, 0, 4);
        actor->routeProgress = 1;
    }
    SceneActor_SetFlags(actor, 4096);
    if ((u8)actor->query(actor) != 1 && ticks > 7)
        actor->finished = 1;
}

s32 DisplayAnimation_GetFrameBoundary(void *, u32 *);

// confidence=low: advances a navigation probe toward its target at animation frame boundaries.

void Actor_UpdateNavigationWalk(void *unused, struct SceneActor *actor)
{
    struct SceneActorBody *body = &actor->body;
    struct SceneActorNavigationProbe *probe = &actor->probe;
    union SceneActorDisplay *drawable = &actor->drawable;
    if (actor->animationEntered) {
        struct ContactPosition *pos;
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, 3, 1, 0, 4);
        actor->routeProgress = 1;
        pos = &actor->body.position;
        ProximityVolume_TranslateBounds(probe, pos);
        Terrain_ApplyBlocked(probe, pos, &actor->destination);
        actor->probeReady = 0;
        Actor_SetAttachmentEffect(actor, 0, 0, 0);
    }
    SceneActor_SetFlags(actor, 0x1000);
    if ((u8)actor->query(actor) != 1) {
        struct ContactPosition *pos = &actor->body.position;
        struct ContactPosition *current = &actor->probe.current;
        s32 x = probe->current.x - body->position.x;
        s32 z = current->z - pos->z;
        s32 distance = x * x + z * z;
        s32 limit = 0x1103;
        struct ContactPosition *from = pos;
        if (distance <= limit) {
            body->position.x = probe->target.x;
            body->position.z = probe->target.z;
            actor->finished = 1;
        } else {
            u32 frame;
            if (DisplayAnimation_GetFrameBoundary(drawable, &frame)) {
                s32 speed;
                struct ContactPosition step;
                if (drawable->common.kind == 2)
                    speed = (frame & 1) ? 32 : 64;
                else
                    speed = 16;
                SceneActorAnimation_SetAngle(actor, Math_Atan2(probe->target.x - body->position.x,
                                                               probe->target.z - from->z));
                SceneActor_SetChargeOffset(body->direction, &step, speed);
                body->position.x += step.x;
                body->position.z += step.z;
            }
        }
        NavMesh_UpdateProbe(probe, from);
        Actor_PlayFootstepSound(actor, 3);
        Actor_PlayAnimationEntry(actor, 3, 1, 0, 1);
    }
}

// confidence=low: walks toward the navigation target while the animation permits a fixed motion step.

void Actor_UpdateNavigationStep(void *unused, struct SceneActor *actor)
{
    struct SceneActorBody *body = &actor->body;
    struct SceneActorNavigationProbe *probe = &actor->probe;
    union SceneActorDisplay *drawable = &actor->drawable;
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, 1, 1, 0, 4);
        actor->routeProgress = 0;
        ProximityVolume_TranslateBounds(probe, &actor->body.position);
        Terrain_ApplyBlocked(probe, &actor->body.position, &actor->destination);
        actor->probeReady = 1;
    }
    SceneActor_SetFlags(actor, 0x1000);
    if ((u8)actor->query(actor) != 1) {
        struct ContactPosition *pos = &actor->body.position;
        struct ContactPosition *current = &actor->probe.current;
        s32 x = probe->current.x - body->position.x;
        s32 z = current->z - pos->z;
        s32 distance = x * x + z * z;
        s32 limit = 0x9C3;
        struct ContactPosition *from = pos;
        if (distance <= limit) {
            body->position.x = actor->probe.target.x;
            body->position.z = actor->probe.target.z;
            actor->finished = 1;
        } else if (drawable->common.kind == 2) {
            struct DisplayKeyframeState *keyframes = &actor->drawable.keyframe.keyframes;
            if (keyframes->status & 4) {
                struct ContactPosition *target = &actor->probe.target;
                struct ContactPosition step;
                SceneActorAnimation_SetAngle(
                    actor, Math_Atan2(probe->target.x - body->position.x, target->z - from->z));
                SceneActor_SetChargeOffset(body->direction, &step, 48);
                body->position.x += step.x;
                body->position.z += step.z;
            }
        }
        NavMesh_UpdateProbe(probe, from);
        Actor_PlayFootstepSound(actor, 1);
        Actor_PlayAnimationEntry(actor, 1, 1, 0, 1);
    }
}

void SceneActorAnimation_UpdateRouteWalk(void *unused, struct SceneActor *actor)
{
    struct SceneActorBody *body = &actor->body;
    struct SceneActorNavigationProbe *probe = &actor->probe;
    struct SceneActorRouteCursor *route = &actor->route;
    union SceneActorDisplay *drawable = &actor->drawable;
    if (actor->animationEntered) {
        struct ContactPosition path;
        struct ContactPosition *pos;
        struct ContactPosition *target;
        actor->animationEntered = 0;
        actor->finished = 0;
        actor->routeProgress = 0;
        pos = &actor->body.position;
        ProximityVolume_TranslateBounds(probe, pos);
        Route_Start(route, actor->routeId, actor->routeMode, actor->routePoint);
        Path_GetPointPosition(&path, actor->route.point, 0);
        Terrain_ApplyBlocked(probe, pos, &path);
        target = &actor->probe.target;
        SceneActorAnimation_SetAngle(
            actor, Math_Atan2(probe->target.x - body->position.x, target->z - pos->z));
        Actor_PlayAnimationEntry(actor, 1, 1, 0, 4);
    }
    SceneActor_SetFlags(actor, 0x1000);
    if ((u8)actor->query(actor) != 1) {
        struct ContactPosition *pos = &actor->body.position;
        struct ContactPosition *current = &actor->probe.current;
        s32 x = probe->current.x - body->position.x;
        s32 z = current->z - pos->z;
        s32 distance = x * x + z * z;
        s32 limit = 0x9C3;
        struct ContactPosition *from = pos;
        if (distance <= limit) {
            body->position.x = actor->probe.target.x;
            body->position.z = actor->probe.target.z;
            actor->finished = 1;
        } else if (drawable->common.kind == 2) {
            struct DisplayKeyframeState *keyframes = &actor->drawable.keyframe.keyframes;
            if (keyframes->status & 4) {
                struct ContactPosition *target = &actor->probe.target;
                struct ContactPosition step;
                s32 speed;
                SceneActorAnimation_SetAngle(
                    actor, Math_Atan2(probe->target.x - body->position.x, target->z - from->z));
                speed = (keyframes->frame & 1) ? 24 : 48;
                SceneActor_OffsetAlongHeading(&step, body->direction, speed);
                body->position.x += step.x;
                body->position.z += step.z;
            }
        }
        NavMesh_UpdateProbe(probe, from);
        Actor_PlayFootstepSound(actor, 1);
        Actor_PlayAnimationEntry(actor, 1, 1, 0, 1);
    }
}

void Actor_UpdateRunToPosition(void *unused, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, 3, 1, 0, 4);
    }
    if ((u8)actor->query(actor) != 1) {
        struct ContactPosition *pos = &actor->body.position;
        struct ContactPosition *target = &actor->target;
        int dx = target->x - pos->x;
        int dz = target->z - pos->z;
        u32 distance = dx * dx + dz * dz;
        u32 limit = actor->distanceSquared;
        struct ContactPosition *from = pos;
        struct ContactPosition *to = target;
        if (distance <= limit) {
            actor->body.position = actor->target;
            actor->finished = 1;
        } else {
            SceneActor_OffsetAlongHeading(&actor->body.motion, actor->body.direction,
                                          actor->distance);
            actor->direction = Math_Atan2(to->x - from->x, to->z - from->z);
            SceneActorAnimation_SetAngle(actor, actor->direction);
            Actor_PlayAnimationEntry(actor, 3, 1, 0, 1);
        }
        Actor_PlayFootstepSound(actor, 3);
    }
}

int Path_GetPointFacing(struct SceneActorRoutePoint *, int);

int Actor_SelectPathNodeAction(struct SceneActor *actor, struct SceneActorRoutePoint *node)
{
    int kind = node->attributes >> 4;
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

static inline void SceneCharacter_EnterState(struct SceneActor *actor, u8 state)
{
    actor->state = state;
    actor->ticks = 0;
    actor->commandEntered = 1;
}

static inline void SceneCharacter_InterruptForTarget(struct SceneActor *actor)
{
    actor->savedState = actor->state;
    SceneCharacter_EnterState(actor, 10);
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
    if (actor->commandEntered != 0) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 3;
    }
    if (actor->finished != 0) {
        SceneCharacter_EnterState(actor, 1);
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    actor->ticks += 1;
}

static inline int SceneActor_IsNearTarget(struct SceneActor *actor, struct ContactPosition *b)
{
    struct ContactPosition *a = &actor->body.position;
    int range = actor->rateA;
    if (Abs(b->x - a->x) < range && Abs(b->y - a->y) <= 255 && Abs(b->z - a->z) < range)
        return 1;
    return 0;
}

static inline int SceneActor_HasNearbyTarget(struct SceneActorManager *manager,
                                             struct SceneActor *actor)
{
    if (manager->target && SceneActor_IsNearTarget(actor, manager->target))
        return 1;
    return 0;
}

void SceneCharacter_UpdateTurnState(void *unused, struct SceneActor *actor)
{
    if (actor->commandEntered) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 4;
    }
    if (actor->finished) {
        int controlled = actor->controlled;
        if (controlled) {
            SceneCharacter_EnterState(actor, actor->savedState);
        } else {
            SceneCharacter_EnterState(actor, 1);
        }
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    actor->ticks++;
}

void SceneCharacter_UpdateAlternateOneShotActionState(void *unused, struct SceneActor *actor)
{
    if (actor->commandEntered != 0) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 5;
    }
    if (actor->finished != 0) {
        SceneCharacter_EnterState(actor, 1);
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    actor->ticks += 1;
}

void SceneCharacter_UpdateStopState(void *unused, struct SceneActor *actor)
{
    if (actor->commandEntered) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 6;
    }
    if (actor->finished) {
        SceneCharacter_EnterState(actor, 0);
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    actor->ticks++;
}

void SceneCharacter_UpdateProximityWaitState(struct SceneActorManager *manager,
                                             struct SceneActor *actor)
{
    if (actor->commandEntered) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 2;
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    if (SceneActor_HasNearbyTarget(manager, actor)) {
        SceneCharacter_InterruptForTarget(actor);
    } else
        actor->ticks++;
}

void SceneCharacter_UpdatePathWaitState(struct SceneActorManager *manager, struct SceneActor *actor)
{
    struct SceneActorRouteCursor *route = &actor->route;
    if (actor->commandEntered) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 2;
    }
    if (!(actor->ticks & 15)) {
        s8 *firstRead = (s8 *)&actor->hasDestination;
        int flag = *firstRead;
        s8 *hasDestination = firstRead;
        if (flag) {
            struct ContactPosition *tether = TetherActor_GetPosition();
            int nearby = 0;
            if (tether) {
                struct ContactPosition *pos = &actor->body.position;
                int dx = pos->x - tether->x;
                int dz = pos->z - tether->z;
                if (dx * dx + dz * dz < 160000)
                    nearby = 1;
            }
            if (nearby)
                actor->cooldown = 1800;
            else if (*hasDestination && manager->weather)
                actor->cooldown = 300;
            else
                goto checkSchedule;
            SceneCharacter_EnterState(actor, 12);
            return;
        }
    checkSchedule:
        if (actor->hasParameters && manager->minuteChanged) {
            if ((manager->hour == actor->paramC && manager->minute >= actor->paramD) ||
                (manager->hour - 1 == actor->paramC && manager->minute <= actor->paramD)) {
                SceneCharacter_EnterState(actor, 13);
                return;
            }
        }
    }
    if (SceneActor_HasNearbyTarget(manager, actor)) {
        SceneCharacter_InterruptForTarget(actor);
        return;
    }
    if (actor->routeId >= 0) {
        if (!actor->nodeId) {
            Route_AdvancePoint(route);
            SceneCharacter_EnterState(actor, SceneCharacter_GetPathNodeState(actor));
        } else {
            actor->nodeId--;
        }
    }
    actor->ticks++;
}

void SceneCharacter_UpdatePathTurnState(struct SceneActorManager *manager, struct SceneActor *actor)
{
    int close;
    if (actor->commandEntered) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 4;
        actor->direction = (actor->nodeFacing + 5) << 5;
    }
    close = SceneActor_HasNearbyTarget(manager, actor);
    if (close) {
        SceneCharacter_InterruptForTarget(actor);
    } else if (actor->finished) {
        SceneCharacter_EnterState(actor, 7);
    } else
        actor->ticks++;
}

void SceneCharacter_UpdatePathActionState(struct SceneActorManager *manager,
                                          struct SceneActor *actor)
{
    if (actor->commandEntered) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 8;
    }
    if (!(actor->ticks & 15)) {
        s8 *firstRead = (s8 *)&actor->hasDestination;
        int flag = *firstRead;
        s8 *hasDestination = firstRead;
        if (flag) {
            struct ContactPosition *tether = TetherActor_GetPosition();
            int nearby = 0;
            if (tether) {
                struct ContactPosition *pos = &actor->body.position;
                int dx = pos->x - tether->x;
                int dz = pos->z - tether->z;
                if (dx * dx + dz * dz < 160000)
                    nearby = 1;
            }
            if (nearby)
                actor->cooldown = 1800;
            else if (*hasDestination && manager->weather)
                actor->cooldown = 300;
            else
                goto checkSchedule;
            SceneCharacter_EnterState(actor, 12);
            return;
        }
    checkSchedule:
        if (actor->hasParameters && manager->minuteChanged) {
            if ((manager->hour == actor->paramC && manager->minute >= actor->paramD) ||
                (manager->hour - 1 == actor->paramC && manager->minute <= actor->paramD)) {
                SceneCharacter_EnterState(actor, 13);
                return;
            }
        }
    }
    if (SceneActor_HasNearbyTarget(manager, actor)) {
        SceneCharacter_InterruptForTarget(actor);
        return;
    }
    if (actor->finished) {
        SceneCharacter_EnterState(actor, 7);
        return;
    }
    actor->ticks++;
}

static inline int SceneActor_IsNearBodyPosition(struct SceneActor *actor, struct ContactPosition *b,
                                                struct SceneActorBody *body)
{
    struct ContactPosition *a = &actor->body.position;
    int range = actor->rateB;
    if (Abs(b->x - body->position.x) < range && Abs(b->y - a->y) <= 255 && Abs(b->z - a->z) < range)
        return 1;
    return 0;
}

void SceneCharacter_UpdateTargetProximityState(struct SceneActorManager *manager,
                                               struct SceneActor *actor)
{
    struct SceneActorBody *body = &actor->body;
    if (actor->commandEntered) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 9;
    }
    if (manager->player && manager->target) {
        if (!SceneActor_IsNearBodyPosition(actor, manager->target, body)) {
            if (actor->registered) {
                actor->direction = actor->savedAngle;
                SceneCharacter_EnterState(actor, 3);
            } else {
                SceneCharacter_EnterState(actor, actor->savedState);
            }
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
        SceneCharacter_EnterState(actor, 0xc);
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
        SceneCharacter_EnterState(actor, 0xE);
    }
    actor->ticks += 1;
}

void SceneCharacter_UpdatePhaseTransitionState(struct SceneActorManager *manager,
                                               struct SceneActor *actor)
{
    int close;
    if (actor->commandEntered) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 12;
    }
    close = SceneActor_HasNearbyTarget(manager, actor);
    if (close) {
        SceneCharacter_InterruptForTarget(actor);
    } else {
        if (actor->finished) {
            SceneCharacter_EnterState(actor, 14);
        }
        actor->ticks++;
    }
}

void SceneCharacter_UpdatePhaseWindowWaitState(struct SceneActorManager *manager,
                                               struct SceneActor *actor)
{
    if (actor->commandEntered) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 1;
        Trigger_ConsumePending(actor->messageReceiver, 1);
    }
    if (!actor->cooldown) {
        if (!manager->weather) {
            if ((manager->hour == actor->paramA && manager->minute >= actor->paramB) ||
                (manager->hour > actor->paramA && manager->hour < actor->paramC) ||
                (manager->hour == actor->paramC && manager->minute < actor->paramD)) {
                SceneCharacter_EnterState(actor, 15);
                return;
            }
        }
    } else
        actor->cooldown--;
    actor->ticks++;
}

void SceneCharacter_UpdatePathResumeState(struct SceneActorManager *manager,
                                          struct SceneActor *actor)
{
    if (actor->commandEntered) {
        actor->commandEntered = 0;
        actor->finished = 0;
        actor->action = 13;
    }
    if (SceneActor_HasNearbyTarget(manager, actor)) {
        SceneCharacter_InterruptForTarget(actor);
    } else {
        if (actor->finished)
            SceneCharacter_EnterState(actor, SceneCharacter_GetPathNodeState(actor));
        actor->ticks++;
    }
}
