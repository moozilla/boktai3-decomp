#include "global.h"
struct Choice {u8 x,y,width,row;};
struct Menu {u8 task[24];s8 selected,initial;u8 delay,count;u8 rowStart[8],rowEnd[8],rowCount[8];u8 rows,useScript,left,top;u8 pad38[2];u8 width,textX,allowCancel,wakeDelay,repeatDelay,explicitWidth;u32 held;u32 arguments[4];u32 script;void *callback,*window;u32 unknown60;struct Choice choices[16];u8 scratch[104];u32 leftFlags;u8 pad110[20];s16 leftX,leftY;u8 pad128[68];u32 rightFlags;u8 pad170[20];s16 rightX,rightY;};
#define ChoiceMenu_DispatchSelection sub_08006374
struct Args {u32 count:16;u32 unused:16;u32 *values;};
void sub_0821AD08(u32,struct Args *);
void sub_0824923C(s32,void *);
static inline u32 selectionScript(struct Menu *p){return p->script;}
void ChoiceMenu_DispatchSelection(struct Menu *p,s32 selection)
{
    u32 values[5];struct Args args;
    if(p->useScript){
        s32 i;u32 script;
        values[0]=selection;i=0;
        for(;i<4;i++)values[i+1]=p->arguments[i];
        args.count=5;args.values=values;script=selectionScript(p);
        if(script)sub_0821AD08(script,&args);
    }else sub_0824923C(selection,p->callback);
}
