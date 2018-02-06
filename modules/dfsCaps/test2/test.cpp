#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int ac, char* argv[])
{
    //62.68.135.40
//    in_addr_t a=inet_addr(/*argv[1]*/ "62.68.135.40");
    in_addr_t a=inet_addr(argv[1]);
//     40 135 68 62


//    in_addr_t a=inet_addr("40.135.68.62");
    unsigned char *p=(unsigned char *)&a;
    in_addr_t out;
    unsigned char *pout=(unsigned char *)&out;
    for(int i=0; i<4; i++)
    {
        pout[i]=p[3-i];
    }
    printf("%d\n %d %d %d %d\n",out,pout[0],pout[1],pout[2],pout[3]);
    printf("ntohl %d\n",ntohl(a));
}