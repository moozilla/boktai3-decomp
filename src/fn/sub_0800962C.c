#include "global.h"
struct Controller {u8 pad[4];u8 disabled,state,entered,level;};
void sub_080095B4(struct Controller *,u32);
#define LevelController_Update sub_0800962C
void LevelController_Update(void *manager,struct Controller *p){
 if(p->disabled!=1){
 switch(p->state){
 case 0:if(p->level==0)sub_080095B4(p,2);else{sub_080095B4(p,1);p->level--;}break;
 case 1:if(p->level>4)sub_080095B4(p,0);else{sub_080095B4(p,1);p->level++;}break;
 case 2:p->level=5;sub_080095B4(p,0);break;
 }
 }
}
