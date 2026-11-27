#include "global.h"
#include "scene/collision/contact_actor.h"
#include "engine/video/display_sprite_playback.h"
// confidence=low: advances falling motion entries and releases them at ground or expiry.
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
static inline void HresvelgrMotion_AddVelocity(struct ContactPosition *out,
                                               struct ContactPosition *v)
{
    out->x = out->x + v->x;
    out->y = out->y + v->y;
    out->z = out->z + v->z;
}
void HresvelgrMotion_UpdateTimedFallingSprite(struct MotionEntry *input)
{
    struct MotionEntry *p = input;
    HresvelgrMotion_AddVelocity(&p->sprite.position, &p->velocity);
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
