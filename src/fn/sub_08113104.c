#include "global.h"
// confidence=low: planar debris lifetime follows completion of its visual animation.
/* ROM copies an uninitialized projected depth halfword; keep it uninitialized to match. */
#include "actor_attachment_state.h"
#include "background_controller.h"
#include "player_cue_sequence.h"
struct Visual {
    u8 unknown0[0x1c];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
};
struct VisualAnim {
    u8 unknown0[0x40];
    struct ContactPosition position;
};
struct Debris {
    struct Visual *visual;
    struct ContactPosition position, velocity;
    u32 timer;
    u8 restitution, active, entered, visualType;
    void (*update)(struct Debris *);
};
struct Camera {
    u8 unknown0[0x44];
    s32 x, y;
};
struct Packed {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
s32 Div(s32, s32);
s32 Terrain_GetCellClearance(void *);
void SceneDebris_OnMotionEnd(struct Debris *);
void SceneDebris_UpdateAnimatedMotion(struct Debris *p)
{
    struct Packed projected;
    s32 ended;
    struct ContactPosition *output;
    if (p->entered) {
        u32 *flags;
        if (!p->visualType)
            flags = &p->visual->sprite.flags;
        else
            flags = (u32 *)((u8 *)p->visual + 0x28);
        {
            u32 mask = ~1;
            *flags &= mask;
        }
        p->entered = 0;
    }
    p->position.x = p->position.x + p->velocity.x;
    p->position.y = p->position.y + p->velocity.y;
    projected.x = p->position.x - gSceneCamera->x;
    projected.y = p->position.y - gSceneCamera->y;
    output = (void *)&projected;
    if (!p->visualType) {
        struct Visual *visual = p->visual;
        visual->sprite.position.x = ((struct ContactPosition *)&projected)->x;
        visual->sprite.position.y = output->y;
        visual->sprite.position.z = output->z;
    } else {
        struct VisualAnim *visual = (void *)p->visual;
        visual->position.x = ((struct ContactPosition *)&projected)->x;
        visual->position.y = output->y;
        visual->position.z = output->z;
    }
    if (!p->visualType) {
        ended = PlayerCue_StepSequenceEnded(&p->visual->sprite, &p->visual->sequence);
    } else
        ended = Anim_Step((u8 *)p->visual + 0x20, p->visual);
    if (ended) {
        p->update = SceneDebris_OnMotionEnd;
        p->entered = 1;
    }
}
