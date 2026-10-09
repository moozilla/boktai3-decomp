#include "global.h"
s32 Div(s32,s32);s32 sub_08228548(s32,s32);
s32 sub_082285C4(s32 year,s32 month,s32 day,s32 otherYear,s32 otherMonth,s32 otherDay)
{
 s32 a,b,c,base,total,otherTotal,i;
 a=Div(year,4);b=Div(year,100);c=Div(year,400);
 base=365*year+a-b;total=base+c;
 i=1;
 while(i<month) {total+=sub_08228548(year,i);i++;}
 total+=day;
 a=Div(otherYear,4);b=Div(otherYear,100);c=Div(otherYear,400);
 base=365*otherYear+a-b;otherTotal=base+c;
 i=1;
 while(i<otherMonth) {otherTotal+=sub_08228548(otherYear,i);i++;}
 otherTotal+=otherDay;
 return total-otherTotal;
}
