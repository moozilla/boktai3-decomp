#include "vanargand_parts.h"

// confidence=low: periodically retargets the part beside the player for 64 ticks.

void OrbitMotion_BeginDistanceWave(void *, const void *, const void *, s32, s32);

void MotionTrack_Update(void *);

void Vanargand_EmitPartTrail(void *);

void AnimatedParticles_EmitBurst(void *, s32, s32, s32, s32, s32, s32, s32);

extern const s16 gUnk_086149C4[];

void Vanargand_UpdatePartTracking(struct VanargandAttackPair *boss, struct PairedSceneHalf *p)
{
    struct ContactPosition delta;
    u32 mask = 8;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        p->actionCounters[0] = 0;
        p->actionCounters[1] = 0;
        p->actionEntered = 1;
        Anim_SetSequence(&p->graphics.sequence, boss->effectResource, 8, (u8)p->mode, 0);
    }
    switch (p->actionCounters[0]) {
        case 0:
            if (!(p->actionCounters[1] & 15)) {
                Vanargand_Difference(&delta, Player_GetEffectivePosition(0), p->origin);
                if (p->mode == 1) {
                    delta.x += 512;
                    delta.z -= 512;
                } else {
                    delta.x -= 512;
                    delta.z += 512;
                }
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 4);
            }
            MotionTrack_Update(&p->motion);
            {
                u32 v = ++p->actionCounters[1];
                if (v & 64) {
                    Vanargand_SetState(p, 1);
                    p->actionEntered = 1;
                }
            }
            break;
        case 1:
            p->finished = 1;
            break;
    }
}

// confidence=low: coordinates a part strike with its opposite part.

void Vanargand_UpdatePartPairedStrike(struct VanargandAttackPair *boss, struct PairedSceneHalf *p,
                                      u32 tick)
{
    u8 *enter;
    u32 mask = 0x20;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        Vanargand_Difference(&p->targetPosition, Player_GetEffectivePosition(0), p->origin);
        if (ScenePair_CountPositiveCounters(boss) == 2) {
            if (p->mode == 1) {
                p->targetPosition.x += 112;
                p->targetPosition.z -= 112;
            } else {
                p->targetPosition.x -= 112;
                p->targetPosition.z += 112;
            }
        } else if (p->mode == 1) {
            p->targetPosition.x -= 256;
            p->targetPosition.z += 256;
        } else {
            p->targetPosition.x += 256;
            p->targetPosition.z -= 256;
        }
        p->targetPosition.y = p->offset.y;
        p->actionCounters[0] = 0;
        p->actionCounters[1] = 0;
        p->actionEntered = 1;
    }
    switch (p->actionCounters[0]) {
        case 0: {
            struct ContactPosition delta;
            if (Vanargand_ReadEntryFlag(&p->actionEntered, &enter)) {
                *enter = 0;
                if (p->mode == 1)
                    Vanargand_MakeVector(&delta, 224, 256);
                else
                    Vanargand_MakeVector(&delta, 96, 256);
                Vanargand_AddVector(&delta, &delta, &p->offset);
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 2);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 1);
                *enter = 1;
            }
            break;
        }

        case 1:
            if (Vanargand_ReadEntryFlag(&p->actionEntered, &enter)) {
                u32 zero = 0;
                *enter = zero;
                MotionTrack_Initialize(&p->motion, &p->offset, &p->targetPosition, 0, 1);
                p->shake = zero;
            }
            if ((++p->actionCounters[1]) > 0) {
                Sound_PlaySong(0x588);
                Vanargand_SetState(p, 2);
                *enter = 1;
            }
            break;

        case 2:
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                if ((ScenePair_CountPositiveCounters(boss) == 2) &&
                    (*((p->mode == 1) ? &boss->halves[0].requestedState
                                      : &boss->halves[1].requestedState) == 5)) {
                    struct ContactPosition delta;
                    struct ContactPosition position;
                    if (p->mode == 1)
                        Vanargand_MakeVector(&delta, 224, 256);
                    else
                        Vanargand_MakeVector(&delta, 96, 256);
                    Vanargand_AddVector(&delta, &delta, &p->offset);
                    MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 2);
                    Sound_PlaySong(0x1ba);
                    Vanargand_AddVector(&position, &p->targetPosition, p->origin);
                    AnimatedParticles_EmitBurst(&position, (s16)p->fieldB0, 0, 64, 4, 4, 4, 4);
                    Vanargand_SetState(p, 6);
                    p->actionEntered = 1;
                    p->finished = 1;
                } else {
                    Sound_PlaySong(0x3d9);
                    {
                        struct ContactPosition *position = &p->sprite.position;
                        s16 heading = (s16)p->fieldB0;
                        AnimatedParticles_EmitBurst(position, heading, 0, 64, 4, 4, 4, 4);
                    }
                    Vanargand_SetState(p, 6);
                    p->actionEntered = 1;
                    p->finished = 1;
                }

            } else if (!(tick & 1))
                Vanargand_EmitPartTrail(p);
            break;

        case 3:
            if (Vanargand_ReadEntryFlag(&p->actionEntered, &enter)) {
                *enter = 0;
                Vanargand_SetFlags(p, 8);
                {
                    u16 saved = p->active;
                    p->bodyCollider.unknown3C = saved;
                }
                p->shake = 16;
            }
            if ((p->actionCounters[1] <= 44) && (!(p->actionCounters[1] & 15))) {
                struct ContactPosition position;
                Vanargand_AddVector(&position, &p->targetPosition, p->origin);
                AnimatedParticles_EmitBurst(&position, (s16)p->fieldB0, 0, 64, 4, 4, 4, 4);
            }
            if ((++p->actionCounters[1]) > 90) {
                Vanargand_SetState(p, 4);
                *enter = 1;
            }
            break;

        case 4:
            Vanargand_SetFlags(p, 8);
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 5);
                p->actionEntered = 1;
            }
            break;

        case 5:
            Vanargand_SetFlags(p, 8);
            if ((++p->actionCounters[1]) > 30) {
                Vanargand_SetState(p, 6);
                p->actionEntered = 1;
            }
            break;

        case 6:
            Vanargand_SetFlags(p, 8);
            p->finished = 1;
            break;
    }
}

