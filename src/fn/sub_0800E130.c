#include "global.h"
typedef struct {u16 id,slot;u16 colors[16],overlay[16],blend[16];void *source[3];} Palette;
typedef struct {u8 task[24];u16 id,count,from,to;u8 blend,overlay;u16 previous,current,pad;Palette palettes[6];} Manager;
extern u8 gUnk_03005430[];extern u32 gUnk_03004DA4,gUnk_030051CC,gUnk_030051C0;
int sub_080036C0(void);int sub_080036DC(void);void sub_08227950(u16*,u16*);void sub_0800E0CC(Manager*,Palette*);void sub_0800E10C(Manager*,Palette*);void sub_08217344(u32,void*);
#define PaletteSet_Update sub_0800E130
int PaletteSet_Update(Manager *p){
 int amount=sub_080036C0(),changed=0,i;Palette *palette;
 if(p->current!=gUnk_03005430[14]){sub_08227950(&p->from,&p->to);goto refresh;}
 if(p->blend!=amount)changed=1;
 if(changed){refresh:p->blend=amount;for(i=0;i<p->count;i++)sub_0800E0CC(p,&p->palettes[i]);}
 amount=sub_080036DC();
 if(p->overlay!=amount){p->overlay=amount;for(i=0;i<p->count;i++)sub_0800E10C(p,&p->palettes[i]);}
 gUnk_03004DA4=gUnk_030051CC;gUnk_030051C0=0;
 for(i=0;i<p->count;i++){sub_08217344(p->palettes[i].slot,p->palettes[i].colors);gUnk_030051C0++;}
 p->previous=gUnk_03005430[15];p->current=gUnk_03005430[14];return 0;
}
