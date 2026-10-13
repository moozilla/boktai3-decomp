#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Tentative: apply a terrain-triggered resource cost and feedback. */
#define Scene_ApplyTerrainInteraction sub_080045E8
struct Actor {u8 type,pad1[3];u16 kind;u8 pad6[16];s16 resource;u8 timer,pad19[3];u16 animation;u8 pad1E,state,pad20,cooldown;u8 tail[0x92];};
struct Contact {u8 pad[0x34];u32 flags,flags2;};
void sub_08215284(void *,u16);
void sub_0822B2F8(s32);
void sub_08015C30(void *,s32,s32,s32,s32,s32,s32,s32);
void Scene_ApplyTerrainInteraction(struct Contact *contact,void *unused,struct Actor *actor)
{
    register struct Actor *incoming asm("r0")=actor;
    s32 applied;
    applied=0;
    actor=incoming;
    if(actor->type==0){
        if(({u32 mask=0x17;mask&contact->flags;})&&({u32 mask=0x10;mask&contact->flags2;})){
            actor->resource-=24;actor->timer=10;actor->state=2;
            sub_08215284((u8 *)actor+0x74,actor->animation);
            applied=1;
        }
    }else if(actor->type==1){
        if(({u32 mask=4;mask&contact->flags2;})&&({u32 mask=0x80017;mask&contact->flags;})){
            actor->resource-=24;actor->timer=10;actor->state=2;
            sub_08215284((u8 *)actor+0x74,actor->animation);
            if(!actor->cooldown){
                actor->cooldown=10;
                sub_0822B2F8(0x15C);
                sub_08015C30((u8 *)actor+0x38,0,0,0x40,12,12,4,4);
            }
            applied=1;
        }
    }else if(actor->type==2){
        if(({u32 mask=1;mask&contact->flags;})){
            actor->resource-=24;actor->timer=10;actor->state=2;
            sub_08215284((u8 *)actor+0x74,actor->animation);
            applied=1;
        }
    }
    if(applied){
        if((u16)(actor->kind-1)<=1){if(actor->resource<=0)actor->resource=1;else goto sound;}
        if(actor->resource<0)actor->resource=0;
        else {sound:sub_0822B2F8(0x13E);}
    }
}