void SceneBurstActor_UpdateTimedBurst(void *unused, struct PairedSceneHalf *actor)
{
    struct ContactPosition point;
    struct ContactPosition *out;
    struct ContactPosition *base;
    struct ContactPosition *offset;
    u32 mask = 0x20;
    actor->actionFlags |= mask;
    actor->actionFlags |= 8;
    if (actor->entered) {
        actor->entered = 0;
        actor->bodyCollider.unknown3C = actor->active;
        actor->shake = 0x10;
    }
    if (actor->actionCounters[1] <= 0x2C && !(actor->actionCounters[1] & 15)) {
        out = &point;
        offset = &actor->targetPosition;
        base = actor->origin;
        out->x = offset->x + base->x;
        out->y = offset->y + base->y;
        out->z = offset->z + base->z;
        AnimatedParticles_EmitBurst(out, (s16)actor->fieldB0, 0, 0x40, 4, 4, 4, 4);
    }
    if (++actor->actionCounters[1] > 0x5A)
        actor->finished = 1;
}

// confidence=low: moves a Vanargand part along its heading, then plays sequence 7.

void Vanargand_UpdatePartRetreat(struct VanargandAttackPair *boss, struct PairedSceneHalf *p)
{
    u8 *enter;
    u32 state;
    struct ContactPosition delta;
    u32 mask = 0x20;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        if (p->mode == 0)
            Vanargand_RetreatVector(&delta, (s16)p->heading);
        else
            Vanargand_RetreatVector(&delta, (s16)p->heading);
        MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 4);
        {
            u32 zero = 0;
            p->actionCounters[0] = zero;
            p->actionCounters[1] = zero;
            p->actionEntered = 1;
        }
    }
    state = p->actionCounters[0];
    switch (state) {
        case 0:
            Vanargand_SetFlags(p, 8);
            MotionTrack_Update(&p->motion);
            {
                u32 finished = p->motion.active == 2;
                if (finished) {
                    Vanargand_SetState(p, 1);
                    p->actionEntered = 1;
                }
            }
            break;
        case 1: {
            u8 *localEnter = &p->actionEntered;
            u8 value = *localEnter;
            enter = localEnter;
            if (value) {
                *localEnter = value = 0;
                Anim_SetSequence(&p->graphics.sequence, boss->effectResource, 7, (u8)p->mode, 0);
            }
        }
            if (Vanargand_StepSequenceEnded(p)) {
                Vanargand_SetState(p, 2);
                *enter = 1;
            }
            break;
        case 2:
            p->finished = 1;
            break;
    }
}

// confidence=low: aims at the player, winds back, strikes, and returns to rest.

void Vanargand_UpdatePartGroundStrike(struct VanargandAttackPair *boss, struct PairedSceneHalf *p,
                                      u32 tick)
{
    u8 *enter;
    register u32 entering;
    u32 mask = 0x20;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        p->actionCounters[0] = 0;
        p->actionCounters[1] = 0;
        p->actionEntered = 1;
    }
    switch (p->actionCounters[0]) {
        case 0: {
            struct ContactPosition delta;
            entering = p->actionEntered;
            enter = &p->actionEntered;

            if (entering) {
                *enter = 0;
                Vanargand_MakeVector(&delta, Vanargand_AimAngle(p), 256);
                Vanargand_AddVectorReversedValue(&delta, &p->offset, &delta);
                {
                    void *data = boss->effectResource;
                    register u32 flip = 4;
                    struct DisplaySequenceState *seq;
                    seq = &p->graphics.sequence;
                    Anim_SetSequence(seq, data, 7, (u8)p->mode, flip);
                    Vanargand_StepSequenceEnded(p);
                }
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 2);
                Sound_PlaySong(0x588);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 1);
                *enter = 1;
            }
            break;
        }
        case 1: {
            struct ContactPosition delta;
            entering = p->actionEntered;
            enter = &p->actionEntered;

            if (entering) {
                *enter = 0;
                Vanargand_MakeVector(&delta, (Vanargand_AimAngle(p) + 128) & 255, 512);
                Vanargand_AddVectorReversedValue(&delta, &p->offset, &delta);
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 4);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 2);
                *enter = 1;
            }
            break;
        }
        case 2:
            entering = p->actionEntered;
            enter = &p->actionEntered;

            if (entering) {
                p->actionEntered = 0;
                Vanargand_Difference(&p->targetPosition, Player_GetEffectivePosition(0), p->origin);
                {
                    struct ContactPosition *player = Player_GetEffectivePosition(0);
                    u32 baseY = (u16)p->origin->y;
                    u32 playerY = (u16)player->y;
                    p->targetPosition.y = playerY - baseY;
                }
                MotionTrack_Initialize(&p->motion, &p->offset, &p->targetPosition, 0, 1);
                p->shake = 0;
            }
            if (++p->actionCounters[1] > 19) {
                Vanargand_SetState(p, 3);
                *enter = 1;
                Sound_PlaySong(0x589);
            }
            break;
        case 3:
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Sound_PlaySong(0x1ba);
                AnimatedParticles_EmitBurst(&p->sprite.position, (s16)p->fieldB0, 0, 64, 4, 4, 4,
                                            4);
                Vanargand_SetState(p, 4);
                p->actionEntered = 1;
            } else if (!(tick & 1))
                Vanargand_EmitPartTrail(p);
            break;
        case 4: {
            u32 entering;
            u32 *flags;
            /* Keep the flags address for the update after the entry effect. */
            {
                u32 mask = 8;
                u32 *initialFlags = &p->actionFlags;
                *initialFlags |= mask;
            }
            entering = p->actionEntered;
            flags = &p->actionFlags;
            enter = &p->actionEntered;

            if (entering) {
                *enter = 0;
                {
                    u16 saved = p->active;
                    p->bodyCollider.unknown3C = saved;
                }
                p->shake = 8;
            }
            if (!(p->actionCounters[1] & 15) && p->actionCounters[1] <= 44)
                AnimatedParticles_EmitBurst(&p->sprite.position, (s16)p->fieldB0, 0, 64, 4, 4, 4,
                                            4);
            {
                u32 mask = 128;
                *flags |= mask;
            }
            if (++p->actionCounters[1] > 89) {
                Vanargand_SetState(p, 5);
                *enter = 1;
            }
            break;
        }
        case 5: {
            struct ContactPosition delta;
            Vanargand_SetFlags(p, 8);
            entering = p->actionEntered;
            enter = &p->actionEntered;

            if (entering) {
                *enter = 0;
                Vanargand_MakeVector(&delta, (s16)p->heading, (s16)p->scale);
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 4);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 6);
                *enter = 1;
            }
            break;
        }
        case 6:
            Vanargand_SetFlags(p, 8);
            p->finished = 1;
            break;
    }
}

