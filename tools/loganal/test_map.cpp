#include <map>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <set>
int main()
{
    /* std::map<long,std::pair<long,long> > m;
     for(long i=0;i<4000000;i++)
     {
         m[i]=std::make_pair(i,i);
     }
     sleep(100);*/
    std::set<int> v;
    v.insert(1);
    v.insert(3);
    v.insert(4);
    v.insert(6);
    v.insert(8);
    v.insert(9);
    v.insert(10);
    v.insert(13);
    v.insert(151);
    v.insert(159);
    v.insert(251);
    v.insert(259);
    v.insert(351);
    v.insert(359);
    std::vector<int>::iterator i=lower_bound(v.begin(),v.end(),152);
    if(i!=v.end())
        printf("%d pos %ld\n",*i,i-v.begin());
}