#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Choice {u8 x,y,width,row;};
struct Menu {u8 task[24];s8 selected,initial;u8 delay,count;u8 rowStart[8],rowEnd[8],rowCount[8];u8 rows,useScript,left,top;u8 pad38[2];u8 width,textX,allowCancel,wakeDelay,repeatDelay,explicitWidth;u32 held;u32 arguments[4];u32 script;void *callback,*window;u32 unknown60;struct Choice choices[16];u8 scratch[104];u32 leftFlags;u8 pad110[20];s16 leftX,leftY;u8 pad128[68];u32 rightFlags;u8 pad170[20];s16 rightX,rightY;};
#define ChoiceMenu_HandleInput sub_080064C4
struct Keys {u16 held,pressed;};
extern struct Keys gUnk_03005260;
void sub_080063D0(struct Menu *,s32);
void sub_08006374(struct Menu *,s32);
s32 sub_08006478(struct Menu *,s32,s32);
void sub_0822B2F8(s32);
void sub_08030BF8(void);
void sub_0821A0C0(void *);
s32 ChoiceMenu_HandleInput(struct Menu *p)
{
    p->leftFlags&=~1;p->rightFlags&=~1;
    if(p->selected<0){
        if((gUnk_03005260.pressed&240)||(!p->wakeDelay&&(gUnk_03005260.pressed&3))){
            sub_080063D0(p,p->initial);sub_0822B2F8(263);
        }else if(p->wakeDelay)p->wakeDelay--;
    }else if(p->wakeDelay)p->wakeDelay--;
    else{
        register struct Keys *initialKeys asm("r1")=&gUnk_03005260;
        struct Keys *keys;
        u8 *repeat;
        u32 pressed;
        u32 sameDirection;
        sameDirection=({u32 held=initialKeys->held;u32 mask=p->held;mask&=held;mask;});
        keys=initialKeys;
        if(sameDirection){
            u8 *ptr=&p->repeatDelay;
            register u32 delay asm("r1")=*ptr;
            repeat=ptr;
            if(delay){u32 value=delay-1;
                {register u8 *destination asm("r1")=repeat;asm("" : "+r"(destination));*destination=value;}}
        }else {p->repeatDelay=sameDirection;repeat=&p->repeatDelay;}
        p->held=keys->held&240;
        pressed=keys->pressed;
        if(pressed&2){
            if(p->allowCancel){
                sub_0822B2F8(222);sub_08030BF8();sub_08006374(p,-1);
                p->leftFlags|=1;p->rightFlags|=1;
                sub_0821A0C0(p);return 1;
            }
        }else {register u32 one asm("r4")=1;
        if(({u32 bits=one;bits&=pressed;asm("" : : "r"(bits));bits;})){
            sub_0822B2F8(221);sub_08030BF8();sub_08006374(p,p->selected);
            p->leftFlags|=one;p->rightFlags|=one;
            sub_0821A0C0(p);return 1;
        }}
        if(!({u8 *ptr=repeat;*ptr;})){
            register s32 currentSelection asm("r2")=p->selected;
            struct Choice *choice=&p->choices[currentSelection];
            s32 selection,limit;
            u32 held=keys->held;
            if(held&32){
                selection=currentSelection-1;
                limit=p->rowStart[choice->row];if(selection>=limit)goto applySelection;else goto clampSelection;
            }else if(held&16){
                selection=currentSelection+1;
                limit=p->rowEnd[choice->row];if(selection>limit)goto clampSelection;
            }else if(held&64){
                s32 column=currentSelection-({u32 value=p->rowStart[choice->row];value;});
                s32 row=sub_08006478(p,choice->row,-1);
                if(choice->row==row)selection=p->selected;else selection=p->rowStart[row]+column;
                limit=p->rowStart[row];if(selection<limit)goto clampSelection;
                limit=p->rowEnd[row];if(selection>limit)goto clampSelection;
            }else if(({u32 bits=128;bits&=held;bits;})){
                register s32 column asm("r6")=currentSelection-({register u32 value asm("r0")=p->rowStart[choice->row];value;});
                s32 row=sub_08006478(p,choice->row,1);
                if(choice->row==row)selection=p->selected;else selection=p->rowStart[row]+column;
                limit=p->rowStart[row];if(selection<limit)goto clampSelection;
                limit=p->rowEnd[row];if(selection>limit)goto clampSelection;
            }else return 0;
            goto applySelection;
        clampSelection:selection=limit;
        applySelection:
            sub_080063D0(p,selection);
            {u32 delay=30;*repeat=delay;}
        }
    }
    return 0;
}