// confidence=low: attacks a position offset from the player before returning.

void Vanargand_UpdatePartOffsetStrike(struct VanargandAttackPair *boss, struct PairedSceneHalf *p,
                                      u32 tick)
{
    u8 *enter;
    u32 entering;
    u32 mask = 0x20;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        p->actionCounters[0] = 0;
        p->actionCounters[1] = 0;
        p->actionEntered = 1;
    }
    switch (p->actionCounters[0]) {
        case 0: {
            struct ContactPosition delta;
            u32 one;
            void *motion;
            Vanargand_SetFlags(p, 8);
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                Vanargand_MakeVector(&delta, (Vanargand_AimAngle(p) + 128) & 255, 128);
                Vanargand_AddVectorReversedValue(&delta, &p->offset, &delta);
                motion = &p->motion;
                one = 1;
                MotionTrack_Initialize(motion, &p->offset, &delta, 0, one);
                {
                    void *data = boss->effectResource;
                    struct DisplaySequenceState *seq = &p->graphics.sequence;
                    Anim_SetSequence(seq, data, 7, (u8)p->mode, 4);
                    Vanargand_StepSequence(p);
                }
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 1);
                *enter = 1;
            }
            break;
        }

        case 1: {
            struct ContactPosition delta;
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                u32 zero = 0;
                *enter = zero;
                delta = *Player_GetEffectivePosition(zero);
                Vanargand_OffsetTarget(&delta, p->mode);
                Vanargand_Difference(&delta, &delta, p->origin);
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 2);
            }
            if ((++p->actionCounters[1]) > 20) {
                Vanargand_SetState(p, 2);
                *enter = 1;
            }
            break;
        }

        case 2:
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                Sound_PlaySong(0x589);
            }
            MotionTrack_Update(&p->motion);
            if (!(tick & 1))
                Vanargand_EmitPartTrail(p);
            if (Vanargand_MotionEnded(p)) {
                Sound_PlaySong(0x3d9);
                AnimatedParticles_EmitBurst(&p->sprite.position, (s16)p->fieldB0, 0, 64, 4, 4, 4,
                                            4);
                Vanargand_SetState(p, 3);
                *enter = 1;
            }
            break;

        case 3: {
            struct ContactPosition delta;
            Vanargand_SetFlags(p, 8);
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                Vanargand_MakeVector(&delta, (s16)p->heading, (s16)p->scale);
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 1);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 4);
                *enter = 1;
            }
            break;
        }

        case 4:
            Vanargand_SetFlags(p, 8);
            p->finished = 1;
            break;
    }
}

// confidence=low: player-directed sweeping strike followed by a timed retreat.

// confidence=low: aims at the player, winds back, strikes, and returns to rest.

void Vanargand_UpdatePartSweepingStrike(struct VanargandAttackPair *boss, struct PairedSceneHalf *p,
                                        u32 tick)
{
    u8 *enter;
    u32 entering;
    u32 mask = 0x20;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        p->actionCounters[0] = 0;
        p->actionCounters[1] = 0;
        p->actionEntered = 1;
    }
    switch (p->actionCounters[0]) {
        case 0: {
            struct ContactPosition delta;
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                Vanargand_MakeVector(&delta, Vanargand_AimAngle(p), 256);
                Vanargand_AddVectorReversedValue(&delta, &p->offset, &delta);
                {
                    void *motion = &p->motion;
                    u32 angle = (Vanargand_GetModeShifted(7, p) + 96) & 255;
                    OrbitMotion_BeginDistanceWave(motion, &p->offset, &delta, angle, 16);
                }
                {
                    void *data = boss->effectResource;
                    u32 flip = 4;
                    struct DisplaySequenceState *seq;
                    seq = &p->graphics.sequence;
                    Anim_SetSequence(seq, data, 8, (u8)p->mode, flip);
                    Vanargand_StepSequenceEnded(p);
                }
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 1);
                *enter = 1;
            }
            break;
        }

        case 1: {
            struct ContactPosition delta;
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                Vanargand_MakeVector(&delta, (Vanargand_AimAngle(p) + 128) & 255, 512);
                Vanargand_AddVectorReversedValue(&delta, &p->offset, &delta);
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 3);
                Sound_PlaySong(0x588);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 2);
                *enter = 1;
            }
            break;
        }

        case 2:
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                p->actionEntered = 0;
                Vanargand_Difference(&p->targetPosition, Player_GetEffectivePosition(0), p->origin);
                {
                    struct ContactPosition *player = Player_GetEffectivePosition(0);
                    u32 baseY = (u16)p->origin->y;
                    u32 playerY = (u16)player->y;
                    p->targetPosition.y = playerY - baseY;
                }
                OrbitMotion_BeginDistanceWave(&p->motion, &p->offset, &p->targetPosition,
                                              ((p->mode << 7) + 96) & 255, 16);
                p->shake = 0;
            }
            if ((++p->actionCounters[1]) > 23) {
                Sound_PlaySong(0x589);
                Vanargand_SetState(p, 3);
                *enter = 1;
            }
            break;

        case 3:
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Sound_PlaySong(0x3d9);
                AnimatedParticles_EmitBurst(&p->sprite.position, (s16)p->fieldB0, 0, 64, 4, 4, 4,
                                            4);
                Vanargand_SetState(p, 4);
                p->actionEntered = 1;
            } else if (!(tick & 1))
                Vanargand_EmitPartTrail(p);
            break;

        case 4: {
            struct ContactPosition *v;
            Vanargand_SetFlags(p, 8);
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                {
                    u16 saved = p->active;
                    p->bodyCollider.unknown3C = saved;
                }
            }
            v = (struct ContactPosition *)&p->motion.target;
            v->x = Vanargand_TruncateShift(v->x, 1);
            v->y = Vanargand_TruncateShift(v->y, 1);
            v->z = Vanargand_TruncateShift(v->z, 1);
            Vanargand_AddVectorReversedValue(&p->offset, v, &p->offset);
            if ((++p->actionCounters[1]) > 59) {
                Vanargand_SetState(p, 5);
                *enter = 1;
            }
            break;
        }

        case 5: {
            struct ContactPosition delta;
            Vanargand_SetFlags(p, 8);
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                Vanargand_MakeVector(&delta, (s16)p->heading, (s16)p->scale);
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 4);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 6);
                *enter = 1;
            }
            break;
        }

        case 6:
            Vanargand_SetFlags(p, 8);
            p->finished = 1;
            break;
    }
}

