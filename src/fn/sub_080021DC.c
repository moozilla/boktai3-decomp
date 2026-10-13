#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define Background_QueueCommand sub_080021DC
#include "background_controller.h"
extern struct Background *gUnk_02000000[];
void Background_QueueCommand(u32 layer,u32 type,s32 length,s32 *args)
{
    u32 commandType=type;
    register s32 lengthLimit asm("r5")=length;
    register s32 *input asm("r12")=args;
    struct Background *p=gUnk_02000000[layer];
    if(p && p->count<=7) {
        s32 j;
        p->types[p->count]=commandType;
        j=0;
        if(j<lengthLimit) {
            /* Retain the original count reload on each argument copy. */
            volatile u32 *count=&p->count;
            struct BackgroundCommand *commands=p->commands;
            s32 *source=input;
            do { *(s16 *)((u32)commands+((j<<1)+(*count<<4)))=*source++; j++; } while(j<lengthLimit);
        }
        p->count++;
    }
}
