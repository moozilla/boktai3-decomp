#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Command{u8 pad[6],type;};
struct Queue{u8 pad[6],count[2];struct Command *commands[2][4];};
struct Globals{u8 pad[36],bank;};extern struct Globals *gUnk_0200047C;
void sub_08020D54(void*,void*);void sub_08020D68(void*,int);
int sub_0800FC28(void*,void*,void*);int sub_0800FE98(void*,void*,void*);int sub_08010298(void*,void*,void*);int sub_0801010C(void*,void*,void*);
static inline int count(struct Queue *q){register u8 *bank asm("r1")=&gUnk_0200047C->bank;return q->count[*bank];}
#define BoardPieces_DispatchCommands sub_0801037C
int BoardPieces_DispatchCommands(u8 *manager){
 struct Queue *queue=(void*)(manager+0xCD4);int i;
 for(i=0;i<count(queue);i++){
  register struct Globals *g asm("r0")=gUnk_0200047C;asm("" : "+r"(g));{struct Command *cmd=queue->commands[g->bank][i];sub_08020D54(queue,cmd);
  switch(cmd->type){
   case 0:sub_0800FC28(manager,queue,cmd);sub_08020D68(manager+0xCD4,1);break;
   case 1:sub_0800FE98(manager,queue,cmd);sub_08020D68(manager+0xCD4,1);break;
   case 2:sub_08010298(manager,queue,cmd);sub_08020D68(manager+0xCD4,1);break;
   case 11:sub_0801010C(manager,queue,cmd);sub_08020D68(manager+0xCD4,1);break;
  }
 }}
 return 0;
}



