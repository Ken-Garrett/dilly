
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "jszl.h"

/*
** The size of the atom table array stored in the json object
** The atom table is an array of pointers to atom structures
** This value should be a prime number to reduce collisions and its best to
** use a prime number that is closest to (but less than) a power of 2
*/
#define ATOM_TABLE_SIZE 127 

/* 
** The max number of namespaces (arrays/objects) that can be nested
** inside of a namespace. This is the size of a array in the json object
*/
#define MAX_NS_LEVEL 16 

/*
** The max length enforced for object keys. NOT IMPLEMENTED
*/
#define MAX_KEY_LENGTH 32

/*
** The max length enforced for string values. NOT IMPLEMENTED
*/
#define MAX_STRING_LENGTH 4096 


#define MAX_VALUE_LIMIT 65536

/*
** max number of errors to accumulate in a buffer. NOT IMPLEMENTED
*/
#define ERROR_STACK_SIZE 16


#define UTF8_CHAR_LEN(byte) (((0xE5000000 >> ((byte >> 3) & 0x1E)) & 3) +1)
#define IS_UTF8_CONT(byte) ((byte >> 6) & 2)
#define CHAR_UTF8_LEN(dword)

#if defined(_WIN32) || defined (_WIN64)
#include <windows.h>
#include "os\windows.c"
#elif defined(_KERNEL_MODE)
#include "os\winkernel.c"
#elif defined(__linux__)
#include "os/linux.c"
#elif defined(unix) || defined(__unix) || defined(__unix__)
#include "os/unix.c"
#elif defined(__APPLE__) || defined(__MACH__)
#include "os/apple.c"
#elif defined(__FreeBSD__)
#include "os/freebsd.c"
#elif defined(__ANDROID__)
#include "os/android.c"
#endif

/*
** Value structure used to store value info
*/
struct jszlnode{
  short type;
  union{
    unsigned short count;
    unsigned short length;
  };
  struct jszlnode *next;
  struct atom *name;

  union{
    void *data;
    unsigned long value;
    struct jszlnode *child;
    char *string;
    int boolean;
    unsigned unum;
    signed snum;
    unsigned hash;
  };
};

/*
** Atom structure used for hashing strings
*/
struct atom {
  struct atom *next;
  const void *data;
  unsigned long hash;
  //potentially use this field to set flags. Ex: Unicode
  unsigned short refcount;
  unsigned length;
};

/*
** TYPE DECLARATIONS
*/
struct jszlcontext {
  struct jszlvtable *vt;

//struct json_node *schema;
  struct jszlnode *RootNS;
  struct jszlnode *CurrentNS;

  char *buffer;
  unsigned int bufsize;

  struct jszlnode  node_pool[512];
  unsigned int node_pool_size;

  struct atom  atom_pool[512];
  unsigned int atom_pool_size;

  const char *error_file;
  unsigned int error_line;

  void *databuf; // field where all data is copied
  unsigned long databufsize;

  jszle OpError;
  void *UserContext;
  unsigned int ThreadId;

  char encoding;
  //unsigned int ProcessorNumber;
};

/*
** Parser state is a disposable piece of memory that should ideally only be created
** on the stack right before a parse for the highest memory and execution efficiency
*/
struct jszlparser {
  JSON_ERROR_HANDLER log_error;

  struct jszlnode *root_namespace;
  struct jszlnode *current_namespace;

  struct jszlnode *node_pool;
  unsigned int node_pool_size;
  unsigned int pool_idx;

  struct atom *atom_pool;
  unsigned int atom_pool_size;
  unsigned int atom_idx;

  struct {
    struct jszlnode *namespace; 
  } ns_stack[MAX_NS_LEVEL];
  char stack_idx;

  struct {
    jszle e;
    const char *msg;
    unsigned int line;
    unsigned int offset; 
  } err_stack[ERROR_STACK_SIZE];

  unsigned line; //current line 
  unsigned lnofs; //current line offset
  const char *loc; //current location
  struct atom *curkey;
  struct jszlnode * prevnode;
  char Phase;

  unsigned int value_pool_idx;
  unsigned int atom_pool_idx;

  //struct node **node_pool;

  union {
    struct json_reader {int NOT_USED;} reader; 
    struct json_writer {int NOT_USED;} writer;
  };

  int (*key_handler)(struct jszlparser *);

};

const char *g_errmsg[MAX_JSZLERR] = {
  "The document root is missing"
};

//#define _WIN32

#include "..\core.h"


#include "hash.c"
#include "shared.c"


/*
** GLOBAL VARIABLES
*/

struct jszlvtable g_vtable;
const unsigned char g_max_handle = 16;

jszle g_operror;

struct jszlcontext g_thread[32];
unsigned long global_seed = 5381;
unsigned long handleIndex;
// Atom Table
struct atom * g_AtomTable[ATOM_TABLE_SIZE];

#define GET_VALUE_TYPE(value) (0x07 & value.type)
#define GET_VALUE_SUBTYPE(value) (0x1F & (value.type >> 3)) 

