#include <string.h>
#include <errno.h>
int main() { return strlen(strerror(errno)); }
