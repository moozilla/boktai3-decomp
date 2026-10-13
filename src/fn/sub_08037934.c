#include "global.h"
struct PeerMenu {u8 pad0[0x19];u8 state,entered,pad1b[5];u32 timer;u8 pad24[0x4c8];int window;};
void sub_08033924(int,int);
void sub_08037934(struct PeerMenu*p){if(p->entered){p->entered=0;sub_08033924(p->window,9);}p->timer++;}
