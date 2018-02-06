#include <string.h>
#include <stdio.h>
#include <string>
char* urlencode(char* src)
{
  int i;
    static char dest[10000];
  for(i = 0; i < strlen(src); i++) {
    char c = src[i];
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9') ||
        c == '-' || c == '_' || c == '.' || c == '~') {
      char t[2];
      t[0] = c; t[1] = '\0';
      strcat(dest, t);
    } else {
      char t[4];
      snprintf(t, sizeof(t), "%%%02x", c & 0xff);
      strcat(dest, t);
    }
  }
return dest;
}

int main(int argc, char * argv[])
{
    printf("\n%s\n",urlencode(argv[1]));
std::string s="";
}