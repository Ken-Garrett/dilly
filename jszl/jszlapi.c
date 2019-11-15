
#define DECLPARAMS const char *__file__,unsigned __line__,const char *__function__

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define WORD_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN
#define DWORD_TO_BINARY_PATTERN WORD_TO_BINARY_PATTERN " " WORD_TO_BINARY_PATTERN

#define BYTE_TO_BINARY(byte)\
  (byte & 0x80 ? '1' : '0'),\
  (byte & 0x40 ? '1' : '0'),\
  (byte & 0x20 ? '1' : '0'),\
  (byte & 0x10 ? '1' : '0'),\
  (byte & 0x08 ? '1' : '0'),\
  (byte & 0x04 ? '1' : '0'),\
  (byte & 0x02 ? '1' : '0'),\
  (byte & 0x01 ? '1' : '0')

#define WORD_TO_BINARY(word)\
  (word & 0x8000 ? '1' : '0'),\
  (word & 0x4000 ? '1' : '0'),\
  (word & 0x2000 ? '1' : '0'),\
  (word & 0x1000 ? '1' : '0'),\
  (word & 0x0800 ? '1' : '0'),\
  (word & 0x0400 ? '1' : '0'),\
  (word & 0x0200 ? '1' : '0'),\
  (word & 0x0100 ? '1' : '0'),\
  BYTE_TO_BINARY(word)

#define DWORD_TO_BINARY(dword)\
  (dword & 0x80000000 ? '1' : '0'),\
  (dword & 0x40000000 ? '1' : '0'),\
  (dword & 0x20000000 ? '1' : '0'),\
  (dword & 0x10000000 ? '1' : '0'),\
  (dword & 0x08000000 ? '1' : '0'),\
  (dword & 0x04000000 ? '1' : '0'),\
  (dword & 0x02000000 ? '1' : '0'),\
  (dword & 0x01000000 ? '1' : '0'),\
  (dword & 0x00800000 ? '1' : '0'),\
  (dword & 0x00400000 ? '1' : '0'),\
  (dword & 0x00200000 ? '1' : '0'),\
  (dword & 0x00100000 ? '1' : '0'),\
  (dword & 0x00080000 ? '1' : '0'),\
  (dword & 0x00040000 ? '1' : '0'),\
  (dword & 0x00020000 ? '1' : '0'),\
  (dword & 0x00010000 ? '1' : '0'),\
  WORD_TO_BINARY(dword)

/*
** PUBLIC INTERFACES
**
*/


/*
** checks membind to see if the global environment has already been
** initialized
*/
void __jszlGlobalInit(DECLPARAMS
                     ,struct jszlvtable *vt
                     ,unsigned long options)
{

    if(g_vtable.memalloc) return;

    if(vt->memalloc) g_vtable.memalloc = vt->memalloc;
    else g_vtable.memalloc = memalloc;

    if(vt->errorlog) g_vtable.errorlog = vt->errorlog;
    else g_vtable.errorlog = deferrorlog;
}

jszle __jszlSetOpt(DECLPARAMS
                  ,jszlHandle handle
                  ,jszlopt opt, ...)
{
    struct jszlcontext *pctx;
    va_list args;
    
    pctx = get_context(handle); 

    va_start(args, opt);
    switch(opt){

    case JSZLOPT_ENCODE_FORMAT:
    {
        break; 
    }

    case JSZLOPT_READ_BUFFER:
    { 
        unsigned int bufsize;
        char *buffer;
	buffer = va_arg(args, char *);
        bufsize = va_arg(args, unsigned int);
	pctx->buffer = buffer;
	pctx->bufsize = bufsize;
	break;
    }

    } //end switch
    va_end(args);

    return JSZLE_NONE;
}


jszle __jszlGetValue(DECLPARAMS
             ,jszlHandle handle
            ,unsigned int *type
             ,const char *path)
{
    struct jszlcontext *pctx;
    struct jszlnode *pnode;

    pctx = get_context(handle);

    resolve_root(pctx, &pnode, *path);
    if(JSZLE_NONE != query_engine(pnode, &pnode, path+1)){
        return 0;
    }

    //if(IS_ARRAY(*pnode) && 
    
    return JSZLE_NONE;
}

jszle __jszlSetEncode(DECLPARAMS
                    ,jszlHandle handle
                    ,enum jszlencode encode)
{
    struct jszlcontext *pctx;
    pctx = get_context(handle);

    pctx->encoding = encode;

    return JSZLE_NONE;
}


jszlHandle __jszlThreadInit(DECLPARAMS)
/*
** function notes
*/

{
    if(!g_vtable.memalloc) return 0; //failure to init global

    struct jszlcontext *pctx;
    int curcpu;
    
    curcpu = get_current_cpu();

    g_thread[curcpu].ThreadId = get_thread_id();

    //assign the handle to a valid resource

    //initialize the multithreaded environment 
        init_multithreaded_environment();

    return (jszlHandle)curcpu+1; //+1 for 0 array index
}

