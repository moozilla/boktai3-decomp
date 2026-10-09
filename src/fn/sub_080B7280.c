#include "global.h"
struct Parent { u8 pad0[0x6e0]; s32 a,value,b; u8 pad6EC[0x1c]; s32 c,copy; };
struct Obj { u8 pad0[0x98]; u8 state; u8 pad99[0x5b]; s32 value; u8 padF8[0x2d8]; struct Parent *parent; };
s32 Div(s32,s32);
void sub_080B7280(struct Obj *p)
{
    struct Parent *owner=p->parent;
    switch(p->state) {
    case 5:
        owner->a=80000; owner->c=80000; owner->copy=p->value; break;
    case 6:
        owner->a=102400; owner->value=p->value;
        owner->b=85535; owner->c=85535; owner->copy=owner->value; break;
    case 7:
        owner->a=102400; owner->value=p->value;
        owner->c=Div(p->value*6,10); owner->copy=owner->value; break;
    }
}