// confidence=low: three motion legs include an approach toward the player.

void Vanargand_UpdatePartLunge(void *unused, struct PairedSceneHalf *p)
{
    u8 *enter;
    u32 entering;
    u32 mask = 0x20;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        p->actionCounters[0] = 0;
        p->actionCounters[1] = 0;
        p->actionEntered = 1;
    }
    switch (p->actionCounters[0]) {
        case 0: {
            struct ContactPosition delta;
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                Vanargand_MakeVector(&delta, 160, 256);
                Vanargand_AddVectorReversedValue(&delta, &p->offset, &delta);
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 4);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 1);
                *enter = 1;
            }
            break;
        }
        case 1: {
            struct ContactPosition delta;
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                struct ContactPosition *player;
                p->actionEntered = 0;
                player = Player_GetEffectivePosition(0);
                Vanargand_DifferenceFromPartBase(&delta, player, p);
                delta.x = Vanargand_TruncateShift(delta.x, 1);
                delta.y = p->offset.y;
                delta.z = Vanargand_TruncateShift(delta.z, 1);
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 3);
            }
            if (++p->actionCounters[1] > 16) {
                Vanargand_SetState(p, 2);
                *enter = 1;
            }
            break;
        }
        case 2:
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 3);
                p->actionEntered = 1;
            }
            break;
        case 3:
            if (++p->actionCounters[1] > 30) {
                Vanargand_SetState(p, 4);
                p->actionEntered = 1;
            }
            break;
        case 4: {
            struct ContactPosition delta;
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                Vanargand_MakeVector(&delta, (s16)p->heading, (s16)p->scale);
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 4);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 5);
                *enter = 1;
            }
            break;
        }
        case 5:
            p->finished = 1;
            break;
    }
}

// confidence=low: emits six radial effects, waits for animation 10, then deactivates the part.

void CpuSet(const void *, void *, u32);

void ActorStatus_Deactivate(void *, void *);

void Vanargand_ReleaseRadialEffects(struct VanargandAttackPair *boss, struct PairedSceneHalf *p)
{
    u32 mask = 12;
    p->actionFlags |= mask;
    if (p->entered) {
        struct ContactPosition velocity;
        struct ContactPosition position;
        s32 i;
        u32 *state;
        u32 *timer;
        u8 *entered;
        p->entered = 0;
        Anim_SetSequence(&p->graphics.sequence, boss->effectResource, 10, (u8)p->mode, 0);
        ZeroFill8(&velocity);
        i = 0;
        state = p->actionCounters;
        timer = &state[1];
        entered = &p->actionEntered;
        do {
            Vanargand_MakeRadialVector(&position, (i * 255) / 6, 128);
            Vanargand_AddVectorReversedValue(&position, &p->sprite.position, &position);
            VanargandEffect_Create(1, i << 1, &position, &velocity, p->mode);
            i++;
        } while (i <= 5);
        Sound_PlaySong(0x58c);
        state[0] = 0;
        *timer = 0;
        *entered = 1;
    }
    switch (p->actionCounters[0]) {
        case 0:
            if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)p,
                                            &p->graphics.sequence)) {
                Vanargand_SetState(p, 1);
                p->actionEntered = 1;
            }
            break;

        case 1:
            p->finished = 1;
            ActorStatus_Deactivate(boss, p);
            break;
    }
}

// confidence=low: emits six surrounding effects before retiring the part.

void Vanargand_EmitRadialPartEffects(struct VanargandAttackPair *boss, struct PairedSceneHalf *p)
{
    u8 *enter;
    u32 mask = 12;
    p->actionFlags |= mask;
    p->actionFlags |= 128;
    if (p->entered) {
        struct ContactPosition zero;
        struct ContactPosition point;
        u32 *state;
        u32 *timer;
        s32 i;
        p->entered = 0;
        Anim_SetSequence(&p->graphics.sequence, boss->effectResource, 10, (u8)p->mode, 0);
        ZeroFill8(&zero);
        i = 0;
        state = &p->actionCounters[0];
        timer = &p->actionCounters[1];
        enter = &p->actionEntered;
        for (; i < 6; i++) {
            Vanargand_MakeRadialVector(&point, (i * 255) / 6, 128);
            Vanargand_AddVectorReversedValue(&point, &p->sprite.position, &point);
            VanargandEffect_Create(3, i * 2, &point, &zero, p->mode);
        }

        *state = 0;
        *timer = 0;
        *enter = 1;
    }
    switch (p->actionCounters[0]) {
        case 0: {
            u32 entered = p->actionEntered;
            enter = &p->actionEntered;
            if (entered) {
                *enter = 0;
                p->shake = 32;
            }
            if (p->shake == 0) {
                Vanargand_SetState(p, 1);
                *enter = 1;
            }
            break;
        }

        case 1:
            if (Vanargand_StepSequenceEnded(p)) {
                Sound_PlaySong(0x58c);
                Vanargand_SetState(p, 2);
                p->actionEntered = 1;
            }
            break;

        case 2:
            p->finished = 1;
            ActorStatus_Deactivate(boss, p);
            break;
    }
}

