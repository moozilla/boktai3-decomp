#include "effects/bat_flock.h"
#include "engine/video/display_sequence_inline.h"
#include "engine/math/monster_particle_random.h"

static inline void BatFlockBat_SetState(struct GroupPiece *piece, u32 state)
{
    piece->state = state;
    piece->active = 1;
    piece->timer = 0;
}

static inline void BatFlockBat_SelectPalette(struct SpriteRenderResource *resource)
{
    AnimData_SelectPaletteBank(resource, 0x2D3);
}
int BatFlockBat_Initialize(struct GroupObjects *owner, struct GroupPiece *p, int index)
{
    struct SpriteRenderResource *resource;
    struct DisplaySequenceSprite *sprite;
    u16 initialTimer[] = {0};

    p->state = 0;
    p->active = 1;
    p->timer = initialTimer[0];
    p->direction = 0;
    resource = &p->resource;
    AnimData_Load(resource, 0xDA6D);
    BatFlockBat_SelectPalette(resource);
    resource->palette = owner->palette;
    sprite = &p->sprite;
    Sprite_Init(sprite, resource, 1);
    sprite->priority = 2;
    ZeroFill8(&sprite->position);
    Shadow_Initialize(p->transform, &sprite->position, 0);
    ActorShadow_Disable(p->transform);
}

u32 BatFlockBat_Destroy(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    Shadow_Destroy(p->transform);
    Sprite_Remove(&p->sprite);
}

static inline void BatFlockBat_GetFacingFlips(u8 quadrant, u8 *flipX, u8 *flipY)
{
    if (quadrant == 1 || quadrant == 2)
        *flipX = 1;
    else
        *flipX = 0;
    if (quadrant >= 2)
        *flipY = 1;
    else
        *flipY = 0;
}

void BatFlockBat_SetFacingAnim(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    u32 quadrant = (((u16)p->direction + 0x60) >> 6) & 3;
    u8 flipX;
    u8 flipY;

    BatFlockBat_GetFacingFlips(quadrant, &flipX, &flipY);
    Anim_SetSequence(&p->anim, m->animation, (u16)index, flipX, flipY);
}

void BatFlockBat_UpdateAnimation(struct GroupPiece *p)
{
    struct DisplaySequenceSprite *sprite = &p->sprite;
    struct DisplaySequenceState *anim;
    const u16 *command;
    int done;

    p->sprite.position = p->position;
    anim = &p->anim;
    command = &anim->commands[anim->index];
    DisplaySequence_ApplyFrame((struct SceneActorControlMotionDrawable *)sprite, anim, command);
    if (++anim->elapsed >= anim->duration) {
        anim->elapsed = 0;
        if (anim->flags & 4) {
            if (anim->index == 0) {
                if (anim->flags & 8)
                    anim->elapsed = anim->duration;
                else
                    anim->index = anim->frameCount - 1;
                done = TRUE;
            } else {
                anim->index--;
                done = FALSE;
            }
        } else if (++anim->index >= anim->frameCount) {
            if ((u8)(anim->flags & 8)) {
                anim->elapsed = anim->duration;
                anim->index = anim->frameCount - 1;
            } else {
                anim->index = 0;
            }
            done = TRUE;
        } else {
            done = FALSE;
        }
        command = &anim->commands[anim->index];
        DisplaySequence_UpdateWeight(anim, command);
        if (anim->duration == 0)
            anim->duration = 1;
    } else {
        done = FALSE;
    }
    p->done = done;
    if (p->position.y > p->targetY)
        p->position.y--;
    else if (p->position.y < p->targetY)
        p->position.y++;
    sprite->position.y += scale(gUnk_086149C4[p->angle] * 32);
    p->angle += 4;
}

static inline void addPositionAt(struct ContactPosition *d, struct ContactPosition *dst,
                                 struct ContactPosition *src)
{
    d->x += src->x;
    dst->y += src->y;
    dst->z += src->z;
}

static inline int BatFlockBats_RandomValue(void)
{
    int result;
    const u16 *table;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    table = gRandomTable;
    PARTICLE_RANDOM_READ_SAMPLE(result, table, gRandomIndex);
    return result;
}

