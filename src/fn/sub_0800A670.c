#include "global.h"
struct Vec {s16 x,y,z,pad;};
struct PackedVec {s32 x:16,y:16,z:16,pad:16;};
struct Sprite {u32 flags;u8 pad4[4];u16 tile;u8 width,height;u8 rest[12];struct Vec position;u8 tail[8];};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct Slot {u16 id;u8 mode;u8 type,kind,active,state,entered,done;u8 pad9;u16 timer,delay,attack,range,script;u8 pad14[84];struct Sprite sprite;struct Anim anim;};
struct Graphics {u32 pad;u16 base;};
struct Manager {u8 pad[28];struct Graphics *graphics;struct Graphics *animationGraphics;void *hitResource;void *animationResource;};

extern struct Manager *gUnk_0200003C;
struct Manager *sub_0800A62C(u32,u32);struct Slot *sub_08009E3C(struct Manager *);void sub_08219DD8(void *,u32);s32 sub_0821ABA8(u32,s32);s32 sub_08009E98(struct Manager *,struct Slot *);s32 Script_SeekToKeyword(u32);s32 Script_GetValue(void);void sub_0821FF24(void *,struct PackedVec *,u32);
#define CollisionController_ScriptCreate sub_0800A670
s32 CollisionController_ScriptCreate(void){
 struct PackedVec position;struct Manager *manager=gUnk_0200003C;struct Slot *p;
 if(!manager){manager=sub_0800A62C(0,0);if(!manager)goto fail;}
 p=sub_08009E3C(manager);if(!p){fail:return -1;}
 sub_08219DD8(p,160);
 p->id=sub_0821ABA8('n',0);p->type=sub_0821ABA8('t',0);p->kind=sub_0821ABA8('b',0);
 p->done=0;p->mode=0;p->timer=0;p->entered=1;p->delay=0;p->state=0;
 sub_08009E98(manager,p);
 if(Script_SeekToKeyword('p')){position.x=Script_GetValue();position.y=Script_GetValue();position.z=Script_GetValue();}
 else{position.x=0;position.y=0;position.z=0;}
 sub_0821FF24((u8 *)p+20,&position,0);
 p->sprite.position=*(struct Vec *)&position;p->sprite.position.y+=230;
 if(Script_SeekToKeyword('s')){p->attack=Script_GetValue();p->range=Script_GetValue();p->script=Script_GetValue();}
 else{p->attack=50;p->range=600;p->script=100;}
 return 0;
}