// confidence=low: paired part travels to an impact point and emits a burst.

void Vanargand_UpdatePartImpact(struct VanargandAttackPair *boss, struct PairedSceneHalf *p,
                                u32 frame)
{
    u8 *enter;
    u32 entering;
    u32 savedFrame = frame;
    u32 mask = 0x20;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        p->actionCounters[0] = 0;
        p->actionCounters[1] = 0;
        p->actionEntered = 1;
    }
    switch (p->actionCounters[0]) {
        case 0:
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                {
                    u32 zero = 0;
                    *enter = zero;
                    Anim_SetSequence(&p->graphics.sequence, boss->effectResource, 8, (u8)p->mode,
                                     zero);
                }
            }
            if (Vanargand_StepSequenceEnded(p)) {
                Vanargand_SetState(p, 1);
                *enter = 1;
            }
            break;

        case 1: {
            struct ContactPosition delta;
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                if (p->mode == 1)
                    Vanargand_MakeRadialVector(&delta, 224, 64);
                else
                    Vanargand_MakeRadialVector(&delta, 96, 64);
                Vanargand_AddPosition(&delta, &delta, &p->offset);
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 4);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 2);
                *enter = 1;
            }
            break;
        }

        case 2:
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                if (p->mode == 1) {
                    Vanargand_SetVector(&p->targetPosition, 0x3ab, 0x100, 0x2cb);
                } else {
                    Vanargand_SetVector(&p->targetPosition, 0x2cb, 0x100, 0x3ab);
                }
                Vanargand_Difference(&p->targetPosition, &p->targetPosition, p->origin);
                MotionTrack_Initialize(&p->motion, &p->offset, &p->targetPosition, 0, 2);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                if ((ScenePair_CountPositiveCounters(boss) == 2) &&
                    (*((p->mode == 1) ? &boss->halves[0].requestedState
                                      : &boss->halves[1].requestedState) == 12)) {
                    struct ContactPosition point;
                    Sound_PlaySong(0x1ba);
                    Vanargand_AddPosition(&point, &p->targetPosition, p->origin);
                    AnimatedParticles_EmitBurst(&point, (s16)p->fieldB0, 0, 64, 4, 4, 4, 4);
                    p->shake = 15;
                    Vanargand_FinishImpact(p, enter);
                } else {
                    Sound_PlaySong(0x3d9);
                    AnimatedParticles_EmitBurst(&p->sprite.position, (s16)p->fieldB0, 0, 64, 4, 4,
                                                4, 4);
                    Vanargand_FinishImpact(p, enter);
                }
            } else if (!(savedFrame & 1))
                Vanargand_EmitPartTrail(p);
            break;

        case 3:
            p->finished = 1;
            break;
    }
}

void OrbitingParticles_Create(void *, int, int, int, int);

void SceneParticleEmitter_EmitPeriodically(void *unused, struct PairedSceneHalf *p, u32 frame)
{
    u32 mask = 12;
    p->actionFlags |= mask;
    if ((frame & 31) == 0) {
        if (p->mode == 0)
            OrbitingParticles_Create(&p->sprite.position, 256, 32, 16, 2);
        else
            OrbitingParticles_Create(&p->sprite.position, 256, 32, 16, 3);
    }
    p->finished = 1;
}

void Palette_InterpolateBanks(void *, const void *, const void *, s32, s32);

void Vanargand_UpdatePartFlashFade(struct VanargandAttackPair *boss, struct PairedSceneHalf *p,
                                   s32 frame)
{

    u32 mask;
    u32 twelve = 12;
    p->actionFlags |= twelve;
    mask = 0x80;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        CpuSet(p->palettes[0], p->graphics.palette, 0x04000008);
        Vanargand_SetState(p, 0);
        p->actionEntered = 1;
    }
    {
        u32 *statePtr;
        u32 value;
        statePtr = &p->actionCounters[0];
        value = *statePtr;
        switch (value) {
            case 0:
                Palette_InterpolateBanks(p->graphics.palette, p->palettes[0], p->palettes[2], frame,
                                         7);
                if (frame & mask) {
                    Vanargand_SetState(p, 1);
                    p->actionEntered = 1;
                }
                break;
            case 1: {
                {
                    u8 *ptr = &p->finished;
                    *ptr = 1;
                }
            } break;
        }
    }
}

// Matched with tools/scope by gpt-6.1-sol workers; the pins and empty asm barriers are

// temporary concessions.

// confidence=low: moves the part along its stored retreat angle, then returns to idle.

void Vanargand_UpdatePartSidestep(struct VanargandAttackPair *bossArg, struct PairedSceneHalf *p)
{
    u32 mask = 12;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        {
            void *resource = bossArg->effectResource;
            Anim_SetSequence(&p->graphics.sequence, resource, 11, (u8)p->mode, 0);
        }
        PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->graphics.sequence);
        {
            u32 zero = 0;
            p->actionCounters[0] = zero;
            p->actionCounters[1] = zero;
        }
        p->actionEntered = 1;
    }
    {
        u32 state = p->actionCounters[0];
        switch (state) {
            case 0: {
                struct ContactPosition target;
                u32 entering = p->actionEntered;
                u8 *enter;
                enter = &p->actionEntered;
                if (entering) {
                    *enter = 0;
                    Vanargand_PolarVector(&target, p->field187, 128);
                    Vanargand_AddPlanar(&target, &p->offset);
                    target.y = p->offset.y;
                    MotionTrack_Initialize(&p->motion, &p->offset, &target, 0, 4);
                }
                MotionTrack_Update(&p->motion);
                if (Vanargand_MotionEnded(p)) {
                    u32 next = 1;
                    p->actionCounters[0] = next;
                    p->actionCounters[1] = 0;
                    *enter = next;
                }
                break;
            }

            case 1:
                if (p->actionEntered) {
                    p->actionEntered = 0;
                    {
                        void *resource = bossArg->effectResource;
                        Anim_SetSequence(&p->graphics.sequence, resource, 9, (u8)p->mode, 0);
                    }
                    PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->graphics.sequence);
                }
                p->finished = 1;
                break;
        }
    }
}