// TODO add function processing to endpoint parsing
// TODO add wildcard processing to endpoint parsing
// TODO use the term 'serialize' and 'deserialize' when writing and reading json objects
// TODO change terminology path to endpoint (ex: char *path -> char *endpoint)
// TODO add JSON schema functionality
// TODO EVENTUALLY (sometime in distant future) add support for custom schemas
// FIXED TODO atom table access should go through a local pointer
//     this is to allow for flexible atom table memory (global vs. private)
// TODO add reference counting to atoms to allow deletions

/* TODO
** Fix issue where error reporting innacurate offset when looking for comma
** This issue is caused because white space is being trimmed before
** the error is detected
*/


// define keywords in code

#define IS_ARRAY(ns) (GET_VALUE_TYPE(ns) == TYPE_ARRAY)
#define IS_OBJECT(ns) (GET_VALUE_TYPE(ns) == TYPE_OBJECT)


#if !defined(MAX_NS_LEVEL) ||  MAX_NS_LEVEL > 256
#error Error: Must define MAX_NS_LEVEL between 0 - 256
#endif

#if !defined(MAX_STRING_LENGTH) || MAX_STRING_LENGTH > 65536
#error Error: Must define MAX_STRING_LENGTH between 0 - 65536
#endif

#if !defined(MAX_KEY_LENGTH)  || MAX_KEY_LENGTH > 256
#error Error: Must define MAX_KEY_LENGTH between 0 - 256
#endif

//#define ESCAPE '?' '\'

/*
** This structure represents the life of the parsed JSON document
*/


union type_buffer{
    unsigned ipv4;
    union ipv6 ipv6;
};

typedef void (*key_handler_cb)(
  struct json_object *obj
 ,unsigned hash
 ,unsigned len
);

#include "atom.c"
#include "utf8.c"
#include "parser.c"

#define NODE_BY_IDX(pnode, idx)\
    pnode = pnode->value;\
    if(idx > (node).count) { printf("Error: Index too high\n"); }\
    while(idx--) pnode = pnode->next;

static struct jszlnode *get_value_byidx
(struct jszlnode *value, unsigned idx)
{
    if(idx >= value->count) return 0; //err: Index too high
    if( !(value = value->child) ) return 0; //err: no values
    while(idx--){
        assert(value);
        value = value->next;
    }
    return value;
}

static struct jszlnode *get_value_byname(
  struct jszlcontext *handle
 ,struct jszlnode *value
 ,unsigned long hash
 ,unsigned len
){
struct atom *atom = 0;
void * l_atomTable = g_AtomTable;
    //hash = djb2(name, len);
    atom = atom_find(l_atomTable, ATOM_TABLE_SIZE, hash, len);
    assert(atom);
    if(!atom) return 0;
    for(value = value->child; value; value = value->next)
        if(value->name == atom) break;
    return value; 
}

// $ = absolute root
// # = relative root
int resolve_root(
  struct jszlcontext *pctx
 ,struct jszlnode **ppnode
 ,const symbol
){
    if(!pctx || !ppnode || !symbol) return JSZLE_BAD_PARAM;
    if(!pctx->RootNS) return JSZLE_NO_ROOT;

    switch(symbol){
    case '$':
        *ppnode = pctx->RootNS;
        break;
    case '#':
        *ppnode = pctx->CurrentNS;
        break;
    default:
	return JSZLE_BAD_PATH;
    }

    return JSZLE_NONE; 
}

int get_node_byname(
  struct jszlnode *pnode
 ,struct jszlnode **ppnode
 ,const char *name
){
    unsigned n;
    unsigned long hash;
    struct atom *atom = 0;
    void * l_atomTable = g_AtomTable;

    n = is_valid_key(name, global_seed, &hash);
    if(!n)    return 0;
    atom = atom_find(l_atomTable, ATOM_TABLE_SIZE, hash, n);
    if(!atom) return 0;

    for(pnode = pnode->child; pnode; pnode = pnode->next)
        if(pnode->name == atom) break;
    if(!pnode) return 0;
    *ppnode = pnode;
    return n;
}

