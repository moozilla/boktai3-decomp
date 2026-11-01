#include "global.h"
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
void SceneDebris_UpdateBouncingMotion(struct Debris *p)
{
    struct Packed world, projected;
    s32 clearance;
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
    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;
    p->position.z += p->velocity.z;
    p->velocity.y++;
    if (p->position.z < 0) {
        p->position.z = 0;
        p->velocity.z = Div(-p->velocity.z * p->restitution, 10);
    } else if (p->position.z > 63) {
        p->position.z = 63;
        p->velocity.z = Div(-p->velocity.z * p->restitution, 10);
    }
    world.x = p->position.x;
    world.y = p->position.y;
    world.z = p->position.z;
    clearance = Terrain_GetCellClearance(&world);
    if (clearance < p->position.y) {
        p->position.y = clearance;
        p->velocity.y = Div(-p->velocity.y * p->restitution, 10);
        p->velocity.x = Div(p->velocity.y * p->restitution, 10);
    }
    projected.x = p->position.x - gSceneCamera->x;
    projected.y = p->position.y - gSceneCamera->y;
    projected.z = 0;
    if (!p->visualType) {
        struct Visual *visual = p->visual;
        visual->sprite.position.x = ((struct ContactPosition *)&projected)->x;
        visual->sprite.position.y = ((struct ContactPosition *)&projected)->y;
        visual->sprite.position.z = ((struct ContactPosition *)&projected)->z;
    } else {
        struct VisualAnim *visual = (void *)p->visual;
        visual->position.x = ((struct ContactPosition *)&projected)->x;
        visual->position.y = ((struct ContactPosition *)&projected)->y;
        visual->position.z = ((struct ContactPosition *)&projected)->z;
    }
    if (!p->visualType)
        PlayerCue_StepSequenceEnded(&p->visual->sprite, &p->visual->sequence);
    else
        Anim_Step((u8 *)p->visual + 0x20, p->visual);
    if (p->timer)
        p->timer--;
    else {
        p->update = SceneDebris_OnMotionEnd;
        p->entered = 1;
    }
}
