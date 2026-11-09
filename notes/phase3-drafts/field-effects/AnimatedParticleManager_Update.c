// Pin-free draft of AnimatedParticleManager_Update (src/fn/sub_08015160.c) on the shared
// DisplaySequence_StepSprite helper: 32 ROM / 32 draft; AGBCC_PRI=p:+245 matches.
#include "global.h"
#include "display_sequence_sprite.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Piece {
    u8 active, pad1;
    u16 timer;
    s16 verticalScale, horizontalScale;
    u16 verticalStart, verticalDecay, horizontalStart, horizontalDecay;
    struct Vec direction, velocity;
    u8 resource[28];
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState anim;
};
struct Manager {
    u8 pad[32];
    u32 timer;
    struct Piece pieces[32];
};
void Sprite_Remove(void *);
static inline int scale8(int a, int b)
{
    int x = a * b;
    if (x >= 0)
        return x >> 8;
    return -((-x) >> 8);
}
static inline int scale12(int a, int b)
{
    int x = a * b;
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
int AnimatedParticleManager_Update(struct Manager *m)
{
    struct DisplaySequenceSprite *sprites = &m->pieces[0].sprite;
    struct Piece *cursor = m->pieces, *base = cursor;
    int i;
    for (i = 31; i >= 0; sprites = (void *)((u8 *)sprites + 120), base++, cursor++, i--) {
        if (cursor->active) {
            struct Piece *p = base;
            struct DisplaySequenceSprite *o = sprites;
            if (cursor->timer >= cursor->horizontalStart) {
                p->horizontalScale = scale8(cursor->horizontalScale, cursor->horizontalDecay);
                p->velocity.x = scale12(p->direction.x, p->horizontalScale);
                p->velocity.z = scale12(p->direction.z, p->horizontalScale);
            }
            if (p->timer >= p->verticalStart) {
                p->verticalScale = scale8(p->verticalScale, p->verticalDecay);
                p->velocity.y = scale12(p->direction.y, p->verticalScale);
            }
            o->position.x += p->velocity.x;
            o->position.y += p->velocity.y;
            if (!(o->flags & 16))
                o->position.z += p->velocity.z;
            if (DisplaySequence_StepSprite(o, &p->anim)) {
                p->active = 0;
                Sprite_Remove(o);
            }
            p->timer++;
        }
    }
    m->timer++;
    return 0;
}
