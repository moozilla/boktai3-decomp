#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Blob {u32 words[11];};
struct P {u8 pad[0x84];struct Blob blob;};
struct Player {u8 pad[0x9c];struct Blob blob;};
struct Item {u8 pad[0x25];u8 kind;u8 pad2[0x13];u8 flag;};
extern struct Player *gUnk_020004F0;
void sub_0811D4AC(void*,int,int,int,int,int,int);
void sub_0803BB04(struct P *p,struct Item *item)
{
    struct Player *player=gUnk_020004F0;
    int okay=0;
    int x=0,z=0;
    if(player) {
        switch(item->kind) {
        case 0:
            if(!item->flag) {
                okay=1;
                z=1024;
                asm("" : "+r"(okay), "+r"(z));
            } else {
                okay=1;
                z=1024;
                asm("" : "+r"(z), "+r"(okay));
            }
            x=z;
            break;
        case 1:
            if(!item->flag) {
                okay=1;
                z=1024;
                asm("" : "+r"(okay), "+r"(z) :: "memory");
            } else {
                okay=1;
                z=1024;
                asm("" : "+r"(z), "+r"(okay) :: "memory");
            }
            x=z;
            break;
        case 2:
            okay=1;
            x=2048;
            z=1024;
            break;
        case 3:
            okay=1;
            z=2048;
            x=z;
            break;
        }
    }
    if(okay) {
        p->blob=player->blob;
        p->blob.words[4]=0x2000;
        p->blob.words[5]+=x;
        p->blob.words[9]+=z;
    } else sub_0811D4AC(p,0x2000,0x2000,-0x1000,0x4000,okay,0x8000);
}
