
#define ROOT_NS (ctx->RootNS)
#define CURRENT_NS (ctx->CurrentNS)
#define PREVIOUS_VALUE (parser->prevnode)
#define CURRENT_KEY (parser->curkey)
#define STACK_INDEX (parser->stack_idx)
#define LINE_OFFSET
#define NAMESPACE_STACK (parser->ns_stack)
#define ATOM_TABLE (ctx->atom_table)
#define ERRORLOG_PROC (parser->log_error)
#define MEMALLOC_PROC (parser->alloc)

#define ERRORMSG_INVALID_PRIMITIVE "Invalid primitive value" 

enum {
    NO_PHASE,
    _ARRAY_PHASE_OPT_VALUE,
    _ARRAY_PHASE_REQ_VALUE,
    _ARRAY_PHASE_COMMA,
    _OBJECT_PHASE_OPT_KEY,
    _OBJECT_PHASE_REQ_KEY,
    _OBJECT_PHASE_COLON,
    _OBJECT_PHASE_VALUE,
    _OBJECT_PHASE_COMMA,
};




static inline struct jszlnode * new_value(
  struct jszlnode *node_pool
 ,int pool_idx
// ,struct jszlnode *current_namespace
 ,short valtype
 ,long data
 ,unsigned len
){
struct jszlnode * pnode;
    pnode = &node_pool[pool_idx];
    return pnode;
}


/*
** Macro to create a new node and log an error on failure
*/
#define NEW_VALUE(VALTYPE, DATA, LEN)\
do{\
PREVIOUS_VALUE = new_value(parser, ctx, current_namespace, VALTYPE, (long)DATA, LEN);\
if(!PREVIOUS_VALUE) { LOG_ERROR(JSZLE_NO_MEMORY, 0); }\
}while(0)


/*
** Macro to create a new root namespace and set it as the 
** current namespace. Push the new namespace onto the namespace stack
*/


/*
** Macro to create a new namespace and initialize. The current
** namespace now references the previous namespace. The namespace
** is pushed onto the stack an execution reenters the loop
*/



//goes to cleanup after 1 error
#define LOG_ERROR(e, msg)\
do{\
if(ERRORLOG_PROC && !ERRORLOG_PROC(e, msg, LINE, 0)) { goto ERROR_CLEANUP; }\
goto ERROR_CLEANUP;\
}while(0);

static long key_exists(struct jszlnode *value, struct atom *key)
{
    for(value = value->child; value; value = value->next)
        if(value->name == key) return 1;   
    return 0;
}

/*
** Skip whitespace
*/
static unsigned inline skip_ws(const char *loc, unsigned *line)
{
unsigned c;
const void *start = loc;
    do{

    c = *loc;
    if(c == ' ' || c == '\t')
        loc++;     
    else if(c == '\r'){
        if(loc[1] == '\n'){
            loc += 2;
            (*line)++;	    
            //offset++;
        }
        else printf("Invalid char\n");
    }
    else if(c == '\n'){
        (*line)++; 
        loc++;
        //offset++;
    }
    else break;

    }while(1);
    return loc - start;
}

struct value_data {
    int type;
    unsigned length;
    unsigned isNegative;
    unsigned numType;
    unsigned long hash;
};

int string_handler(struct jszlparser *parser, unsigned *hash)
{
int len;
struct jszlnode *pnode;

struct jszlnode *current_namespace;

    len = is_valid_string(parser->loc+1, global_seed, hash);
    if(!len) return 0;
    if(!pnode) //error: no value 

    current_namespace = parser->current_namespace;

    if(!current_namespace);
    else if(!current_namespace->child){
        current_namespace->child = pnode;
        current_namespace->count++;
    }
    else{
        PREVIOUS_VALUE->next = pnode;
        parser->current_namespace->count++;
    }

    return len+1;
}


    parser->loc++;
    len = is_valid_key(parser->loc, global_seed, &hash);
    if(parser->loc[len] != '"'){
        LOG_ERROR(JSON_ERROR_SYNTAX, 0);
    }
    atom = &ctx->atom_pool[parser->atom_pool_idx++];
    CURRENT_KEY = atom_add(l_atomTable, ATOM_TABLE_SIZE, atom, hash, len, parser->loc, 0);
    if(key_exists(current_namespace, CURRENT_KEY)){
        LOG_ERROR(JSZLE_DUP_KEY, 0); 
    }