jszle __jszlParseString(DECLPARAMS
                       ,jszlHandle handle
                       ,const char *json)
{
    return JSZLE_NONE;
}

jszle __jszlParseLocalFile(DECLPARAMS
                          ,jszlHandle handle
                          ,const char *path)
{
struct jszlfile fs = {0};
//struct jszlfile *pfs = &fs;
const char *json;

struct jszlcontext *ctx;
struct jszlparserstate state = {0};
unsigned rslt;

    ctx = get_context(handle);
    if(!ctx){
        printf("Error: No context found\n"); 
        return 0;
    }

    openFile(&fs, path, true);
    json = mapFile(&fs);

    printf("%.*s\n", 300, json);

    //invoke the parsing engine on the mapped pointer
    rslt = json_parser_engine(&state, ctx, json);
    if(rslt != JSZLE_NONE){
        printf("Error(%u): Failed to parse JSON file\n", rslt); 
        return 0;
    }
    printf("Parse Success!\n");

    char buf[8], *s = buf;

    //printf("UTF8 0x%x\n", utf8_encode(&buf, 1));
    s = utf8_encode(s, WHITE_CHESS_KING);
    s = utf8_encode(s, BLACK_SPADE_SUIT);
    printf("UC %s\n", buf);

    //unmapFile(&fs);
    return JSZLE_NONE;
}


jszle __jszl_load(DECLPARAMS
   ,jszlHandle handle
   ,const char *filename)
/*
**
*/
{
struct jszlfile file;
jszle err;
struct jszlcontext *pctx;
struct jszlparserstate state;

    pctx = get_context(handle);
    pctx->error_file = __file__;
    pctx->error_line = __line__;

    // open the file and load it into memory
    openFile(&file, filename, 0);
    err = json_parser_engine(&state, pctx, filename);

    return JSZLE_NONE;
}

//jszl_set_vtable(0, 0, 0, 0, 0, 0, 0, 0);


jszle __json_read(DECLPARAMS
 ,struct jszlparserstate *pstate
 ,struct jszlcontext *handle 
 ,const char *str
){
int rslt;
    rslt = json_parser_engine(pstate, handle, str);
    if(rslt != JSZLE_NONE) return rslt;
    return JSZLE_NONE;
}

/**
* Set the namespace scope of the current document
* @param handle handle to a JSizzle context
* @param path JSON path to a valid object or array
*/
jszlopresult __jszlSetScope(DECLPARAMS
                           ,jszlHandle handle
                           ,const char *path)
{
    struct jszlcontext *pctx;
    struct jszlnode *pnode;

    pctx = get_context(handle);
    if(!pctx){
        g_operror = JSZLE_BAD_HANDLE;
        return 0;
    }

    if(!pctx->RootNS) {//error: no document loaded
        g_operror = JSZLE_NO_ROOT;
        return 0;
    }

    //search the node tree and set the node returned
    resolve_root(pctx, &pnode, *path+1);
    if(JSZLE_NONE != query_engine(pnode, &pnode, path+1)){
        return 0;
    }

    if(!IS_ARRAY( (*pnode) ) && !IS_OBJECT( (*pnode) ) ){
        return 0; 
    }

    pctx->CurrentNS = pnode;
    return 1;    
}

jszlopresult __jszlIsRoot(DECLPARAMS
                         ,jszlHandle handle
                         ,const char *path)
{
    struct jszlcontext *pctx;
    struct jszlnode *pnode;

    pctx = get_context(handle);
    if(!pctx){
        return 0; 
    }

    if(!pctx->RootNS){
        g_operror = JSZLE_NO_ROOT; 
        return 0;
    }

    resolve_root(pctx, &pnode, *path);
    if(JSZLE_NONE != query_engine(pnode, &pnode, path+1)){
        return 0; 
    }

    return pctx->CurrentNS == pnode;
}


/*
** Set the user context pointer to be passed to the user defined virtual table
*/
//jszle __jszlSetUserContext
jszle __jszlSetUserContext(DECLPARAMS
                          ,jszlHandle handle
                          ,void *userctx)
{
    struct jszlcontext *ctx;

    ctx = get_context(handle);

    ctx->UserContext = userctx;
    return JSZLE_NONE;
}

//jszle __jszlGetErrorMessage
jszle __jszl_geterror(DECLPARAMS
                       ,jszlHandle handle
                       ,char **errmsg)
{
    struct jszlcontext *ctx;
    ctx = get_context(handle);

    if(g_operror == JSZLE_NONE) *errmsg = 0;
    else *errmsg = g_errmsg[0];

    return g_operror;
}



