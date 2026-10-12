#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Choice {u8 x,y,width,row;};
struct Menu {u8 task[24];s8 selected,initial;u8 delay,count;u8 rowStart[8],rowEnd[8],rowCount[8];u8 rows,useScript,left,top;u8 pad38[2];u8 width,textX,allowCancel,wakeDelay,repeatDelay,explicitWidth;u32 held;u32 arguments[4];u32 script;void *callback,*window;u32 unknown60;struct Choice choices[16];u8 scratch[64];struct Resource {u32 words[8];} resource;u8 leftPrefix[8];u32 leftFlags;u8 pad110[20];s16 leftX,leftY;u8 pad128[68];u32 rightFlags;u8 pad170[20];s16 rightX,rightY;};
#define ChoiceMenu_CreateFromScript sub_08006D44
extern struct Menu *gUnk_0200002C;
s32 Script_SeekToKeyword(s32);s32 Script_GetValue(void);s32 Script_GetPc(void);
s32 sub_0821ABA8(s32,s32);void *sub_08227E90(void);s32 sub_08031D90(s32 *);
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void (*)(void),void (*)(void));
void sub_080066C0(void);void sub_08006710(void);void sub_0821A0C0(void *);
s32 sub_08006B60(struct Menu *,void *,s32,s32 *);
struct Menu *ChoiceMenu_CreateFromScript(void)
{
    s32 options[7],arguments[4],defaults[4];
    void *window;
    s32 stringBase,script;
    struct Menu *p;
    s32 i;
    register s32 *argumentBase asm("r5");
    if(gUnk_0200002C)return 0;
    if(Script_SeekToKeyword('r'))window=sub_08227E90();else window=0;
    stringBase=sub_0821ABA8('i',0);
    script=0;
    if(Script_SeekToKeyword('p'))script=Script_GetValue();
    options[1]=sub_0821ABA8('l',0);options[0]=sub_0821ABA8('w',0);options[2]=sub_0821ABA8('c',0);
    if(Script_SeekToKeyword('a')){
        options[3]=Script_GetValue();options[4]=Script_GetValue();options[5]=Script_GetValue();options[6]=Script_GetValue();
    }else if(!sub_08031D90(defaults)){
        options[3]=defaults[0];options[4]=defaults[1];options[5]=defaults[2];options[6]=defaults[3];
    }else{options[3]=0;options[4]=0;options[5]=24;options[6]=3;}
    if(Script_SeekToKeyword('A')){
        register s32 *arg asm("r4");argumentBase=arguments;arg=argumentBase;
        do {s32 value=Script_GetPc();if(value)value=Script_GetValue();*arg=value;asm("" : : "r"(arg));arg++;}while((s32)arg<=({register s32 limit asm("r0")=(s32)argumentBase;asm("" : "+r"(limit));limit+12;}));
    }else {s32 *arg;register s32 zero asm("r2");register s32 *begin asm("r1");argumentBase=arguments;begin=argumentBase;asm("" : "+r"(begin));zero=0;asm("" : : "r"(zero));arg=arguments+3;do{*arg=zero;arg--;}while((s32)arg>=(s32)begin);}
    p=sub_08219FBC(11,0x1E8);
    if(p){
        sub_0821A04C(p,sub_080066C0,sub_08006710);
        p->useScript=1;p->script=script;
        {register s32 *source asm("r3")=argumentBase;u32 *destination=p->arguments;for(i=3;i>=0;i--)*destination++=*source++;}
        if(sub_08006B60(p,window,stringBase,options)<0){sub_0821A0C0(p);return 0;}
    }
    return p;
}
