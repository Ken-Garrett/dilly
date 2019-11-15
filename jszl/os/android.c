
static void *_default_membind(void *ctx, unsigned long size)
{
void *mem;

    mem = VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
    return mem;
}

static void _default_memfree(void *ctx, void *mem)
{
    VirtualFree(mem, 4096, MEM_DECOMMIT);
}

static void _default_errorlog(void *ctx, jszlerror err, const char *msg)
{

}

static void init_multithreaded_environment()
{
DWORD threadid;
DWORD processorid;

    threadid    = GetCurrentThreadId();
    processorid = GetCurrentProcessorNumber();

}

static void open_json_file(){}
