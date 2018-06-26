#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define g_chunksize (2*1024*1024)
int main()
{
    std::string localfile="1";
    FILE *f=fopen("kall","wb");
#ifndef _WIN32
    struct stat64 st;
    if(stat64(localfile.c_str(),&st))
#else
    struct _stati64 st;
    if(_stati64(localfile.c_str(),&st))
#endif
    {
        printf("cannot find file %s\n",localfile.c_str());
        return 0;
    }

    FILE* fd=fopen(localfile.c_str(),"rb");
    printf("\nfile open %d\n",fd);
    long long readen=0;
    int chunk=1;
    while(readen<st.st_size)
    {
        printf("\nreaden %lld size %lld",readen,st.st_size);

        long sz=g_chunksize;
        if(st.st_size-readen<g_chunksize)
        {
            sz=st.st_size-readen;
        }
        printf("\nsz %ld\n",sz);
        std::string buffer;
        char buf[1024*8];
        while(buffer.size()<sz)
        {
            int len=sz-buffer.size()>sizeof(buf)?sizeof(buf):sz-buffer.size();
            //printf("\nlen %d",len);
            int r=fread(buf,1,len,fd);
            if(r==-1)
            {
                printf("\nread: %s\n",strerror(errno));
                return 0;
            }
//          	printf("\nread %d",r) ;
            buffer.append(std::string(buf,r));
            readen+=r;

        }
        printf("\nbuffersize %d\n",buffer.size());
        fwrite(buffer.data(),1,buffer.size(),f);
        buffer="";
        printf("\nsend chunk %d\n",chunk);
        chunk++;

    }
    fclose(f);
    fclose(fd);


}