static inline void addPosition(struct ContactPosition *d, struct ContactPosition *s)
{
    d->x = d->x + s->x;
    d->y = d->y + s->y;
    d->z = d->z + s->z;
}

static inline int BatFlock_AngleTo(const struct ContactPosition *from,
                                   const struct ContactPosition *to)
{
    return Math_Atan2(to->x - from->x, to->z - from->z);
}

static inline void BatFlock_BlendPalette(struct GroupObjects *m, int weight)
{
    u8 *from = gPaletteBankData + 0x5A60;
    u8 *to = gPaletteBankData + 0x5AA0;

    Palette_InterpolateBanks(m->palette, from, to, weight, 6);
}

static inline int BatFlock_Random(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
}

int BatFlockBats_StartStraightAdvance(struct GroupObjects *m, struct GroupQueue *q, struct GroupCommand *cmd)
{
    struct ContactPosition origin;
    int i;
    int duration;
    int startRadius;
    int targetRadius;
    int direction;
    int spread;
    struct GroupPiece *p;
    struct GroupPiece *piece;

    origin.x = cmd->originX;
    origin.y = cmd->originY;
    origin.z = cmd->originZ;
    duration = cmd->radius0;
    startRadius = cmd->radius1;
    targetRadius = cmd->radius2;
    direction = cmd->direction;
    spread = cmd->spread;
    m->origin = origin;
    m->timer = 0;
    m->paletteActive = 1;
    m->paletteMask = 63;
    BatFlock_BlendPalette(m, 63);
    piece = m->pieces;

    for (i = 0; i < 16; i++, piece++) {
        p = piece;
        BatFlockBat_SetState(p, 5);
        p->direction = direction;
        if (spread != 0)
            p->direction += Mod(BatFlockBats_RandomValue(), spread);
        p->duration = duration;
        polar(&piece->position, p->direction, startRadius);
        addPositionAt(&p->position, &piece->position, &origin);
        polar(&piece->target, p->direction, targetRadius);
        addPositionAt(&p->target, &piece->target, &origin);
        p->angle = BatFlockBats_RandomValue();
        p->targetY = i * 16 + p->position.y;
        p->speed = (BatFlockBats_RandomValue() & 7) + 7;
        p->sprite.position = p->position;
        p->sprite.flags &= ~1;
        BatFlockBat_SetFacingAnim(m, piece, 0);
        AnimData_SelectPaletteBank(&piece->resource, 0x2D3);
        p->resource.palette = m->palette;
        ActorShadow_Enable(piece->transform);
    }

    m->active = 1;
    return 0;
}

int BatFlockBats_StartLaunch(struct GroupObjects *m, struct GroupQueue *q, struct GroupCommand *cmd)
{
    struct ContactPosition origin;
    int i;
    int duration;
    int startRadius;
    int targetRadius;
    int direction;
    int spread;
    struct GroupPiece *p;
    struct GroupPiece *piece;
    origin.x = cmd->originX;
    origin.y = cmd->originY;
    origin.z = cmd->originZ;
    duration = cmd->radius0;
    startRadius = cmd->radius1;
    targetRadius = cmd->radius2;
    direction = cmd->direction;
    spread = cmd->spread;
    m->origin = origin;
    m->timer = 0;
    m->paletteActive = 1;
    m->paletteMask = 63;
    BatFlock_BlendPalette(m, 63);
    piece = m->pieces;
    for (i = 0; i < 16; i++, piece++) {
        p = piece;
        BatFlockBat_SetState(p, 0);
        p->direction = direction;
        if (spread != 0)
            p->direction += Mod(randomValue(), spread);
        p->duration = duration;
        polar(&piece->position, p->direction, startRadius);
        addPositionAt(&p->position, &piece->position, &origin);
        polar(&piece->target, p->direction, targetRadius);
        addPositionAt(&p->target, &piece->target, &origin);
        p->angle = BatFlockBats_RandomValue();
        p->targetY = i * 16 + p->position.y;
        p->speed = (BatFlockBats_RandomValue() & 7) + 7;
        p->sprite.position = p->position;
        p->sprite.flags &= ~1;
        BatFlockBat_SetFacingAnim(m, piece, 0);
        AnimData_SelectPaletteBank(&piece->resource, 0x2D3);
        p->resource.palette = m->palette;
        ActorShadow_Enable(piece->transform);
    }

    m->active = 1;
    return 0;
}

