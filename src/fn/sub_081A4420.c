#include "global.h"
#include "contact_actor.h"
#include "display_sprite_playback.h"
// confidence=low: advances falling motion entries and releases them at ground or expiry.
#define MotionEntry_UpdateFalling sub_081A4420
struct MotionEntry {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    struct ContactPosition savedPosition, velocity;
    void *origin;
    u16 lifetime, tick, radius, height;
    u8 type, mode;
    u16 angularStep;
};
void Hresvelgr_ReleaseEffect(void *);
static inline void MotionEntry_AddVelocity(struct ContactPosition *out, struct ContactPosition *v)
{
    out->x = out->x + v->x;
    out->y = out->y + v->y;
    out->z = out->z + v->z;
}
void MotionEntry_UpdateFalling(struct MotionEntry *input)
{
    struct MotionEntry *p = input;
    MotionEntry_AddVelocity(&p->sprite.position, &p->velocity);
    if (p->velocity.y > (-4))
        p->velocity.y--;
    if (p->sprite.position.y <= 0) {
        Hresvelgr_ReleaseEffect(p);
    } else {
        if (p->type != 3)
            PlayerCue_StepSequenceEnded(&p->sprite, &p->sequence);
        if ((--p->lifetime) == 0)
            Hresvelgr_ReleaseEffect(p);
    }
}