/*
**
*/
jszle _json_query_get_type(DECLPARAMS
 ,struct jszlcontext *pctx
 ,const char *path
 ,short *ptype
){
int msg;
struct jszlnode *pval;

    msg = query_engine(pctx, path, &pval);
    *ptype = pval->type;
    return JSZLE_NONE; 
}

struct descriptor_table {
    struct field_instance * inst;
    int idx;
    int inst_count;
};

jszle __jszlIterate(DECLPARAMS
                   ,jszlHandle handle
                   ,int (*callback)(void *, int)
                   ,void *passback
                   ,const char *path)
{
    struct jszlcontext *pctx;
    struct jszlnode *pnode;

    pctx = get_context(handle);

    resolve_root(pctx, &pnode, *path);
    query_engine(pnode, &pnode, path+1);

    if(GET_VALUE_TYPE((*pnode)) != TYPE_ARRAY) return JSZLE_TYPE_MISMATCH;

    //for each node, use callback to process record
    return 1;
}

jszle __jszlDeserializeObject(DECLPARAMS
  ,jszlHandle handle
  ,void *buffer
  ,struct field_desc table[]
  ,int count
  ,const char *path
){
  struct jszlcontext *pctx;
  int msg;
  struct jszlnode *rootnode;
  struct jszlnode *pnode;
  struct field_desc *curdesc;
  char jsonPath[64];
  int size;
  int length;

  int stack_idx = 0;
  struct descriptor_table stack[8] = {0};
  struct descriptor_table *stack_item;

  struct field_desc *fd;
  int i = 0;

  char *field;

    if(!handle || !path || !count || !table || !buffer) return JSZLE_BAD_PARAM;

    pctx = get_context(handle);

    resolve_root(pctx, &rootnode, *path);
    msg = query_engine(rootnode, &rootnode, path+1);
    if(msg != JSZLE_NONE) return msg;
    if(GET_VALUE_TYPE((*rootnode)) != TYPE_OBJECT) return JSZLE_TYPE_MISMATCH;

RESTART:


    // 1. Look for field name in json object, if a field name is not
    // in the object move to next field as fields aren't required to exist

    length = get_node_byname(rootnode, &pnode, table[i].name);
    if(!length) goto NEXT_DESCRIPTOR;

    field = (char*)buffer + table[i].offset;

    switch(table[i].type){

      case TYPE_STRING:
      case JSZLTYPE_STRING_UTF8:
      case JSZLTYPE_STRING_UTF16:

	// 2. Check the size of the field against the size of the value returned
	// if the size of the returned data is too large, bailout!
        if(pnode->length > table[i].size){
            return JSZLE_NO_MEMORY;
        }

	// 3. copy data and set null term
        size = copy_json_string((char*)field, pnode->data, pnode->length);

	// 4. increment to next instance
	/*
	stack_item->idx++;
	if(stack_item->idx > stack_item->inst_count) {
            // gone through table, decrement stack
            stack_idx--;
            //goto ENTER_INSTANCE;
        }
	*/
	//goto ENTER_INSTANCE; 
	break;

      case JSZLTYPE_NUMBER_INT8:
      case JSZLTYPE_NUMBER_UINT8:

      case JSZLTYPE_NUMBER_INT16:
      case JSZLTYPE_NUMBER_UINT16:

      case TYPE_NUMBER:
      case JSZLTYPE_NUMBER_INT32:
      case JSZLTYPE_NUMBER_UINT32:
          *(int*)field = pnode->value;
          break;

      case JSZLTYPE_NUMBER_INT64:
      case JSZLTYPE_NUMBER_UINT64:

      case TYPE_OBJECT:
          break;
          
      case TYPE_ARRAY:
	// go into field instance and get the pointer to nested object instance
        stack_idx++;

      default: ; // unsupported type, bailout!

    }

NEXT_DESCRIPTOR:
    if(++i < count) goto RESTART;

    return 0;
}


jszle __jszlValueExists(DECLPARAMS
  ,jszlHandle handle
  ,const char *path)
{
    return JSZLE_NONE;
}

/**
* Get a string representation of a given error code
* @param errcode an error code 
* @param errmsg  the string message
*
*/
jszle __jszlGetErrMsg(DECLPARAMS
                     ,jszle errcode 
                     ,char **errmsg)
{
    return JSZLE_NONE;
}

jszle jszlGetArrayCount(DECLPARAMS
                       ,jszlHandle handle
                       ,short *count
                       ,const char *path)
{
int msg;
struct jszlnode *pnode;
struct jszlcontext *pctx;

    pctx = get_context(handle);
    resolve_root(pctx, &pnode, *path);
    msg = query_engine(pnode, &pnode, path+1);
    if(msg != JSZLE_NONE) return msg;
    if( GET_VALUE_TYPE((*pnode)) != TYPE_ARRAY) return JSON_ERROR_TYPE_MISMATCH; 
    *count = pnode->count;
    return JSZLE_NONE;
}

#undef DECLPARAMS