int key_handler(struct jszlparser *parser, unsigned *hash,
    struct atom * atom_pool, int pool_idx, struct atom **atom)
{
int len;

    len = is_valid_key(parser->loc+1, global_seed, hash);
    if(parser->loc[len] != '"') return 0;

    *atom = &atom_pool[pool_idx];
    parser->curkey = atom_add(g_atomTable, ATOM_TABLE_SIZE, atom, hash, len, loc);
    if(key_exists(parser->current_namespace, parser->current_key)){
        return JSZLE_DUP_KEY; 
    }
    return len+1;
}

struct jszlnode * new_node(struct jszlparser *parser)
{
    struct jszlnode *pnode;

    if(parser->pool_idx == parser->node_pool_size){
        return 0;
    }
    pnode = &parser->node_pool[parser->pool_idx++];
    return pnode;
}

struct jszlnode * new_atom()

typedef int (*jszl_string_handler)(struct jszlparser *);
typedef int (*jszl_key_handler)(struct jszlparser *);


/*
** validation function for a JSON value
*/
static enum jszltype validate_value(
    struct jszlparser *parser
   ,jszl_string_handler string_handler
   ,struct value_data *vd
){
struct jszlnode *pnode;
unsigned length;
unsigned type;

    // create new value
    pnode = new_node(parser);
    if(!pnode){
        //log error: cannot create node
        return 0;
    }

    switch(*parser->loc){
    case '"':
        length = string_handler(parser);
        if(!length) return 0; // log error: invalid string
        pnode->type   = TYPE_STRING;
        pnode->value  = parser->loc+1;
        pnode->length = len;
        break;

    case '[':
        pnode->type  = TYPE_ARRAY;
        pnode->child = 0;
        pnode->count = 0;
        break;

    case '{':
        pnode->type  = TYPE_OBJECT;
        pnode->child = 0;
        pnode->count = 0;
        break;

    case 'f':
        if(memcmp(parser->loc, "false", 5)){
            break;
        }
	pnode->type   = TYPE_BOOLEAN;
        pnode->value  = 0;
        pnode->length = 5;
        break;

    case 't':
        if(memcmp(parser->loc, "true", 4)){
            break;
        }
        pnode->type   = TYPE_BOOLEAN;
        pnode->value  = 1;
        pnode->length = 4;
	break;

    case 'n':
	if(memcmp(parser->loc, "null", 4)){
            break;
        }
        pnode->type   = TYPE_NULL;
        pnode->value  = 0;
        pnode->length = 4;
	break;

    default:
        if(IS_DEC_DIGIT(n) || n == '-'){
            vd->length = is_valid_number(parser->loc, &vd->numType, &vd->isNegative);
	    if(!vd->length){ 
                break;
            }
	    else if(vd->numType == _NumberTypeHex){
                n = hextoint(parser->loc+2, vd->length-2);
            }
	    else if(vd->numType == _NumberTypeDecimal){
                n = atouint(parser->loc, len);
            }
            return TYPE_NUMBER;
        }

	// log error: invalid value
	// delete node
	return 0;
    }

    pnode->name = parser->curkey;
    pnode->next = 0;
    parser->prevnode = pnode;
    return pnode->type;
}