// confidence=low: holds sequence 11 for 180 frames before restoring action 22.

void Vanargand_UpdatePartRecovery(struct VanargandAttackPair *boss, struct PairedSceneHalf *p,
                                  s32 frame)
{
    u32 mask = 12;
    p->actionFlags |= mask;
    p->actionFlags |= 0x120;
    if (p->entered) {
        p->entered = 0;
        {
            void *resource = boss->effectResource;
            Anim_SetSequence(&p->graphics.sequence, resource, 11, (u8)p->mode, 0);
        }
        Vanargand_StepSequence(p);
    }
    if (frame > 180) {
        {
            u32 *flags;
            u32 mask;
            flags = &p->actionFlags;
            mask = ~0x100;
            *flags &= mask;
        }
        Vanargand_ChangeAction(p, 22);
    }
}

// Tentative action: raise the part along a variant-dependent heading, then land.

void Vanargand_UpdatePartLeap(struct VanargandAttackPair *boss, struct PairedSceneHalf *p)
{
    u8 *enter;
    u32 mask = 12;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        {
            void *resource = boss->effectResource;
            Anim_SetSequence(&p->graphics.sequence, resource, 9, (u8)p->mode, 0);
        }
        PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->graphics.sequence);
        Vanargand_SetState(p, 0);
        p->actionEntered = 1;
    }
    switch (p->actionCounters[0]) {
        case 0: {
            struct ContactPosition target;
            u32 entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                if (p->mode == 0)
                    Vanargand_PolarVector(&target, 48, 1024);
                else
                    Vanargand_PolarVector(&target, 16, 1024);
                target.y = 768;
                MotionTrack_Initialize(&p->motion, &p->offset, &target, 0, 3);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                u32 next = 1;
                Vanargand_SetState(p, next);
                *enter = next;
            }
            break;
        }
        case 1: {
            struct ContactPosition target;
            struct ContactPosition speed;
            u32 entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                *enter = 0;
                target = p->offset;
                Vanargand_SetY(&target, Vanargand_TerrainHeight(&p->offset));
                Vanargand_SetVector(&speed, 8, 32, 8);
                MotionTrack_Initialize(&p->motion, &p->offset, &target, 0, 1);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                p->finished = 1;
                Vanargand_SetState(p, 2);
                *enter = 1;
            }
        }
        case 2:
            Vanargand_SetFlags(p, 128);
            p->finished = 1;
            break;
    }
}

// Tentative role: oscillate a Vanargand part vertically with a pause at each end.

void Vanargand_UpdatePartVerticalOscillation(u8 *unused, struct PairedSceneHalf *p)
{
    struct ContactPosition lowerTarget, upperTarget;
    u8 *enter;
    u32 mask = 0x20;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        p->actionCounters[0] = 0;
        p->actionCounters[1] = 0;
        p->actionEntered = 1;
    }
    switch (p->actionCounters[0]) {
        case 0: {
            u32 entered;
            void *motion;
            {
                u8 *address = &p->actionEntered;
                entered = *address;
                enter = address;
            }
            motion = &p->motion;
            if (entered) {
                u32 zero = 0;
                *enter = zero;
                Vanargand_SetVector(&lowerTarget, zero, -256, zero);
                Vanargand_AddOffset(&lowerTarget, &p->offset, &lowerTarget);
                MotionTrack_Initialize(motion, &p->offset, &lowerTarget, 0, 4);
            }
            MotionTrack_Update(motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 1);
                *enter = 1;
            }
            break;
        }
        case 1:
            if (++p->actionCounters[1] > 30) {
                Vanargand_SetState(p, 2);
                p->actionEntered = 1;
            }
            break;
        case 2: {
            u32 entered;
            void *motion;
            {
                u8 *address = &p->actionEntered;
                entered = *address;
                enter = address;
            }
            motion = &p->motion;
            if (entered) {
                *enter = 0;
                Vanargand_CopyRaised(&upperTarget, &p->offset);
                MotionTrack_Initialize(motion, &p->offset, &upperTarget, 0, 4);
            }
            MotionTrack_Update(motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 3);
                *enter = 1;
            }
            break;
        }
        case 3:
            if (++p->actionCounters[1] > 30) {
                Vanargand_SetState(p, 0);
                p->actionEntered = 1;
            }
            break;
    }
}

void Vanargand_UpdatePartBobbing(struct VanargandAttackPair *boss, struct PairedSceneHalf *p,
                                 s32 frame)
{
    u32 mask = 0x20;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        {
            void *resource = boss->effectResource;
            Anim_SetSequence(&p->graphics.sequence, resource, 9, (u8)p->mode, 0);
        }
        Vanargand_StepSequence(p);
    }
    p->offset.y = Vanargand_TruncateScale(gUnk_086149C4[frame & 255] * 128);
    p->finished = 1;
}

// confidence=low: moves the part to its heading/distance target before releasing render flags.

void Vanargand_UpdatePartRadialTravel(struct VanargandAttackPair *boss, struct PairedSceneHalf *p)
{
    if (p->entered) {
        struct ContactPosition target;
        p->entered = 0;
        Vanargand_PolarVector(&target, (s16)p->heading, (s16)p->scale);
        MotionTrack_Initialize(&p->motion, &p->offset, &target, 0, 4);
        {
            void *resource = boss->effectResource;
            Anim_SetSequence(&p->graphics.sequence, resource, 9, (u8)p->mode, 0);
        }
        Vanargand_StepSequence(p);
    }
    MotionTrack_Update(&p->motion);
    if (Vanargand_MotionEnded(p)) {
        u8 *finished;
        finished = &p->finished;
        *finished = 1;
    } else {
        u32 mask = 12;
        p->actionFlags |= mask;
    }
}

void Vanargand_SpawnPartEmergenceEffects(void *);

