#include "global.h"
#include "engine/video/render.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
// confidence=low: palette transition accompanies the leap motion track.
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
void Anim_SetFrameDurationScale(void *, u32);

static inline u8 MotionComplete(struct VanargandPairAttack *p)
{
    u8 result = 0;
    if (p->track.active == 2)
        result = 1;
    return result;
}

static inline void StartTrack(void *track, void *data, s16 x, s16 y, s16 z,
                              struct MotionTrackVector *target, u32 mode,
                              struct MotionTrackVector *origin)
{
    s16 *q = &origin->x;
    *q = x;
    origin->y = y;
    origin->z = z;
    MotionTrack_Initialize(track, data, origin, target, mode);
}

void Sound_PlaySong(u32);
void VanargandAfterimagePool_Spawn(void *, void *);

void Vanargand_UpdatePaletteLeap(struct VanargandPairAttack *p, s32 frame)
{
    struct VanargandPairAttack *self = p;
    u8 *animation;
    struct MotionTrackVector first;
    animation = (u8 *)&p->animation;
    VanargandPairAttack_SetFlags(p, 4);
    Vanargand_SetPendingCue(p, 1);
    if (p->pending) {
        p->pending = 0;
        Vanargand_LoadStagePalette(p, self);
        SceneActors_StartUnmirroredAnimation(p, animation, animation + 0x60, 4, 3, 4);
        StartTrack(&self->track, &p->position, 0x280, 0x300, 0x280, 0, 4, &first);
        Sound_PlaySong(0x13E);
        self->delay = 8;
        {
            struct VanargandAttackPair *pair = self->pair;
            u8 alpha = 0xA0;
            pair->halves[0].field187 = alpha;
            self->pair->halves[1].field187 = alpha;
        }
        PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 0x16);
    }
    MotionTrack_Update(&self->track);
    if (MotionComplete(self) && frame > 0x3C) {
        struct VanargandDamagePart *part = &self->part;
        part->sprite.frame = 7;
        self->part.animation = 0;
        p->finished = 1;
        return;
    }
    VanargandAfterimagePool_Spawn(self->afterimages, &p->position);
    VanargandPairAttack_SetFlags(p, 5);
}
