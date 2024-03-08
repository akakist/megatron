#include <stdio.h>
#include <stdlib.h>
int main()
{
    for(int i=0; i<100; i++)
    {
        printf("%d, ",arc4random()%300+30);
    }

}
