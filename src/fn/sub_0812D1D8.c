#include "player_cue_sequence.h"
struct Vec {
    s16 x, y, z, pad;
};
struct FeatherSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct Feather {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct Vec position;
    u8 unknown24[8];
    struct FeatherSequence sequence;
    u8 unknown3C[0x54];
    u16 age;
    u8 unknown92[4];
    u8 state;
    s8 slot;
    u8 unknown98[4];
    u16 timer;
    u8 unknown9e, enter;
};
struct FeatherPool {
    u8 task[0x18];
    u8 graphics[0x1c];
    void *sequences;
    u32 used;
};
void LargeFragmentEffect_UpdateCollider(u8 *);
s32 Mod(s32, s32);
void LargeFragmentEffect_SpawnRandomParticle(struct Vec *);
void LargeFragmentEffect_UpdateAnimation(void *unused, struct Feather *p)
{
    if (p->enter) {
        p->flags &= ~1;
        p->timer = 0;
        Sound_PlaySong(0x145);
        p->enter = 0;
    }
    if ((u16)(p->sequence.index - 1) <= 2) {
        LargeFragmentEffect_UpdateCollider((u8 *)p);
        if (Mod(p->timer, 6) == 0)
            LargeFragmentEffect_SpawnRandomParticle(&p->position);
    }
    p->timer++;
    if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)p,
                                    (struct DisplaySequenceState *)&p->sequence)) {
        p->state = 3;
        p->enter = 1;
    }
}
