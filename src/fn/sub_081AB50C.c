#include "global.h"
/* Vanargand_RunDefeatSequence: waits, rises along a motion track, approaches the part position while the paired halves play cues 0x14/0x18/0x1A, then plays animation 0xF for 256 frames, sets event flags 0x42/0x43/0x52 and runs the second event script (provisional name) */
#define Vanargand_RunDefeatSequence sub_081AB50C
struct VanargandPart {
    u8 pad0[0x10];
    u16 state;
};
struct VanargandPoint {
    u32 a, b;
};
struct Vanargand {
    u8 pad0[0x54];
    u8 motionData[0x9C - 0x54];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[0xAF - 0xA8];
    u8 finished, entered, pendingAnimation, ready;
    u8 padB3[0xF0 - 0xB3];
    u32 scriptId;
    u8 padF4[0x17C - 0xF4];
    s16 eventPosition[8];
    u8 animation[0x60];
    u8 drawable[0x3FC - 0x1EC];
    struct VanargandPart part;
    u8 pad410[0x418 - 0x410];
    struct VanargandPoint goal;
    u8 pad420[0x500 - 0x420];
    u16 partActive;
    u16 script;
    u8 pad504[0x534 - 0x504];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0x56C - 0x55E];
    void *attachment;
    u8 pad570[0x5B0 - 0x570];
    u16 delay;
    u8 pad5B2[0x5B8 - 0x5B2];
    u16 field5B8, field5BA;
    u8 pad5BC[0xF18 - 0x5BC];
    u32 fieldF18, fieldF1C;
    u16 fieldF20, fieldF22;
    u8 padF24[0xF3C - 0xF24];
    u16 flash;
    u8 padF3E[0xF44 - 0xF3E];
    void *afterimages;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
void Anim_SetFrameDurationScale(void *, u32);
static inline u8 complete(struct Vanargand *p)
{
    u8 result = 0;
    if (p->trackState == 2)
        result = 1;
    return result;
}

void Sound_PlaySong(u32);
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void SceneActors_RunFirstEventScript(void *, s16 *);
void SceneActors_RunSecondEventScript(void *, s16 *);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);
u32 PairedSceneActor_AreSelectedHalvesFinished(void *);
s32 ScenePair_CountPositiveCounters(void *);
void PositionMotion_InitParameterizedApproach(void *, void *, struct VanargandPoint *, u32, u32,
                                              u32, u32, u32);
void GameState_SetEventFlag(u32);

static inline void setOrigin(s16 *origin, s16 x, s16 y, s16 z)
{
    s16 *q = origin;
    *q = x;
    origin[1] = y;
    origin[2] = z;
}

static inline void startPulse(struct Vanargand *p, u16 a, u16 b)
{
    p->fieldF1C = 0;
    p->fieldF20 = a;
    p->fieldF22 = b;
    p->fieldF18 = 0;
}

static inline void setShake(struct Vanargand *p, u16 a, u16 b)
{
    p->field5B8 = a;
    p->field5BA = b;
}

static inline void advance(struct Vanargand *p, u8 phase)
{
    p->phase = phase;
    p->timer = 0;
}

void sub_081AB50C(struct Vanargand *p)
{
    struct Vanargand *self = p;
    void *animation = p->animation;
    s16 origin[4];
    struct VanargandPoint first, second;
    {
        u32 mask = 0xC;
        p->flags |= mask;
    }
    Vanargand_SetPendingCue(p, 3);
    if (p->pendingAnimation) {
        p->pendingAnimation = 0;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 4, 0, 4);
        advance(p, 0);
    }
    switch (p->phase) {
        case 0:
            if (++p->timer > 8 && !Player_IsSlotInActionTwelveOrThirtyEight(0)) {
                Sound_PlaySong(0x585);
                p->scriptId = self->script;
                SceneActors_RunFirstEventScript(p, self->eventPosition);
                advance(p, 1);
            }
            break;
        case 1:
            if (p->timer == 0) {
                setOrigin(origin, 0x140, 0x400, 0x140);
                MotionTrack_Initialize(self->track, p->motionData, origin, 0, 4);
                PairedSceneActor_ApplyToSelectedHalves(self->attachment, 3, 2);
                setShake(self, 8, 3);
            }
            p->timer++;
            MotionTrack_Update(self->track);
            if (complete(self)) {
                advance(p, 2);
            }
            break;
        case 2:
            if (p->timer == 0) {
                {
                    struct VanargandPoint *v = &first;
                    *v = self->goal;
                }
                PositionMotion_InitParameterizedApproach(self->track, p->motionData, &first, 0x20,
                                                         0x20, 0x20, 0, 1);
                PairedSceneActor_ApplyToSelectedHalves(self->attachment, 3, 0x14);
            }
            p->timer++;
            MotionTrack_Update(self->track);
            if (PairedSceneActor_AreSelectedHalvesFinished(self->attachment)) {
                if (ScenePair_CountPositiveCounters(self->attachment) > 0) {
                    Sound_PlaySong(0x1BA);
                    self->flash = 0x3C;
                }
                advance(p, 3);
            }
            break;
        case 3:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(self->attachment, 3, 0x18);
                p->timer++;
            }
            if (PairedSceneActor_AreSelectedHalvesFinished(self->attachment)) {
                advance(p, 4);
            }
            break;
        case 4:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(self->attachment, 3, 0x1A);
                p->timer++;
            }
            if (PairedSceneActor_AreSelectedHalvesFinished(self->attachment)) {
                advance(p, 5);
            }
            break;
        case 5:
            if (p->timer == 0) {
                {
                    struct VanargandPoint *v = &second;
                    *v = self->goal;
                }
                PositionMotion_InitParameterizedApproach(self->track, p->motionData, &second, 0x20,
                                                         0x20, 0x20, 0, 1);
            }
            p->timer++;
            MotionTrack_Update(self->track);
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
                self->field5B8 = zero;
                self->field5BA = zero;
                advance(p, 8);
                GameState_SetEventFlag(0x42);
                GameState_SetEventFlag(0x43);
                GameState_SetEventFlag(0x52);
                p->scriptId = p->script;
                SceneActors_RunSecondEventScript(p, p->eventPosition);
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
