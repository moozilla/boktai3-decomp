#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define Background_QueueCommand sub_080021DC
struct Command { s16 args[8]; };
struct Background { u8 pad[0xA8]; u32 count; u16 types[8]; struct Command commands[8]; };
extern struct Background *gUnk_02000000[];
void Background_QueueCommand(u32 layer,u32 type,s32 length,s32 *args)
{
    register u32 commandType asm("r6")=type;
    register s32 lengthLimit asm("r5")=length;
    register s32 *input asm("r12")=args;
    struct Background *p=gUnk_02000000[layer];
    if(p && p->count<=7) {
        s32 j;
        p->types[p->count]=commandType;
        j=0;
        if(j<lengthLimit) {
            register s32 *source asm("r6")=input;
            do { p->commands[p->count].args[j]=*source++; j++; } while(j<lengthLimit);
        }
        p->count++;
    }
}