// confidence=low: reveals the controller and moves its part outward after two keyframe tracks.

void Vanargand_UpdatePartEmergence(struct VanargandAttackPair *boss, struct PairedSceneHalf *p)
{
    struct ContactPosition projected;
    u8 *enter;
    struct SceneInputResource *keyframes;
    struct SceneInputWidget *animation;
    u32 entering;
    u32 state;
    u32 mask = 12;
    void *paletteSource;
    void *paletteDestination;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        if (!(p->sprite.flags & 1)) {
            p->finished = 1;
            Vanargand_SetState(p, 3);
            p->actionEntered = 1;
            return;
        }
        boss->widget.flags &= ~1;
        if (p->mode == 0)
            boss->widget.flags |= 4;
        else
            boss->widget.flags &= ~4;
        {
            void *data = boss->effectResource;
            u32 zero = 0;
            Anim_SetSequence(&p->graphics.sequence, data, 9, (u8)p->mode, zero);
        }
        PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->graphics.sequence);
        paletteSource = p->palettes[0];
        paletteDestination = (void *)p->graphics.palette;
        CpuSet(paletteSource, paletteDestination, 0x04000008);
        {
            u32 zero = 0;
            p->actionCounters[0] = zero;
            p->actionCounters[1] = zero;
        }
        p->actionEntered = 1;
    }
    state = p->actionCounters[0];
    switch (state) {
        case 0:
            entering = p->actionEntered;
            enter = &p->actionEntered;
            animation = &boss->widget;
            keyframes = &boss->resource;
            if (entering) {
                p->actionEntered = 0;
                Anim_SetKeyframes(animation, keyframes, 0, 2, 0);
                Sound_PlaySong(0x58d);
            }
            if (Anim_Step(animation, keyframes)) {
                Vanargand_SetState(p, 1);
                *enter = 1;
            }
            p->sprite.position = *((struct ContactPosition *)p->origin);
            break;

        case 1:
            entering = p->actionEntered;
            enter = &p->actionEntered;
            animation = &boss->widget;
            keyframes = &boss->resource;
            if (entering) {
                p->actionEntered = 0;
                Anim_SetKeyframes(animation, keyframes, 1, 2, 0);
            }
            if (Anim_Step(animation, keyframes)) {
                Vanargand_SetState(p, 2);
                *enter = 1;
                boss->widget.flags |= 1;
            }
            p->sprite.position = *((struct ContactPosition *)p->origin);
            break;

        case 2: {
            struct ContactPosition target;
            entering = p->actionEntered;
            enter = &p->actionEntered;
            if (entering) {
                p->actionEntered = 0;
                p->sprite.flags &= ~1;
                Vanargand_PolarVectorShort(&p->offset, (s16)p->heading, 640);
                Vanargand_PolarVectorShort(&target, (s16)p->heading, (s16)p->scale);
                MotionTrack_Initialize(&p->motion, &p->offset, &target, 0, 4);
                Vanargand_SpawnPartEmergenceEffects(p);
                Sound_PlaySong(0x58e);
            }
            MotionTrack_Update(&p->motion);
            if (Vanargand_MotionEnded(p)) {
                Vanargand_SetState(p, 3);
                *enter = 1;
            }
            break;
        }

        case 3:
            p->finished = 1;
            break;
    }

    Vanargand_Project(&projected, p->origin);
    projected.x = (projected.x - gCameraOrigin[0]) + 120;
    projected.y = (projected.y - gCameraOrigin[1]) + 92;
    boss->widget.position = projected;
}

// confidence=low: moves to the radial target and fades from the alternate palette.

void Vanargand_UpdatePartRevealFade(struct VanargandAttackPair *boss, struct PairedSceneHalf *p,
                                    s32 frame)
{
    u32 mask = 12;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        Vanargand_PolarVector(&p->offset, (s16)p->heading, (s16)p->scale);
        p->sprite.flags &= ~1;
        CpuSet(p->palettes[1], p->graphics.palette, 0x04000008);
        {
            void *resource = boss->effectResource;
            Anim_SetSequence(&p->graphics.sequence, resource, 9, (u8)p->mode, 0);
        }
        PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->graphics.sequence);
        {
            u32 zero = 0;
            p->actionCounters[0] = zero;
            p->actionCounters[1] = zero;
        }
        p->actionEntered = 1;
    }
    switch (p->actionCounters[0]) {
        case 0:
            Palette_InterpolateBanks(p->graphics.palette, p->palettes[1], p->palettes[0], frame, 6);
            if (frame > 63) {
                u32 next = 1;
                Vanargand_SetState(p, next);
                p->actionEntered = next;
            }
            break;
        case 1:
            p->finished = 1;
            break;
    }
}

void Vanargand_UpdatePartFadeOut(struct VanargandAttackPair *boss, struct PairedSceneHalf *p,
                                 s32 frame)
{

    u32 twelve = 12;
    p->actionFlags |= twelve;
    if (p->entered) {
        p->entered = 0;
        CpuSet(p->palettes[0], p->graphics.palette, 0x04000008);
        Vanargand_SetState(p, 0);
        p->actionEntered = 1;
    }
    {
        u32 *statePtr;
        u32 value;
        statePtr = &p->actionCounters[0];
        value = *statePtr;
        switch (value) {
            case 0:
                Palette_InterpolateBanks(p->graphics.palette, p->palettes[0], p->palettes[1], frame,
                                         6);
                if (frame > 63) {
                    Vanargand_SetState(p, 1);
                    p->actionEntered = 1;
                }
                break;
            case 1: {
                {
                    u8 *ptr = &p->finished;
                    *ptr = 1;
                }
                ActorStatus_Deactivate(boss, p);
            } break;
        }
    }
}

#define gPairedSceneHalfStateCallbacks gUnk_086120A0

#define gActorUpdatePaused gEnginePauseFlags

extern u32 gUnk_02000560;

extern void *gPairedSceneHalfStateCallbacks[];

void Vanargand_ClearPartContact(void *);

void sub_081AFF98(struct PairedSceneHalf *);

