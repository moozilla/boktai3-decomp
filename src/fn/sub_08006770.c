#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Choice {u8 x,y,width,row;};
struct Menu {u8 task[24];s8 selected,initial;u8 delay,count;u8 rowStart[8],rowEnd[8],rowCount[8];u8 rows,useScript,left,top;u8 pad38[2];u8 width,textX,allowCancel,wakeDelay,repeatDelay,explicitWidth;u32 held;u32 arguments[4];u32 script;void *callback,*window;u32 unknown60;struct Choice choices[16];u8 scratch[104];u32 leftFlags;u8 pad110[20];s16 leftX,leftY;u8 pad128[68];u32 rightFlags;u8 pad170[20];s16 rightX,rightY;};
#define ChoiceMenu_ParseTag sub_08006770
extern const u8 gUnk_0824DB0C[],gUnk_0824DB14[],gUnk_0824DB1C[],gUnk_0824DB20[];
s32 sub_08032168(const u8 *,const u8 *,s32);
s32 sub_080321A0(const u8 *,s32);
s32 sub_08031DDC(s32);
s32 sub_08031DBC(s32);
u8 *Text_FindChar(u8 *,s32);
#define READ_NUMBER() ({ \
    s32 found=0,length=0,value; \
    while(*cursor!='>'){ \
        if(!*cursor){cursor++;break;} \
        if(*cursor=='=')found=({register s32 one asm("r4")=1;asm("" : : "r"(one));one;}); \
        else if(({register s32 f asm("r0")=found;asm("" : : "r"(f));f;}))p->scratch[length++]=*cursor; \
        cursor++; \
    } \
    p->scratch[length]=0; \
    if(({register s32 f asm("r1")=found;asm("" : : "r"(f));f;})&&length)value=sub_080321A0(p->scratch,length);else value=0; \
    value; \
})
#define READ_WIDTH() ({ \
    s32 found=0,length=0; \
    while(*cursor!='>'){ \
        if(!*cursor){cursor++;break;} \
        if(*cursor=='=')found=({register s32 one asm("r4")=1;asm("" : : "r"(one));one;}); \
        else if(({register s32 f asm("r0")=found;asm("" : : "r"(f));f;}))p->scratch[length++]=*cursor; \
        cursor++; \
    } \
    p->scratch[length]=0; \
    if(({register s32 f asm("r1")=found;asm("" : : "r"(f));f;})&&length){ \
        s32 value=sub_080321A0(p->scratch,length); \
        register u8 *out asm("r2")=&p->explicitWidth; \
        asm("" : : "r"(out));*out=value; \
    }else{ \
        register s32 zero asm("r0")=0; \
        register u8 *out asm("r3"); \
        asm("" : : "r"(zero));out=&p->explicitWidth; \
        asm("" : : "r"(out));*out=zero; \
    } \
})
u8 *ChoiceMenu_ParseTag(struct Menu *p,u8 *text)
{
    u8 *cursor=text;
    if(*text!='/'){
        if(!sub_08032168(text,gUnk_0824DB0C,5)){
            READ_WIDTH();
            cursor=Text_FindChar(cursor,'>');
            p->choices[p->count].x=(p->left+({register u8 *x asm("r4")=&p->textX;asm("" : : "r"(x));*x;}))<<3;
            p->choices[p->count].y=(p->top<<3)+(p->rows<<4);
            p->choices[p->count].row=p->rows;
            p->width=0;
        }else if(!sub_08032168(text,gUnk_0824DB14,6)){
            register s32 width asm("r1")=sub_08031DDC(READ_NUMBER());
            p->width+=width;p->textX+=width;
            cursor=Text_FindChar(cursor,'>');
        }else if(!sub_08032168(text,gUnk_0824DB1C,3)){
            register s32 width asm("r1")=sub_08031DBC(READ_NUMBER());
            p->width+=width;p->textX+=width;
            cursor=Text_FindChar(cursor,'>');
        }else goto unknown;
    }else if(!sub_08032168(text,gUnk_0824DB20,6)){
        cursor=Text_FindChar(text,'>');
        if(p->explicitWidth)p->choices[p->count].width=p->explicitWidth;
        else p->choices[p->count].width=p->width;
        p->count++;p->width=0;p->rowCount[p->rows]++;
    }else goto unknown;
    goto matchedTag;
unknown:
    {u8 *result=Text_FindChar(text,'>');
     register u8 *saved asm("r4")=result;
     asm("" : : "r"(saved));return result;}
matchedTag:
    text=cursor+1;
returnText:
    return text;
}
