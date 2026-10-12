#include "global.h"
struct Object{u8 pad0[9];u8 angle;u8 pada[0x42];int health;u32 flags;u8 pad54[4];int damage;u32 source,attackFlags;u8 pad64[2];u16 reaction;u8 pad68[0xe0];int maxHealth;};struct Collider{u8 pad0[0x34];u32 source,flags;u8 pad3c[2];u16 damage;u8 pad40[4];u16 reaction;};
void sub_0802C934(struct Object*p,int health){p->health=health;if(health<0)p->health=0;else if(health>p->maxHealth)p->health=p->maxHealth;}