s32 Div(s32, s32);
struct PackedVec {
    u32 x : 16, y : 16, z : 16;
};

static inline void BatFlock_Subtract(struct ContactPosition *d, const struct ContactPosition *a,
                                     const struct ContactPosition *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}

// confidence=low: converges the board pieces on the command origin at a scaled speed.
int BatFlockBats_StartConvergence(struct GroupObjects *m, struct GroupQueue *q,
                                 struct GroupCommand *cmd)
{
    struct ContactPosition origin;
    struct ContactPosition delta;
    int i;
    int frames;
    void *palette;
    struct GroupPiece *p;
    struct GroupPiece *piece;

    origin.x = cmd->originX;
    origin.y = cmd->originY;
    origin.z = cmd->originZ;
    frames = cmd->radius0;
    if (frames <= 0)
        frames = 1;
    m->origin = origin;
    m->timer = 0;
    piece = m->pieces;
    i = 0;
    palette = m->palette;
    for (; i < 16; i++, piece++) {
        int angle;

        p = piece;
        BatFlockBat_SetState(p, 7);
        BatFlock_Subtract(&delta, &origin, &p->position);
        angle = Math_Atan2(delta.x, delta.z);
        p->duration = Div(scale(delta.x * gUnk_086149C4[(angle + 64) & 255] +
                                delta.z * gUnk_086149C4[angle & 255]),
                          frames);
        p->target = origin;
        {
            union RandomTableView table = {gRandomTable};
            p->angle = randomWith(table.samples);
            p->targetY = i * 16 + p->position.y;
            p->speed = (randomWith(table.samples) & 7) + 7;
        }
        p->sprite.position = p->position;
        p->sprite.flags &= ~1;
        BatFlockBat_SetFacingAnim(m, piece, 0);
        AnimData_SelectPaletteBank(&piece->resource, 0x2D3);
        p->resource.palette = palette;
        ActorShadow_Enable(piece->transform);
    }
    m->active = 1;
    return 0;
}

static inline int BatFlockBats_NextRandomValue(const u16 *table, u32 *index, u32 mask)
{
    u32 offset;
    *index = (*index + 1) & mask;
    offset = *index * 2;
    return *(const u16 *)(offset + (u32)table);
}

int BatFlockBats_StartActorChase(struct GroupObjects *m, struct GroupQueue *q, struct GroupCommand *cmd)
{
    struct SceneActorBody *actor;
    s32 duration;
    struct ContactPosition position;
    int i;
    struct GroupPiece *p;
    u32 *index;
    u32 mask;
    m->actorId = cmd->originX;
    actor = SceneBody_GetById(m->actorId);
    if (!actor)
        return -1;
    position = actor->position;
    duration = (s16)cmd->originY;
    m->timer = 0;
    index = &gRandomIndex;
    mask = 1023;
    for (i = 15, p = m->pieces; i >= 0; i--, p++) {
        int angle;
        int random;
        BatFlockBat_SetState(p, 3);
        p->target = position;
        angle = Math_Atan2(p->target.x - p->position.x, p->target.z - p->position.z);
        p->direction = angle;
        {
            union RandomTableView table = {gRandomTable};
            random = BatFlockBats_NextRandomValue(table.samples, index, mask);
            angle -= 16;
            p->direction = angle + (random & 31);
            p->duration = duration;
            p->speed = (BatFlockBats_NextRandomValue(table.samples, index, mask) & 7) + 7;
        }
    }
    m->active = 1;
    return 0;
}

static inline int GroupQueue_GetCount(struct GroupQueue *queue)
{
    return queue->count[gSceneEventDispatchState->bank];
}

