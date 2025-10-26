#include <stdio.h>
typedef int Key;
typedef struct { Key key; char information[10]; } Item;
#define key(A) (A.key)
int main(void)
{
  Item item = {5, "ABCDEFG"};
  printf("Key of the item is %d\n", key(item));
  return 0;
}
