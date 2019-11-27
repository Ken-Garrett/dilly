
#define LINE_OFFSET
#define NAMESPACE_STACK (parser->ns_stack)
#define ERRORLOG_PROC (parser->log_error)

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


void * allocmem(int size)
{
    return 0;
}

void deallocmem(void *mem)
{}

//typedef unsigned int dword;
static inline void *
new_node(long value, long atom, short type, short count)
{
    struct jszlnode * pnode;
    pnode = malloc(sizeof(struct jszlnode));
    if(!pnode) return 0;

    pnode->value  = value;
    pnode->name   = (void*)atom;
    pnode->type   = type;
    pnode->count  = count;
    pnode->next   = 0;
    return pnode;
}

static inline void
delete_node(void *node)
{}


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
if(ERRORLOG_PROC && !ERRORLOG_PROC(e, msg, parser->line, 0)) { goto ERROR_CLEANUP; }\
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


typedef int (*jszl_string_handler)(struct jszlparser *);
typedef int (*jszl_key_handler)(struct jszlparser *);


/*
** validation function for a JSON value
*/
static enum jszltype validate_value(
    struct jszlparser *parser
   ,jszl_string_handler string_handler
){
struct jszlnode *pnode;
unsigned length = 0;
unsigned type = 0;
long value = 0;
int n;
int numType;
int isNegative;

    switch(*parser->loc){
    case '"':
        length = string_handler(parser);
        if(!length) return 0; // log error: invalid string
        type   = TYPE_STRING;
        value  = (long)parser->loc+1;
        break;

    case '[':
        type  = TYPE_ARRAY;
        break;

    case '{':
        type  = TYPE_OBJECT;
        break;

    case 'f':
        if(memcmp(parser->loc+1, "false", 5)){
            break;
        }
	type   = TYPE_BOOLEAN;
        length = 5;
        break;

    case 't':
        if(memcmp(parser->loc+1, "true", 4)){
            break;
        }
        type   = TYPE_BOOLEAN;
        value  = 1;
        length = 4;
	break;

    case 'n':
	if(memcmp(parser->loc+1, "null", 4)){
            break;
        }
        type   = TYPE_NULL;
        length = 4;
	break;

    default:
        if(IS_DEC_DIGIT(*parser->loc) || *parser->loc == '-'){
            length = is_valid_number(parser->loc+1, &numType, &isNegative);
	    if(length){
	        if(numType == _NumberTypeHex){
                    value = hextoint(parser->loc+2, length-2);
                }
		else if(numType == _NumberTypeDecimal){
                    value = atouint(parser->loc, length);
                }
                value = n;
                type = TYPE_NUMBER;
                break;
            }
        }

	// log error: invalid value
	return 0;
    }

    pnode = new_node(value, (long)parser->curkey, type, length);
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
        parser->line = 1;
    }

    parser->loc += skip_ws(parser->loc, &parser->line);
    if(*parser->loc == '['){
        parser->current_namespace = new_node(0, 0, TYPE_ARRAY, 0);
        if(!parser->current_namespace) { LOG_ERROR(JSZLE_NO_MEMORY, 0); }
        parser->ns_stack[parser->stack_idx].namespace = parser->current_namespace;
    }
    else if(*parser->loc == '{'){
        parser->current_namespace = new_node(0, 0, TYPE_OBJECT, 0);
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
    parser->curkey = 0;
    parser->current_namespace = parser->prevnode; //namespace node is stored here
    if(++parser->stack_idx == MAX_NS_LEVEL){
        return 0; //exit, out of namespace memory 
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
    parser->curkey = 0;
    parser->current_namespace = parser->prevnode;
    if(++parser->stack_idx == MAX_NS_LEVEL){
        return 0; 
    }
    parser->ns_stack[parser->stack_idx].namespace = parser->current_namespace;
    parser->loc++;
    goto ENTER_NAMESPACE;

    parser->curkey = 0;
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