int BatFlockBats_DispatchCommands(struct GroupObjects *manager)
{
    struct GroupQueue *queue = &manager->queue;
    int i;

    for (i = 0; i < GroupQueue_GetCount(queue); i++) {
        struct GroupGlobals *dispatch = gSceneEventDispatchState;
        struct GroupCommand *cmd = queue->commands[dispatch->bank][i];

        SceneMessages_SetPendingTrigger(queue, cmd);
        switch (cmd->type) {
            case 0:
                BatFlockBats_StartStraightAdvance(manager, queue, cmd);
                Trigger_ConsumePending(&manager->queue, 1);
                break;
            case 1:
                BatFlockBats_StartLaunch(manager, queue, cmd);
                Trigger_ConsumePending(&manager->queue, 1);
                break;
            case 2:
                BatFlockBats_StartActorChase(manager, queue, cmd);
                Trigger_ConsumePending(&manager->queue, 1);
                break;
            case 11:
                BatFlockBats_StartConvergence(manager, queue, cmd);
                Trigger_ConsumePending(&manager->queue, 1);
                break;
        }
    }
    return 0;
}

static inline struct ContactPosition *addPositionN(struct ContactPosition *d,
                                                   struct ContactPosition *s)
{
    struct ContactPosition *src = s;
    struct ContactPosition *dst = d;
    d->x += s->x;
    dst->y += src->y;
    dst->z += src->z;
    return dst;
}

void BatFlockBat_Launch(struct GroupObjects *m, struct GroupPiece *p, int delay)
{
    struct ContactPosition delta;
    if (p->active)
        p->active = 0;
    polar(&delta, p->direction, p->duration * 2);
    addPosition(&p->position, &delta);
    BatFlockBat_SetFacingAnim(m, p, 0);
    if (p->timer >= delay + 2) {
        p->state = 1;
        p->active = 1;
        p->timer = 0;
    }
    p->timer++;
}

void BatFlockBat_Wander(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct ContactPosition delta;
    const u16 *table;

    if (p->active) {
        p->active = 0;
        p->heading = p->direction =
            Math_Atan2(p->target.x - p->position.x, p->target.z - p->position.z);
        // agbcc weights a register's uses by loop depth: four empty loops make
        // `table` outrank the random-index address and mask, so it gets r2
        // (found by tools/translate.py; see docs/PERMUTER.md).
        do {
            do {
                do {
                    do {
                        table = gRandomTable;
                    } while (0);
                } while (0);
            } while (0);
        } while (0);
        if (randomByte(table) <= 127)
            p->heading += 64 - (randomWith(table) & 63);
        else
            p->heading += 64 + (randomWith(table) & 63);
        p->speed = (randomValue() & 3) + 4;
    }
    if (p->speed != 0) {
        polar(&delta, p->heading, p->duration);
        addPosition(&p->position, &delta);
        p->direction = BatFlock_AngleTo(&p->position, &m->origin);
        p->speed--;
    }
    BatFlockBat_SetFacingAnim(m, p, 0);
    if (p->timer >= 96)
        BatFlockBat_SetState(p, 1);
    p->timer++;
}

void BatFlockBat_ApproachTarget(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct ContactPosition delta;
    int dx, dz;
    struct ContactPosition *position;

    if (p->active) {
        const u16 *table;

        p->active = 0;
        p->direction = BatFlock_AngleTo(&p->position, &m->origin);
        p->heading = BatFlock_AngleTo(&p->position, &p->target);
        table = gRandomTable;
        p->heading += (randomWith(table) & 31) - 16;
        p->speed = (randomWith(table) & 15) + 16;
    }
    position = &p->position;
    dx = p->target.x - p->position.x;
    dz = p->target.z - position->z;
    if (dx * dx + dz * dz < 200 * 200) {
        BatFlockBat_SetState(p, 2);
        return;
    }
    if (p->speed == 0) {
        const u16 *table;

        p->heading = Math_Atan2(dx, dz);
        table = gRandomTable;
        p->heading += (randomWith(table) & 31) - 16;
        p->speed = (randomWith(table) & 31) + 32;
    } else {
        p->speed--;
    }
    polar(&delta, p->heading, p->duration);
    addPosition(&p->position, &delta);
    p->direction = BatFlock_AngleTo(&p->position, &m->origin);
    BatFlockBat_SetFacingAnim(m, p, 0);
    p->timer++;
}

