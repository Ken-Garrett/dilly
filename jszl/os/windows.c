
struct jszlfile {
  HANDLE hfile;
  HANDLE hfmap;
  void *baseAddress;
};

/*
** default memory handler 
*/
static void *memalloc(void *ctx, unsigned long size, int doalloc)
{
void *mem;

    if(doalloc) { //allocate memory
        mem = VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE); 
	return mem; 
    }
    mem = (void*)size;
    VirtualFree(mem, 0, MEM_RELEASE);
    return 0;
}


static void deferrorlog(void *ctx, jszle err, const char *msg)
{

}

static void init_multithreaded_environment()
{
DWORD threadid;
DWORD processorid;

    threadid    = GetCurrentThreadId();
    processorid = GetCurrentProcessorNumber();
}

static void createfile(const char *filename)
{}

static int openFile(struct jszlfile *pf, const char *path, jszlstropt opt)
{

    if(opt & JSZLSTROPT_ANSI){
        pf->hfile = CreateFileA(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,
                0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    }
    else {
        //hfile = CreateFileW(filename.strw, GENERIC_READ
    }
    if(INVALID_HANDLE_VALUE == pf->hfile){
        printf("Error: Could not open file\n");
        return 0;
    }
    return 1;
}

static void readfile(const char *filename)
{}

static void *mapFile(struct jszlfile *pf)
{
    pf->hfmap = CreateFileMappingA(pf->hfile, 0, PAGE_READWRITE, 0, 0, 0);
    if(!pf->hfmap){
        printf("Error(%u): Could create file mapping\n", GetLastError()); 
        return 0;
    }

    pf->baseAddress = MapViewOfFile(pf->hfmap, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
    if(!pf->baseAddress){
        printf("Error: Could not map file\n");
	return 0;
    }
    return pf->baseAddress;
}

static void unmapFile(struct jszlfile *pf)
{
    UnmapViewOfFile(pf->baseAddress);
    CloseHandle(pf->hfmap);
}

static void deleteFile(struct jszlfile *pf)
{}

static int get_processor_count()
{
    SYSTEM_INFO si;

    GetSystemInfo(&si); 
    return si.dwNumberOfProcessors;

    /*
    DWORD pa, sa;
    GetProcessAffinityMask(GetCurrentProcess(), &pa, &sa);
    */
}

static int get_current_cpu()
{
    return GetCurrentProcessorNumber();
}

static int get_thread_id()
{
    return GetCurrentThreadId();
}
