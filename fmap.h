#include <windows.h>

void* create_filemap(const char *file, HANDLE *pfhdl, HANDLE *pfmhdl);
void free_filemap(HANDLE hfile, HANDLE hmap, char *fmap);
unsigned readline(const char *file);
