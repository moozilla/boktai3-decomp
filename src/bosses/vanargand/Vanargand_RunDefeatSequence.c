#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
#include "bosses/vanargand/vanargand_part_layout.h"
/* Vanargand_RunDefeatSequence: waits, rises along a motion track, approaches the part position while the paired halves play cues 0x14/0x18/0x1A, then plays animation 0xF for 256 frames, sets event flags 0x42/0x43/0x52 and runs the second event script (provisional name) */
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
void Anim_SetFrameDurationScale(void *, u32);
static inline u8 complete(struct VanargandPairAttack *p)
{
    u8 result = 0;
    if (p->track.active == 2)
        result = 1;
    return result;
}

void Sound_PlaySong(u32);
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void SceneActors_RunFirstEventScript(void *, struct ContactPosition *);
void SceneActors_RunSecondEventScript(void *, struct ContactPosition *);
void PositionMotion_InitParameterizedApproach(void *, void *, struct ContactPosition *, u32, u32,
                                              u32, u32, u32);
void Bestiary_UnlockEntry(u32);

static inline void setOrigin(struct MotionTrackVector *origin, s16 x, s16 y, s16 z)
{
    s16 *q = &origin->x;
    *q = x;
    origin->y = y;
    origin->z = z;
}

static inline void startPulse(struct VanargandPairAttack *p, u16 a, u16 b)
{
    p->paletteSelector = 0;
    p->paletteMode = a;
    p->paletteShift = b;
    p->paletteFrame = 0;
}

static inline void setShake(struct VanargandPairAttack *p, u16 a, u16 b)
{
    p->attackParameter5B8 = a;
    p->attackParameter5BA = b;
}

static inline void advance(struct VanargandPairAttack *p, u8 phase)
{
    p->phase = phase;
    p->timer = 0;
}

void Vanargand_RunDefeatSequence(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *self = p;
    void *animation = &p->animation;
    struct MotionTrackVector origin;
    struct ContactPosition first, second;
    u32 mask = 0xC;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 3);
    if (p->pending) {
        p->pending = 0;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 4, 0, 4);
        advance(p, 0);
    }
    switch (p->phase) {
        case 0:
            if (++p->timer > 8 && !Player_IsSlotInActionTwelveOrThirtyEight(0)) {
                Sound_PlaySong(0x585);
                p->scriptId = self->part.breaks;
                SceneActors_RunFirstEventScript(p, &self->coordinates.position);
                advance(p, 1);
            }
            break;
        case 1:
            if (p->timer == 0) {
                setOrigin(&origin, 0x140, 0x400, 0x140);
                MotionTrack_Initialize(&self->track, &p->position, &origin, 0, 4);
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 2);
                setShake(self, 8, 3);
            }
            p->timer++;
            MotionTrack_Update(&self->track);
            if (complete(self)) {
                advance(p, 2);
            }
            break;
        case 2:
            if (p->timer == 0) {
                struct ContactPosition *v = &first;
                *v = self->part.sprite.position;
                PositionMotion_InitParameterizedApproach(&self->track, &p->position, &first, 0x20,
                                                         0x20, 0x20, 0, 1);
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 0x14);
            }
            p->timer++;
            MotionTrack_Update(&self->track);
            if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                if (ScenePair_CountPositiveCounters(self->pair) > 0) {
                    Sound_PlaySong(0x1BA);
                    self->flash = 0x3C;
                }
                advance(p, 3);
            }
            break;
        case 3:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 0x18);
                p->timer++;
            }
            if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                advance(p, 4);
            }
            break;
        case 4:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 0x1A);
                p->timer++;
            }
            if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                advance(p, 5);
            }
            break;
        case 5:
            if (p->timer == 0) {
                struct ContactPosition *v = &second;
                *v = self->part.sprite.position;
                PositionMotion_InitParameterizedApproach(&self->track, &p->position, &second, 0x20,
                                                         0x20, 0x20, 0, 1);
            }
            p->timer++;
            MotionTrack_Update(&self->track);
            if (complete(self)) {
                Sound_PlaySong(0x1BA);
                self->flash = 0x3C;
                advance(p, 6);
            }
            break;
        case 6:
            if (p->timer > 0x1E) {
                advance(p, 7);
            } else {
                p->timer++;
            }
            break;
        case 7:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, animation, (u8 *)animation + 0x60, 0xF, 2,
                                                     2);
                Anim_SetFrameDurationScale(animation, 0x100);
                startPulse(self, 1, 8);
                setShake(self, 1, 2);
            }
            if (++p->timer > 0xFF) {
                u32 zero = 0;
                self->attackParameter5B8 = zero;
                self->attackParameter5BA = zero;
                advance(p, 8);
                Bestiary_UnlockEntry(0x42);
                Bestiary_UnlockEntry(0x43);
                Bestiary_UnlockEntry(0x52);
                p->scriptId = p->part.breaks;
                SceneActors_RunSecondEventScript(p, &p->coordinates.position);
                p->finished = 1;
            }
            break;
        case 8:
            return;
    }
    {
        u32 mask = 2;
        p->flags |= mask;
    }
}