void BatFlockBat_ConvergeOnTarget(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct ContactPosition delta;
    int dx, dz;
    struct ContactPosition *position;

    if (p->active) {
        const u16 *table;

        p->active = 0;
        p->direction = BatFlock_AngleTo(&p->position, &m->origin);
        p->heading = BatFlock_AngleTo(&p->position, &p->target);
        table = gRandomTable;
        p->heading += (randomWith(table) & 31) - 16;
        p->speed = (randomWith(table) & 15) + 16;
    }
    position = &p->position;
    dx = p->target.x - p->position.x;
    dz = p->target.z - position->z;
    if (dx * dx + dz * dz < 200 * 200) {
        BatFlockBat_SetState(p, 8);
        return;
    }
    if (p->speed == 0) {
        const u16 *table;

        p->heading = Math_Atan2(dx, dz);
        table = gRandomTable;
        p->heading += (randomWith(table) & 31) - 16;
        p->speed = (randomWith(table) & 31) + 32;
    } else {
        p->speed--;
    }
    polar(&delta, p->heading, p->duration);
    addPosition(&p->position, &delta);
    p->direction = BatFlock_AngleTo(&p->position, &m->origin);
    BatFlockBat_SetFacingAnim(m, p, 0);
    p->timer++;
}

void BatFlockBat_Deactivate(struct GroupObjects *m, struct GroupPiece *p)
{
    if (p->active != 0) {
        p->active = 0;
        p->sprite.flags |= 1;
        ActorShadow_Disable(p->transform);
    }
}

void BatFlockBat_ChasePlayer(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct ContactPosition delta;
    struct ContactPosition *position;
    struct ContactPosition *target;
    int dx, dz;
    int active;

    if (m->actor)
        p->target = m->actor->position;
    active = p->active;
    position = &p->position;
    target = &p->target;
    if (active) {
        const u16 *table;

        p->active = 0;
        p->direction = BatFlock_AngleTo(position, target);
        table = gRandomTable;
        p->direction += (randomWith(table) & 31) - 16;
        p->speed = (randomWith(table) & 15) + 16;
    }
    if (p->speed == 0) {
        const u16 *table;

        p->direction = BatFlock_AngleTo(position, target);
        table = gRandomTable;
        p->direction += (randomWith(table) & 15) - 8;
        p->speed = (randomWith(table) & 15) + 16;
    } else {
        p->speed--;
    }
    dx = p->target.x - p->position.x;
    dz = target->z - position->z;
    if (dx * dx + dz * dz < 64 * 64) {
        BatFlockBat_SetState(p, 4);
        return;
    }
    polar(&delta, p->direction, p->duration);
    addPositionAt(&p->position, position, &delta);
    BatFlockBat_SetFacingAnim(m, p, 0);
    p->timer++;
}

