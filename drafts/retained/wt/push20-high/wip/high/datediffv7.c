#include "global.h"
s32 Div(s32,s32);s32 sub_08228548(s32,s32);
s32 sub_082285C4(s32 year,s32 month,s32 day,s32 otherYear,s32 otherMonth,s32 otherDay)
{
 s32 a,b,c,total,otherTotal,i;
 a=Div(year,4);b=Div(year,100);
 total=(365*year+a-b)+Div(year,400);
 i=1;
 while(i<month) {total+=sub_08228548(year,i);i++;}
 total+=day;
 a=Div(otherYear,4);b=Div(otherYear,100);
 otherTotal=(365*otherYear+a-b)+Div(otherYear,400);
 i=1;
 while(i<otherMonth) {otherTotal+=sub_08228548(otherYear,i);i++;}
 otherTotal+=otherDay;
 return total-otherTotal;
}
