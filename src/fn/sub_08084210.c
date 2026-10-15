// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_helpers.h"
#define SceneActorControl_TickHiddenResponse sub_08084210
void sub_0808484C(struct SceneActorRegistryResetView *);
void sub_080838AC(struct SceneActorRegistryResetView *);
s32 SceneActorControl_TickHiddenResponse(struct SceneActorRegistryResetView *actor)
{
    u32 mask=0x4000;
    u32 offset=0x2D4;
    register u16 *field asm("r0");
    field=(void *)((u8 *)actor+offset);
    if(*field&mask){
        u32 mask=0x10;
        register u32 *input asm("r1");
        u32 *flags;
        u32 enabled;
        input=(void *)((u8 *)actor+0x3D4);
        enabled=*input&mask;
        flags=input;
        if(SceneActorControl_TestFlags(enabled,~0)){
            u32 one=1;
            if(actor->drawableKind==0){
                u32 *field=actor->drawable;
                *field|=one;
            }else{
                struct Tile {u8 unknown0[8];u32 flags;};
                struct Tile *tile=(void *)((u8 *)actor->drawable+0x20);
                tile->flags|=one;
            }
            {
                u32 mask=~0x60;
                *flags&=mask;
            }
        }else sub_0808484C(actor);
    }
    sub_080838AC(actor);
    return 1;
}