void BatFlockBat_AdvanceWithOffset(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct ContactPosition delta;
    struct ContactPosition *offset;
    int active;
    if (m->actor)
        p->target = m->actor->position;
    active = p->active;
    offset = &p->offset;
    if (active) {
        p->active = 0;
        p->anchor = p->position;
        p->direction = BatFlock_Random();
        ZeroFill8(offset);
    }
    Vec16_Lerp(&p->anchor, &p->anchor, &p->target, index + 8, 5);
    if (p->speed == 0) {
        const u16 *table;
        p->direction = Math_Atan2(-p->offset.x, -p->offset.z);
        table = (const u16 *)RANDOM_TABLE_ADDR;
        p->direction += (randomWith(table) & 15) - 7;
        p->speed = (randomWith(table) & 7) + 8;
    } else
        p->speed--;
    polar(&delta, p->direction, p->duration);
    {
        struct ContactPosition *src = &delta;
        p->offset.x += delta.x;
        offset->y += src->y;
        offset->z += src->z;
    }
    p->position.x = p->anchor.x + p->offset.x;
    p->position.z = p->anchor.z + p->offset.z;
    BatFlockBat_SetFacingAnim(m, p, 0);
    p->timer++;
}
void BatFlockBat_AdvanceStraight(struct GroupObjects *m, struct GroupPiece *p, int delay)
{
    struct ContactPosition delta;
    if (p->active)
        p->active = 0;
    polar(&delta, p->direction, p->duration * 2);
    addPosition(&p->position, &delta);
    BatFlockBat_SetFacingAnim(m, p, 0);
    if (p->timer >= delay + 2) {
        p->state = 6;
        p->active = 1;
        p->timer = 0;
    }
    p->timer++;
}
void BatFlockBat_HomeToTarget(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct ContactPosition delta;
    struct ContactPosition *position;
    int dx, dz;

    if (p->active) {
        const u16 *table;

        p->active = 0;
        p->direction = BatFlock_AngleTo(&p->position, &p->target);
        table = gRandomTable;
        p->direction += (randomWith(table) & 31) - 16;
        p->speed = (randomWith(table) & 15) + 16;
    }
    position = &p->position;
    dx = p->target.x - p->position.x;
    dz = p->target.z - position->z;
    if (dx * dx + dz * dz < 200 * 200) {
        BatFlockBat_SetState(p, 2);
        return;
    }
    if (p->speed == 0) {
        const u16 *table;

        p->direction = Math_Atan2(dx, dz);
        table = gRandomTable;
        p->direction += (randomWith(table) & 31) - 16;
        p->speed = (randomWith(table) & 31) + 32;
    } else {
        p->speed--;
    }
    polar(&delta, p->direction, p->duration);
    addPosition(&p->position, &delta);
    BatFlockBat_SetFacingAnim(m, p, 0);
    p->timer++;
}

int BatFlock_Update(struct GroupObjects *m)
{
    int i;
    struct GroupPiece *p;
    BatFlockBats_DispatchCommands(m);
    if (m->active) {
        if (m->actorId)
            m->actor = SceneBody_GetById(m->actorId);
        if (m->paletteActive == 0) {
            if (++m->paletteMask >= 63) {
                m->paletteMask = 63;
                m->paletteActive = -1;
            }
        } else if (m->paletteActive == 1) {
            if (--m->paletteMask <= 0) {
                m->paletteMask = 0;
                m->paletteActive = -1;
            }
        } else if (m->paletteActive == 2) {
            if (++m->paletteMask > 31) {
                m->paletteMask = 32;
                m->paletteActive = -1;
            }
        }
        if (m->paletteActive >= 0) {
            void *first = gPaletteBankData + 0x5A60;
            void *second = gPaletteBankData + 0x5AA0;
            Palette_InterpolateBanks(m->palette, first, second, m->paletteMask, 6);
        }
        p = m->pieces;
        for (i = 0; i < 16; i++, p++) {
            gUnk_08604B48[p->state](m, p, i);
            BatFlockBat_UpdateAnimation(p);
        }
        m->timer++;
    }
    return 0;
}

int BatFlock_Destroy(struct GroupObjects *m)
{
    struct GroupPiece *p = m->pieces;
    int i;
    for (i = 0; i < 16; i++, p++)
        BatFlockBat_Destroy(m, p, i);
    SceneMessages_UnregisterReceiver(&m->queue);
    return 0;
}

int BatFlock_Initialize(struct GroupObjects *m, int id)
{
    struct GroupPiece *p;
    int i;
    m->active = 0;
    m->actorId = 0;
    m->actor = 0;
    m->animation = ScriptProc_Start(0x922E, 0x5BB7);
    {
        void *first = gPaletteBankData + 0x5A60;
        void *second = gPaletteBankData + 0x5AA0;
        Palette_InterpolateBanks(m->palette, first, second, 64, 6);
    }
    p = m->pieces;
    for (i = 0; i < 16; i++, p++)
        BatFlockBat_Initialize(m, p, i);
    SceneMessages_RegisterReceiver(&m->queue, id, 10);
    return 0;
}

void *BatFlock_CreateTask(void *arg)
{
    void *obj;
    obj = Task_Create(11, 0xD08);
    if (obj != 0) {
        Task_SetCallbacks(obj, BatFlock_Update, BatFlock_Destroy);
        if (BatFlock_Initialize(obj, (int)arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
