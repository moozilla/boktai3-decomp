#include "global.h"
#include "display_sprite_playback.h"
struct Entry {
    u32 timer;
    u8 pad4[0x14];
    s32 x, y, z;
    u8 pad24[4];
    struct DisplaySequenceState anim;
    u8 pad38[2];
    u16 lifetime;
    u8 pad3c[8];
    u32 callback;
    s32 vx, vy, vz;
    u8 pad54[0x10];
    u32 script;
    u8 pad68[4];
    u8 pad6c[0x1c];
    struct ContactSwarmSprite sprite;
    u8 padtail[0xb8 - 0x88 - sizeof(struct ContactSwarmSprite)];
};
struct Pool {
    u8 pad[0x18];
    u32 mask;
    struct Entry e[32];
};
void sub_081FEDE8(struct Pool *, struct Entry *, s32);
s32 sub_08249240(struct Entry *, s32, u32);
s32 SpriteParticle_UpdateManager(struct Pool *pool)
{
    struct Entry *base = pool->e, *cursor;
    s32 i;
    for (i = 0, cursor = base; i <= 31; i++, cursor++, base++) {
        if (pool->mask & (1u << i)) {
            struct ContactSwarmSprite *o = &base->sprite;
            int done = 0;
            if (sub_08249240(base, i, cursor->callback) == 1) {
                sub_081FEDE8(pool, base, i);
                continue;
            }
            cursor->x += cursor->vx;
            cursor->y += cursor->vy;
            cursor->z += cursor->vz;
            o->position.x = cursor->x >> 12;
            o->position.y = cursor->y >> 12;
            o->position.z = cursor->z >> 12;
            if (cursor->script) {
                struct ContactSwarmSprite *sprite = o;
                struct DisplaySequenceState *sequence = &base->anim;
                const u16 *command = cursor->anim.commands + cursor->anim.index;
                done = PlayerCue_AdvanceCommand(sprite, sequence, command, 1);
            }
            if (cursor->timer > cursor->lifetime || done == 1)
                sub_081FEDE8(pool, base, i);
            cursor->timer++;
        }
    }
    return 0;
}
