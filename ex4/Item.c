#define _ITEM_C_
#include <stdio.h>
#include <stdlib.h>
#include "Item.h"

Item NULLitem = {-1, "NULL"};

Key ITEMrand(void) 
{ return rand()%100000; }
int ITEMscan(Key *x) 
{ return scanf("%d", x); }  
void ITEMshow(Item x) 
{ printf("%3d ", key(x)); }  
