#include "global.h"
#include "affine_background.h"
#define AffineBackground_QueueCommand sub_08002A04
extern struct AffineBackground *gUnk_02000010[];
void AffineBackground_QueueCommand(u32 layer,u32 type,s32 length,s32 *args)
{
    struct AffineBackground *p=gUnk_02000010[layer];
    if(p && p->count<=7) {
        s32 j;
        p->types[p->count]=type;
        for(j=0;j<length;j++)p->args[p->count][j]=*args++;
        p->count++;
    }
}
