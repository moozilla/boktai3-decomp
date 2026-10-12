#include "global.h"
typedef struct {u8 task[24];int text;u32 script;int timer,result;void *message;} ResultPrompt;
extern int gUnk_03005408;extern u16 gUnk_03005260[];
u8 sub_0822BE20(void);void sub_08033924(int,int);void sub_0822B2F8(int);void sub_08177DEC(int);void sub_0821AD08(u32,void*);void sub_0821A0C0(void*);
#define ResultPrompt_Update sub_0800F850
int ResultPrompt_Update(ResultPrompt *p){
 if(p->result<120)gUnk_03005408=1;
 if(p->result<0){if(p->timer==60){p->result=sub_0822BE20();if(p->result==1){sub_08033924(p->text,14);sub_0822B2F8(686);}else{sub_08033924(p->text,15);sub_0822B2F8(402);}}}
 if((p->result>=0&&p->timer>119&&(gUnk_03005260[1]&1))||p->timer>299){
  sub_08177DEC(7);if(p->script){int value=p->result;struct {u32 count:16;u32 pad:16;int *values;} args;args.count=1;args.values=&value;sub_0821AD08(p->script,&args);p->script=0;}sub_0821A0C0(p);
 }
 p->timer++;return 0;
}
