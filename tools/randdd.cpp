#include <stdio.h>
#include <stdlib.h>
#include <memory>
struct a{
    int z;

};
void f(std::unique_ptr<a> &&z)
{
    auto b=std::move(z);
}
void c()
{
    auto p=std::make_unique<a>();
    f(std::move(p));
    if(p)
    {
        printf("p\n");
    }
    else printf("!p\n");
}



int main()
{
c();
    for(int i=0; i<100; i++)
    {
        printf("%d, ",arc4random()%300+30);
    }

}