static unsigned json_parser_engine(
  struct jszlparser *parser
 ,struct jszlcontext *ctx
 ,const char *str
 ,jszl_key_handler key_handler
 ,jszl_string_handler string_handler
){
struct jszlnode *current_namespace = 0;
const char *loc;
struct atom * atom;
struct value_data vd = {0};

void * l_atomTable = g_AtomTable;

unsigned type = 0;
unsigned subtype = 0;
unsigned numtype = 0;
unsigned isNegative = 0;
unsigned len = 0;
unsigned hash;
int n;


    if(parser->phase){
        current_namespace = parser->ns_stack[parser->stack_idx].namespace;

        switch(parser->phase){
        case _ARRAY_PHASE_OPT_VALUE: goto ARRAY_PHASE_OPT_VALUE;
        case _ARRAY_PHASE_REQ_VALUE: goto ARRAY_PHASE_REQ_VALUE;
        case _ARRAY_PHASE_COMMA:     goto ARRAY_PHASE_COMMA;
        case _OBJECT_PHASE_OPT_KEY:  goto OBJECT_PHASE_OPT_KEY;
        case _OBJECT_PHASE_REQ_KEY:  goto OBJECT_PHASE_REQ_KEY;
        case _OBJECT_PHASE_COLON:    goto OBJECT_PHASE_COLON;
        case _OBJECT_PHASE_VALUE:    goto OBJECT_PHASE_VALUE;
        case _OBJECT_PHASE_COMMA:    goto OBJECT_PHASE_COMMA;
        }
    }
    else{
        parser->loc = str;
        LINE = 1;
    }

    parser->loc += skip_ws(parser->loc, &LINE);
    if(*parser->loc == '['){
        parser->current_namespace = new_value(parser, TYPE_ARRAY);
        if(!parser->current_namespace) { LOG_ERROR(JSZLE_NO_MEMORY, 0); }
        parser->ns_stack[parser->stack_idx].namespace = parser->current_namespace;
    }
    else if(*parser->loc == '{'){
        parser->current_namespace = new_value(parser, TYPE_OBJECT);
        if(!parser->current_namespace) { LOG_ERROR(JSZLE_NO_MEMORY, 0); }
        parser->ns_stack[parser->stack_idx].namespace = parser->current_namespace;
    }
    else {
        LOG_ERROR(JSON_ERROR_TYPE_MISMATCH, 0);  
    }
    parser->root_namespace = parser->current_namespace;
    ctx->CurrentNS = parser->root_namespace;
    parser->loc++;


  ENTER_NAMESPACE:
    if(GET_VALUE_TYPE((*parser->current_namespace)) == TYPE_OBJECT){
        goto OBJECT_PHASE_OPT_KEY;
    }


  ARRAY_PHASE_OPT_VALUE:
    parser->phase = _ARRAY_PHASE_OPT_VALUE;
    parser->loc += skip_ws(parser->loc, &parser->line);
    if(*parser->loc == ']'){
        if(parser->current_namespace == parser->root_namespace)
            goto EXIT_ROOT_NAMESPACE;
        parser->prevnode = parser->current_namespace;
        parser->current_namespace = parser->ns_stack[--parser->stack_idx].namespace;
        parser->loc++;
        if(GET_VALUE_TYPE((*parser->current_namespace)) == TYPE_ARRAY)
            goto ARRAY_PHASE_COMMA;
	else
            goto OBJECT_PHASE_COMMA;
    }

  ARRAY_PHASE_REQ_VALUE:
    parser->phase = _ARRAY_PHASE_REQ_VALUE;
    parser->loc += skip_ws(parser->loc, &parser->line);
    type = validate_value(parser, 0);
    switch(type){
        case TYPE_OBJECT:
        case TYPE_ARRAY:
            break;
        case 0:
            //value error
        default:
            goto ARRAY_PHASE_COMMA;
    }
    parser->current_key = 0;
    parser->current_namespace = parser->prevnode;
    if(++parser->stack_idx == MAX_NS_LEVEL){
        return 0; 
    }
    parser->ns_stack[parser->stack_idx].namespace = parser->current_namespace;
    parser->loc++;
    goto ENTER_NAMESPACE;

    
  ARRAY_PHASE_COMMA:
    parser->phase = _ARRAY_PHASE_COMMA;
    parser->loc += skip_ws(parser->loc, &parser->line);
    if(*parser->loc == ','){
        parser->loc++;
        goto ARRAY_PHASE_REQ_VALUE;
    }
    else if(*parser->loc == ']'){
        if(parser->current_namespace == parser->root_namespace)
            goto EXIT_ROOT_NAMESPACE;
        parser->prevnode = parser->current_namespace;
        parser->current_namespace = parser->ns_stack[--parser->stack_idx].namespace;
        parser->loc++;
        if(GET_VALUE_TYPE((*parser->current_namespace)) == TYPE_ARRAY)
            goto ARRAY_PHASE_COMMA;
        else
            goto OBJECT_PHASE_COMMA;
    }
    else{
        LOG_ERROR(JSON_ERROR_SYNTAX, "Error on comma in array");
    }

  OBJECT_PHASE_OPT_KEY:
    parser->phase = _OBJECT_PHASE_OPT_KEY;
    parser->loc += skip_ws(parser->loc, &parser->line);
    if(*parser->loc == '}'){
        if(parser->current_namespace == parser->root_namespace)
            goto EXIT_ROOT_NAMESPACE;
        parser->prevnode = parser->current_namespace;
        parser->current_namespace = parser->ns_stack[--parser->stack_idx].namespace;
        parser->loc++;
        if(GET_VALUE_TYPE((*parser->current_namespace)) == TYPE_ARRAY)
            goto ARRAY_PHASE_COMMA;
        else
            goto OBJECT_PHASE_COMMA;
    }

  OBJECT_PHASE_REQ_KEY:
    parser->phase = _OBJECT_PHASE_REQ_KEY;
    parser->loc += skip_ws(parser->loc, &parser->line);

    if(*parser->loc != '"'){
        LOG_ERROR(JSON_ERROR_SYNTAX, "Error on key"); 
    }
    key_handler(parser);
    parser->loc += len + 1;

  OBJECT_PHASE_COLON:
    parser->phase = _OBJECT_PHASE_COLON;
    parser->loc += skip_ws(parser->loc, &parser->line);
    if(*parser->loc != ':')
        LOG_ERROR(JSON_ERROR_SYNTAX, "Error on colon");
    parser->loc++;

  OBJECT_PHASE_VALUE:
    parser->phase = _OBJECT_PHASE_VALUE;
    parser->loc += skip_ws(parser->loc, &parser->line);

    type = validate_value(parser, 0);
    switch(type){
        case TYPE_OBJECT:
        case TYPE_ARRAY:
            break;
        case 0:
            //value error
        default:
            goto OBJECT_PHASE_COMMA;
    }
    parser->current_key = 0;
    parser->current_namespace = parser->prevnode;
    if(++parser->stack_idx == MAX_NS_LEVEL){
        return 0; 
    }
    parser->ns_stack[parser->stack_idx].namespace = parser->current_namespace;
    parser->loc++;
    goto ENTER_NAMESPACE;

    CURRENT_KEY = 0;
    type = 0;

  OBJECT_PHASE_COMMA:
    parser->phase = _OBJECT_PHASE_COMMA;
    parser->loc += skip_ws(parser->loc, &parser->line);
    if(*parser->loc == ','){
        parser->loc++;
        goto OBJECT_PHASE_REQ_KEY;
    }
    else
    if(*parser->loc == '}'){
        if(parser->current_namespace == parser->root_namespace)
            goto EXIT_ROOT_NAMESPACE;
        parser->prevnode = parser->current_namespace;
        parser->current_namespace = parser->ns_stack[--parser->stack_idx].namespace;
        parser->loc++;
        if(GET_VALUE_TYPE((*parser->current_namespace)) == TYPE_ARRAY)
            goto ARRAY_PHASE_COMMA;
        else
            goto OBJECT_PHASE_COMMA;
    }
    else {
        LOG_ERROR(JSON_ERROR_SYNTAX, "Error on comma in object");
    }

  ERROR_CLEANUP:
    printf("Error(%u) on line %u\n", 1, parser->line);
    return 1;
 
  EXIT_ROOT_NAMESPACE:
    parser->phase = 0;
    return JSZLE_NONE;

} //END ENGINE


