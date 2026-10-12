#include "global.h"
struct Resource{u32 words[8];};
struct Menu{u8 task[24];u32 timer;u8 pad[6];u16 script;u8 formatter[24];u8 lines[1024];void *source,*text;int handles[3];void *map,*palette;struct Resource resource;u8 transition[96];};
void *sub_0821A520(int,int);void sub_082196C4(void*,void*);int Script_SeekToKeyword(int);int Script_GetPc(void);int Script_ParseStringRef(int);void *Text_LookupString(int);void *sub_08227E90(void);int sub_0821ABA8(int,int);void sub_08044E6C(void*);void sub_08044DD0(void*,void*,void*,int);void sub_080111EC(void*,int);int sub_08011264(void*);void sub_08011350(void*);
#define MessageMenu_Initialize sub_0801140C
int MessageMenu_Initialize(struct Menu*m){struct Resource*r=sub_0821A520(0xCB05,0xE2AB);if(!r)return -1;m->resource=*r;sub_082196C4(&m->resource,r);m->timer=0;if(!Script_SeekToKeyword('c'))return -1;m->source=Text_LookupString(Script_ParseStringRef(Script_GetPc()));if(!Script_SeekToKeyword('d'))return -1;m->text=sub_08227E90();if(!m->text)return -1;m->script=sub_0821ABA8('e',0);sub_08044E6C(m->formatter);sub_08044DD0(m->lines,m->formatter,m->source,6);sub_080111EC(m,12);if(sub_08011264(m)!=0)return -1;sub_08011350(m);return 0;}