void sub_081AF7F4(struct PairedSceneHalf *);

void sub_08249244(struct VanargandAttackPair *, struct PairedSceneHalf *, s32, void *);

void PairedSceneActor_UpdateHalves(struct VanargandAttackPair *actor)
{
    s32 i;
    struct PairedSceneHalf *half;
    u8 *state;
    u32 *timer;
    u32 mask;
    if (!(gEnginePauseFlags & 7)) {
        mask = 4;
        timer = &actor->halves[0].stateTimer;
        state = &actor->halves[0].requestedState;
        half = &actor->halves[0];
        i = 1;
        do {
            if (*state) {
                u32 paused;
                Vanargand_ClearPartContact(half);
                paused = gUnk_02000560 & mask;
                if (!paused) {
                    sub_081AFF98(half);
                    sub_081AF7F4(half);
                    state[3] = paused;
                    {
                        void **table = gPairedSceneHalfStateCallbacks;
                        void **callback = (void **)((*state << 2) + (u32)table);
                        sub_08249244(actor, half, *timer, *callback);
                    }
                }
                PairedSceneActor_UpdateHalfPresentation(actor, half);
                ++*timer;
            }
            timer = (u32 *)((u8 *)timer + sizeof(*half));
            state += sizeof(*half);
            half++;
        } while (--i >= 0);
    }
}

void Vanargand_DestroyPartController(struct VanargandAttackPair *controller)
{
    struct PairedHalfTarget *node;
    u8 *shadow;
    struct ContactCollider *collider;
    struct ContactSwarmSprite *sprite;
    s32 i;
    Anim_Remove(&controller->widget);
    node = &controller->halves[0].target;
    shadow = controller->halves[0].shadow;
    collider = &controller->halves[0].damageCollider;
    sprite = &controller->halves[0].sprite;
    i = 1;
    do {
        Collider_Remove(collider);
        if (sprite->unknown4[0])
            Sprite_Remove(sprite);
        ScaledShadow_Destroy(shadow);
        SceneLink_RemoveNode(node);
        {
            union VanargandPartStride {
                u32 bytes;
            } stride = {sizeof(struct PairedSceneHalf)};
            node = (struct PairedHalfTarget *)((u8 *)node + stride.bytes);
            shadow += stride.bytes;
            collider = (struct ContactCollider *)((u8 *)collider + stride.bytes);
            sprite = (struct ContactSwarmSprite *)((u8 *)sprite + stride.bytes);
        }
    } while (--i >= 0);
    {
        u32 zero = 0;
        gVanargandPartController = (void *)zero;
    }
}

void *ScriptProc_Start(u16, u16);

s32 Widget_InitLinked(void *, void *, u16, u32, u8, u8, u8, const void *);

// confidence=low: initializes the controller resources and its two embedded halves.

#define Vanargand_InitializePartController sub_081B5EA4

s32 Vanargand_InitializePartController(struct VanargandAttackPair *boss, void *origin,
                                       u16 (*palettes)[16], s32 mode)
{
    struct SceneInputResource *resource;
    struct SceneInputWidget *animation;
    const void *source;
    s32 i;
    void *data = ScriptProc_Start(0xcb05, 0xb3de);
    resource = &boss->resource;
    ResourceHeader_Relocate(resource, data);
    animation = &boss->widget;
    Widget_InitLinked(animation, resource, 0, 16, 3, 1, 60, 0);
    Anim_SetKeyframes(animation, resource, 0, 1, 0);
    if (!palettes)
        source = gPaletteBankData + 0xe00;
    else
        source = palettes[1];
    CpuSet(source, boss->palettes[2], 0x04000010);
    CpuSet(source, boss->palettes[0], 0x04000010);
    source = gPaletteBankData + 0x3240;
    CpuSet(source, boss->palettes[4], 0x04000008);
    source = gPaletteBankData + 0x1060;
    CpuSet(source, boss->palettes[5], 0x04000010);
    boss->widget.flags |= 1;
    boss->effectResource = ScriptProc_Start(0x922e, 0x572b);
    for (i = 0; i < 2; i++) {
        struct PairedSceneHalf *part = &boss->halves[i];

        PairedSceneActor_InitializeHalf(part, origin, i);
        if (i == 1) {
            part->graphics.paletteIndex = 0x70;
            part->palettes[0] = boss->palettes[2];
            part->palettes[2] = boss->palettes[5];
            if (!palettes)
                part->graphics.palette = boss->palettes[1];
            else
                part->graphics.palette = palettes[1];
        } else {
            part->graphics.paletteIndex = 0x71;
            part->palettes[0] = boss->palettes[3];
            part->palettes[2] = boss->palettes[6];
            if (!palettes)
                part->graphics.palette = boss->palettes[0];
            else
                part->graphics.palette = palettes[2];
        }
        part->palettes[1] = boss->palettes[4];
        CpuSet(part->palettes[0], part->graphics.palette, 0x04000008);
    }
    boss->selectedMask = 0;
    switch (mode) {
        case 0:
            boss->halves[0].health = 0;
            boss->halves[1].health = 0;
            break;

        case 1:
            i = 0;
            do {
                struct PairedSceneHalf *part = &boss->halves[i];
                boss->halves[i].health = boss->halves[i].maxHealth;
                part->sprite.flags &= ~1;
                CpuSet(part->palettes[0], part->graphics.palette, 0x04000008);
                Vanargand_MakeRadialVector(&boss->halves[i].offset, (s16)boss->halves[i].heading,
                                           (s16)boss->halves[i].scale);
                Vanargand_RequestAction(&boss->halves[i], 2);
                i++;
            } while (i <= 1);
            break;
    }

    return 0;
}

void *Vanargand_CreatePartController(void *origin, u16 (*palettes)[16], s32 mode)
{
    void **global = &gVanargandPartController;
    struct VanargandAttackPair *p = Task_Create(9, sizeof(*p));
    if (p) {
        Task_SetCallbacks(p, PairedSceneActor_UpdateHalves, Vanargand_DestroyPartController);
        *global = p;
        if (Vanargand_InitializePartController(p, origin, palettes, mode) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
