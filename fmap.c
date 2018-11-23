//#include "fmap.h"

void* create_filemap(const char *file, HANDLE *pfhdl, HANDLE *pfmhdl);
void free_filemap(HANDLE hfile, HANDLE hmap, char *fmap);

unsigned readline(const char *file);
//#line 3 "h:\\dev\\code\\lib-dilly\\fmap.c"

void* create_filemap(const char *file, HANDLE *pfhdl, HANDLE *pfmhdl)
{
char *mapped_view;

  *pfhdl = CreateFileA
  (file,
  (0x80000000L)|(0x40000000L),
  0, 
  0, 
  3,
  0x00000080,
  0); 
  (void)( (!!(*pfhdl != ((HANDLE)(LONG_PTR)-1))) || (_wassert(L"*pfhdl != INVALID_HANDLE_VALUE", L"h:\\dev\\code\\lib-dilly\\fmap.c", (unsigned)(16)), 0) );

  *pfmhdl = CreateFileMappingA
  (*pfhdl,
  0, 
  0x04, 
  0, 
  0,
  0); 
  (void)( (!!(*pfmhdl != 0)) || (_wassert(L"*pfmhdl != 0", L"h:\\dev\\code\\lib-dilly\\fmap.c", (unsigned)(25)), 0) );

  mapped_view = MapViewOfFile
  (*pfmhdl,
  0x0002, 
  0, 0, 
  0); 
  (void)( (!!(mapped_view != 0)) || (_wassert(L"mapped_view != 0", L"h:\\dev\\code\\lib-dilly\\fmap.c", (unsigned)(32)), 0) );

  return mapped_view;
}

void free_filemap(HANDLE hfile, HANDLE hmap, char *fmap)
{
    (void)( (!!(UnmapViewOfFile(fmap))) || (_wassert(L"UnmapViewOfFile(fmap)", L"h:\\dev\\code\\lib-dilly\\fmap.c", (unsigned)(39)), 0) );
    (void)( (!!(CloseHandle(hmap))) || (_wassert(L"CloseHandle(hmap)", L"h:\\dev\\code\\lib-dilly\\fmap.c", (unsigned)(40)), 0) );
    (void)( (!!(CloseHandle(hfile))) || (_wassert(L"CloseHandle(hfile)", L"h:\\dev\\code\\lib-dilly\\fmap.c", (unsigned)(41)), 0) );
}

unsigned readline(const char *file) 
{
const char *tmp = file;

    while(*tmp && (*tmp++ != '\r')); 
    if(*tmp == 0) return 0;
    return tmp - file + 1;
}