int query_engine(
  struct jszlnode *pnode
 ,struct jszlnode **ppnode
 ,const char *path
){
const char *loc;
unsigned n, type, subtype;
unsigned short idx;


loc = path;

BEGIN_LOOP:

    if(*loc == '\0'){
        *ppnode =  pnode;
        return JSZLE_NONE;
    }
    else if(*loc == '['){ 
        if(!IS_ARRAY((*pnode)) && !IS_OBJECT((*pnode)))
        {
            printf("Error: Must be structural node\n");
            *ppnode = 0;
            return JSON_ERROR_MUST_BE_ARRAY_OR_OBJECT;
        }
        loc++;
        n = is_valid_number(loc, &type, &subtype);
	idx = atouint(loc, n);
        pnode = get_value_byidx(pnode, idx);
	loc += n;
	if(!pnode) return JSZLE_KEY_UNDEF;
        if(*loc++ != ']'){
            printf("Error: Syntax\n");	
            *ppnode = 0;
            return JSON_ERROR_SYNTAX;
	}
    }
    else if(*loc == '.' || *loc == '/'){ //object
        loc++;
        if(!IS_OBJECT((*pnode))){
            printf("Error: Must be an object\n");
            *ppnode = 0;
	    return JSON_ERROR_TYPE_MISMATCH;
        }
        n = get_node_byname(pnode, &pnode, loc);
        if(!n) return JSZLE_KEY_UNDEF;
	loc += n;
    }
    else { //err
    //should check if 'loc' is an object and a valid key name
        if(IS_ARRAY( (*pnode) ) || IS_OBJECT((*pnode)))
        {
            printf("Error: No namespaces\n");
            *ppnode = 0;
            return JSON_ERROR_SYNTAX;
        }
    }

    //do an atom find on the key
    //ns = json_find_ns(node);
    goto BEGIN_LOOP;
}

/*
int _json_query_engine(
  struct jszlcontext *ctx
 ,const char *path
 ,struct jszlnode ** ppval
){
const char *loc;
unsigned n, type, subtype;
unsigned long hash;
struct atom *atom = 0;
unsigned short idx;

void * l_atomTable = g_AtomTable;

loc = path;

struct jszlnode *pval = 0;

    if(*loc == '$' || *loc == '#'){
        pval = ROOT_NS;
    }
    else {
        pval = CURRENT_NS; 
    }
    loc++;
BEGIN_LOOP:


    if(*loc == '\0'){
        *ppval =  pval;
        return JSZLE_NONE;
    }
    else if(*loc == '['){ 
        if(!IS_ARRAY((*pval)) && !IS_OBJECT((*pval)))
        {
            printf("Error: Must be structural node\n");
            *ppval = 0;
            return JSON_ERROR_MUST_BE_ARRAY_OR_OBJECT;
        }
        loc++;
        n = is_valid_number(loc, &type, &subtype);
	idx = atouint(loc, n);
        pval = get_value_byidx(pval, idx);
	loc += n;
	if(!pval) return JSZLE_KEY_UNDEF;
        if(*loc++ != ']'){
            printf("Error: Syntax\n");	
            *ppval = 0;
            return JSON_ERROR_SYNTAX;
	}
    }
    else if(*loc == '.' || *loc == '/'){ //object
        loc++;
        if(!IS_OBJECT((*pval))){
            printf("Error: Must be an object\n");
            *ppval = 0;
	    return JSON_ERROR_TYPE_MISMATCH;
        }
        n = is_valid_key(loc, global_seed, &hash);
        assert(n);
        //get the node by name
	atom = atom_find(l_atomTable, ATOM_TABLE_SIZE, hash, n); 
	if(!atom) return JSZLE_KEY_UNDEF;

	for(pval = pval->child; pval; pval = pval->next)
            if(pval->name == atom) break;
        if(!pval) return JSZLE_KEY_UNDEF;
	loc += n;
    }
    else { //err
        if(IS_ARRAY( (*pval) ) || IS_OBJECT((*pval)))
        {
            printf("Error: No namespaces\n");
            *ppval = 0;
            return JSON_ERROR_SYNTAX;
        }
    }

    //do an atom find on the key
    //ns = json_find_ns(node);
    goto BEGIN_LOOP;
}
*/

static int copy_json_string(void *dest, void *src, unsigned int len)
{
char *_dest = dest, *_src = src;
int count;
int buf;
    while(len--) {
        if(*_src == '\\'){
            count = get_escaped_char(++_src, &buf);
            len -= count;
            _src += count;
            *_dest++ = buf;
        }
        else *_dest++ = *_src++;
    }
    return _dest - dest;
}


//128
//2048    0x7FF
//65536   0xFFFF
//2097152 0x1FFFFF


void utf8_encode_char(unsigned long c)
{
    if(c < 128); //1 byte
    else if(c < 2048); //2 byte
    else if(c < 65536); //3 byte
}

static int uft8_string(char *str, char *buf)
{
    unsigned int c;
    c = (0xF0 | ((char)(c >> 18) & 0x07)); //0000 0111
    c = (0x80 | ((char)(c >> 12) & 0x0F)); //0000 1111
    c = (0xC0 | ((char)(c >> 6) & 0x1F)); //0001 1111 
    c = (0x80 | (char)(c & 0x3F));         //0011 1111
}

/*
** Validate the descriptor table of an object against a binary json object value
** Descriptor table stack is the stack that holds the nested descriptors of objects
*/

/*
#define enter_public_api(ptr)\
ptr = jszl_handle_to_resource(handle);\
ptr->error_file = __file__;\
ptr->error_line = __line__
*/


static void *get_context(jszlHandle handle)
{
    if(handle > g_max_handle){
        return 0; 
    }
    return &g_thread[handle-1]; //decrement by 1
}

//0x80
//0xF0

void utf8()
{

}



#include "jszlapi.c"

