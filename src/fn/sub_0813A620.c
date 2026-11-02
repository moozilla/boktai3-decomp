#include "player_cue_sequence.h"
#include "background_controller.h"

/* tentative: positions two sprites relative to camera and steps their frame animation */

struct PairPos {
    s32 x, y, z;
};

struct PairEntry {
    u8 pad0[0x1c];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState anim;
    struct PairPos *pos;
    s32 *offset;
    void *active;
};

struct PairManager {
    u8 task[0x18];
    struct PairEntry entries[2];
};

struct PairCamera {
    u8 pad[0x44];
    s32 x;
};

s32 SpritePair_UpdateAnimation(struct PairManager *m)
{
    s32 i = 0;
    for (; i < 2; i++) {
        struct PairEntry *e = &m->entries[i];
        if (!e->active)
            continue;
        e->sprite.position.x =
            (e->pos->x >> 12) - (s16)(((struct PairCamera *)gSceneCamera)->x - 16);
        e->sprite.position.y = ((e->pos->z + e->pos->y + *e->offset) >> 12) + 0x4e;
        PlayerCue_StepSequence(&e->sprite, &e->anim);
    }
    return 0;
}
