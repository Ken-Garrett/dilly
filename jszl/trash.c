/*
static unsigned validate_value(PARSER *s, unsigned type)
{
const char *str = s->str;
unsigned n = 0;
    switch(*str){
    case 'f':
        if(type && type != DICT_TYPE_BOOLEAN) return 0;
        if(memcmp(str, "false", 5)){
            printf("Error: false\n");
            return 0;
        }
        return 5;
    case 't':
        if(type && type != DICT_TYPE_BOOLEAN) return 0;
        if(memcmp(str, "true", 4)){
            printf("Error: true\n");
            return 0;
        }
        return 4;
    case 'n':
        if(memcmp(str, "null", 4)){
            printf("Error: null\n");
            return 0;
        }
        return 4;
    case '"':
        switch(type){
        case 0:
        case DICT_TYPE_STRING:
            n = validate_string(str+1);
        case DICT_TYPE_IPV4:
	    n = validate_ipv4(str+1);
        }
        assert(len);
	n += 1;
        if(str+n != '"'){
            printf("Expecting end string\n");	
            return 0;
        }
	return n+1;
    case '{':
        s->str++;
        assert(enter_object(s));
        break;
    case '[':
        s->str++;
        assert(enter_array(s));
        break;
    default:
        n = *str;
        if(IS_DEC_DIGIT(n) || n == '-'){
            n = validate_number(str);
            assert(n);
            return n;
        }
        printf("Error: %c\n", *s->str);
        return 0;
    }
}
*/

/*
if(c == '\n'){
    state.ln++;
    state.lnofs = 0;
}
*/

/*
static unsigned trim_whitespace(const char *str)
{
char *pstr = str;
unsigned c;
    for(c = *pstr;
        c && (c == ' ' || c == '\t' || c == '\n' || c == '\r');
        c = *++pstr);
    c = pstr - str;
    //if(c) *ptr = pstr;
    //printf("whitepsace: %u\n", c);
    return c;
}
*/

/*
unsigned check_false(

#define CHECK_VALUE(c) \
    if(c == 'f')      memcmp(str, "false", 5);\
    else if(c == 't') memcmp(str, "true", 4);\
    else if(c == 'n') memcmp(str, "null", 4);\
    else if(c == '"') ;\
    else if(c == '{') ;\
    else if(c == '[') ;\
    else if(IS_DEC_DIGIT(c) || c == '-') ;\
    else printf("error\n");
*/

#define ARRAY_LOOP(state)                           \
    do {                                            \
        TRIM_WHITESPACE(state);                     \
        switch(state.phase){                        \
        case PHASE_OPT_VALUE:                       \
            if(*state.loc == ']') break;            \
        case PHASE_REQ_VALUE:                       \
            VALIDATE_VALUE(state, state.loc, type); \
            state.phase = PHASE_COMMA;              \
            continue;                               \
        case PHASE_COMMA:                           \
            if(*state.loc == ','){                  \
                state.loc++;                        \
                state.phase = PHASE_REQ_VALUE;      \
                continue;                           \
            }                                       \
            else if(*state.loc == ']') {            \
                break;                              \
            }                                       \
            else{                                   \
                err = JSON_ERROR_SYNTAX;            \
                printf("ERROR\n");                  \
            }                                       \
        }                                           \
        state.loc++;                                \
        break;                                      \
    } while(1)

//50 chars out
#define OBJECT_LOOP(state)                        \
do {                                              \
    TRIM_WHITESPACE(state);                       \
    switch(state.phase){                          \
    case PHASE_OPT_KEY:                           \
        if(*state.loc == '}') break;              \
    case PHASE_REQ_KEY:                           \
        if(*state.loc == '"'){                    \
            state.str = state.loc;                \
            state.loc++;                          \
            while(*state.loc != '"'){             \
                if(!state.loc) return 0;          \
                state.loc++;                      \
            }                                     \
            state.curkey = atom_add(state.table, state.str, state.loc - state.str);\
	printf("here\n");\
            type = schema_find_key(str);          \
            state.loc++;                          \
            state.phase = PHASE_COLON;            \
            continue;                             \
        }                                         \
        err = JSON_ERROR_SYNTAX;                  \
        printf("ERROR FOR KEY\n");                \
    case PHASE_COLON:                             \
        if(*state.loc != ':')                     \
            err = JSON_ERROR_SYNTAX;              \
        state.loc++;                              \
        state.phase = PHASE_VALUE;                \
        continue;                                 \
    case PHASE_COMMA:                             \
        if(*state.loc == ','){                    \
            state.loc++;                          \
            state.phase = PHASE_REQ_KEY;          \
            continue;                             \
        }                                         \
        else if(*state.loc == '}') {              \
            break;                                \
        }                                         \
        err = JSON_ERROR_SYNTAX;                  \
        printf("ERROR FOR COMMA\n");              \
    case PHASE_VALUE:                             \
        VALIDATE_VALUE(state, state.loc, type);   \
        type = 0;                                 \
        state.phase = PHASE_COMMA;                \
        continue;                                 \
    }                                             \
    state.loc++;                                  \
    break;                                        \
} while(1)

#ifdef RECURSIVE_NS 
static unsigned enter_array(PARSER state, unsigned type)
{
union type_buffer tb = {0};
unsigned n = 0, err = 0;
const char *str = 0;
struct json_ns *this = 0;

    //this->parent = state->cur_ns;
    //state->cur_ns = this;
    
    ALLOC_ARRAY( (*state), this);
    *pcur_ns = this;
    if(!cur_ns->child){
        cur_ns->child = this;
    }
    state->phase = PHASE_OPT_VALUE;
    printf("BEGIN ARRAY\n");
    ARRAY_LOOP( (*state) );
    printf("END ARRAY\n");

    return 1;
}

static unsigned enter_object(PARSER state)
{
union type_buffer tb = {0};
unsigned n, err;
const char *str = state->loc;
unsigned short type = 0;
struct json_ns *this = 0;

    //ns = malloc(sizeof(struct json_ns));

    ALLOC_OBJECT( (*state), this);
    state->phase = PHASE_OPT_KEY;
    printf("BEGIN OBJECT\n");
    OBJECT_LOOP( (*state) );
    printf("END OBJECT\n");

    return 1;
}
#endif

#ifdef RECURSIVE_NS
    state.curkey = 0;
    if(*state.loc == '['){
        state.loc++;
        enter_array(&state, 0);
    }
    else if(*state.loc == '{'){
        state.loc++;
        enter_object(&state);
    }
    else printf("Error: Invalid token\n");

#else


struct json_node *_json_load
(const char *json_string,
 struct atom_table *at,
 struct json_node *schema,
 struct json_node **root,
 struct json_state *state
 DEBUG_DECL)
{

union type_buffer tb = {0};
unsigned n = 0;

struct local local = {0};

char type;
char *str;
char phase = 0;
unsigned len;
const char *loc;
unsigned long hash = 0;
unsigned line;
unsigned short index;
struct atom *patom = 0;
struct json_node *node;

    INIT_PARSER(state);

    loc = json_string;

    loc += skip_ws(loc, &state->line);
    if(*loc == '['){
        if(schema){
            node = json_get(at, schema, "$.type", 0);
            if(memcmp("array", node->value, 5)){
                SET_ERROR(JSON_ERROR_SCHEMA_ERROR);
            }
        }
        NEW_ROOT_ARRAY();
    }
    else if(*loc == '{'){
        if(schema){
            node = json_get(at, schema, "$.type", 0);
            if(memcmp("object", node->value, 6)){ 
                SET_ERROR(JSON_ERROR_SCHEMA_ERROR);
            }
        }
        NEW_ROOT_OBJECT();
    }
    else{
        SET_ERROR(JSON_ERROR_INVALID_TYPE);
    }
    loc++;
    *root = local.cur_ns; 



ENTER_NAMESPACE:
if( local.cur_ns->type == TYPE_ARRAY )
    goto ARRAY_LOOP;
else
if( local.cur_ns->type == TYPE_OBJECT )
    goto OBJECT_LOOP;

ARRAY_LOOP:
    ARRAY_PHASE_OPT_VALUE:
        state->phase = _ARRAY_PHASE_OPT_VALUE;
        loc += skip_ws(loc, &state->line);
        if(*loc == ']'){ DELETE_ARRAY(); }
    ARRAY_PHASE_REQ_VALUE:
        state->phase = _ARRAY_PHASE_REQ_VALUE;
        loc += skip_ws(loc, &state->line);

        //VALIDATE_VALUE(state, loc, 0);

//VALIDATE_VALUE
    if(n < 100)
    switch(n){
    case '"':
        n = *++loc;
        switch(type){
	case 0:
        case TYPE_STRING:
        case SUBTYPE_IPV4:
        case SUBTYPE_IPV6:
	case SUBTYPE_UUID:
	case SUBTYPE_DATE:
	default:
	}
    case '[':
        if(type && (type != TYPE_ARRAY)){}
        node = new_node(state, local, TYPE_ARRAY, 0, 0);
        local.cur_ns = local.last_value;
        state->ns_buffer.array[++state->ns_buffer.index].node = local.cur_ns;
        loc++;
        
    default:
        if(!IS_DEC_DIGIT(n) && n != '-'){
            SET_ERROR(JSON_ERROR_INVALID_VALUE);
            break;
        }

        switch(type){
        case 0:
            n = validate_number(loc, &num_type);
            assert(n);
            switch(num_type){
            case SUBTYPE_HEX:
                num = hextoint(loc+2, n-2);
                break;
            case TYPE_INTEGER:
                num = atouint(loc, n);
                break;
            }
            node = new_node(state, local, type, n);
            STORE_VALUE(num_type, num);
            loc += n;
            break;
        default:
            SET_ERROR(JSON_ERROR_INVALID_VALUE);
        }

        break;
    }
 
    else
    switch(n){
    case '{':
        if(type && type != TYPE_OBJECT){}
        node = new_node(state, local, TYPE_OBJECT, 0, 0);
        local.cur_ns = local.last_value;
        state->ns_buffer.array[++state->ns_buffer.index].node = local->cur_ns;
        loc++;
        goto ENTER_NAMESPACE;
    case 'f':
        if(type && (type != TYPE_BOOLEAN)){}
    case 't':
        if(type && (type != TYPE_BOOLEAN)){}
    case 'n':
        if(type){}
    default:
    }
//END VALIDATE VALUE

    ARRAY_PHASE_COMMA:
        state->phase = _ARRAY_PHASE_COMMA;
        loc += skip_ws(loc, &state->line);
        if(*loc == ','){
            loc++;
            goto ARRAY_PHASE_REQ_VALUE;
        }
        else
        if(*loc == ']'){ DELETE_ARRAY(); }
        else{
            SET_ERROR(JSON_ERROR_SYNTAX);
        }

OBJECT_LOOP:
    OBJECT_PHASE_OPT_KEY:
        state->phase = _OBJECT_PHASE_OPT_KEY;
        loc += skip_ws(loc, &state->line);
        if(*loc == '}'){ DELETE_OBJECT(); }
    OBJECT_PHASE_REQ_KEY:
        state->phase = _OBJECT_PHASE_REQ_KEY;
        loc += skip_ws(loc, &state->line);
        if(*loc == '"'){
            loc++;
            len = valid_key(loc, global_seed, &hash);
            if(loc[len] != '"'){
                SET_ERROR(JSON_ERROR_SYNTAX);
            }
	    //printf("%u : %.*s : %u\n", hash, len, loc, len);
            state->curkey = atom_add(at, hash, len, loc, state->alloc, 0); 
            KEY_EXISTS(node, state->curkey);
            node = key_exists(local.cur_ns, state->curkey); 
            if(node){
            SET_ERROR(JSON_ERROR_KEY_ALREADY_EXISTS);
            }
            //type = schema_find_key(state.str);
            loc += len +1;
        }
        else{
            SET_ERROR(JSON_ERROR_SYNTAX);
        }
    OBJECT_PHASE_COLON:
        state->phase = _OBJECT_PHASE_COLON;
        loc += skip_ws(loc, &state->line);
        if(*loc != ':')
            SET_ERROR(JSON_ERROR_SYNTAX);
        loc++;
    OBJECT_PHASE_VALUE:
        state->phase = _OBJECT_PHASE_VALUE;
        loc += skip_ws(loc, &state->line);
        //VALIDATE_VALUE(state, loc, 0);
        state->curkey = 0;
        type = 0;
    OBJECT_PHASE_COMMA:
        state->phase = _OBJECT_PHASE_COMMA;
        loc += skip_ws(loc, &state->line);
        if(*loc == ','){
            loc++;
            goto OBJECT_PHASE_REQ_KEY;
        }
        else
        if(*loc == '}'){ DELETE_OBJECT(); }
        else {
            SET_ERROR(JSON_ERROR_SYNTAX);
        }
EXIT_ROOT_NAMESPACE:
state->phase = 0;
return 0;
}

/*
unsigned inline validate_value(struct json_state *state,
	struct local *local, const char *loc, short type, short subtype)
{
struct json_node *node;
void *start = loc;
unsigned num, num_type;
unsigned n = *loc;
if(n < 100)
    switch(n){
    case '"':
        n = *++loc;
        switch(type){
	case 0:
        case TYPE_STRING:
        case SUBTYPE_IPV4:
        case SUBTYPE_IPV6:
	case SUBTYPE_UUID:
	case SUBTYPE_DATE:
	default:
	}
    case '[':
        if(type && (type != TYPE_ARRAY)){}
        node = new_node(state, local, TYPE_ARRAY, 0, 0);
        local->cur_ns = local->last_value;
        state->ns_buffer.array[++state->ns_buffer.index].node = local_cur_ns;
        loc++;
        
    default:
        if(IS_DEC_DIGIT(n) || n == '-'){
            switch(type){
            case 0:
                n = validate_number(loc, &num_type);
                assert(n);
                switch(num_type){
                case SUBTYPE_HEX:
                    num = hextoint(loc+2, n-2);
                    break;
                case TYPE_INTEGER:
                    num = atouint(loc, n);
                    break;
                }
                node = new_node(state, local, type, n);
                STORE_VALUE(num_type, num);
                loc += n;
                break;
            default:
                SET_ERROR(JSON_ERROR_INVALID_VALUE);
            }
            break;
        }
        SET_ERROR(JSON_ERROR_INVALID_VALUE);
 
    }
else
    switch(n){
    case '{':
        if(type && type != TYPE_OBJECT){}
        node = new_node(state, local, TYPE_OBJECT, 0, 0);
        local->cur_ns = local->last_value;
        state->ns_buffer.array[++state->ns_buffer.index].node = local->cur_ns;
        loc++;
        goto ENTER_NAMESPACE;
    case 'f':
        if(type && (type != TYPE_BOOLEAN)
    case 't':
    case 'n':
    default:
    }
}
*/

/*
#define VALIDATE_VALUE(p, loc, type)                 \
n = *loc;\
if(n < 100){\
    switch(n){\
    case '"':\
        loc++;\
        switch(type){\
        case 0:\
        case TYPE_STRING:\
            n = validate_string(loc, global_seed, &hash);\
            if(!n) SET_ERROR(JSON_ERROR_INVALID_VALUE);\
            STORE_VALUE(TYPE_STRING, loc);\
            local.last_value->length = n-1;\
            loc += n-1;\
            break;\
        case SUBTYPE_IPV4:\
            n = validate_ipv4(loc, 0, &tb.ipv4);    \
            if(!n) SET_ERROR(JSON_ERROR_TYPE_MISMATCH);\
            loc += n;                               \
            break;                                  \
        case SUBTYPE_IPV6: {                      \
            n = validate_ipv6(loc, 0, &tb.ipv6);    \
            if(!n) SET_ERROR(JSON_ERROR_TYPE_MISMATCH);\
            loc += n;                               \
            break;                                  \
            }                                       \
        case SUBTYPE_UUID:                        \
            n = validate_uuid(loc);                 \
           if(!n) SET_ERROR(JSON_ERROR_TYPE_MISMATCH);\
            loc += n;                               \
            break;                                  \
        case SUBTYPE_DATE:                        \
            n = validate_date(loc);                 \
            if(!n) SET_ERROR(JSON_ERROR_TYPE_MISMATCH);\
            loc += n;                               \
            break;                                  \
        default:                                    \
            SET_ERROR(JSON_ERROR_TYPE_MISMATCH);    \
        }                                           \
        if(*loc != '"')\
            SET_ERROR(JSON_ERROR_INVALID_VALUE);\
        loc++;\
        break;\
    case '[':\
        if(type && (type != TYPE_ARRAY))\
            SET_ERROR(JSON_ERROR_TYPE_MISMATCH);\
        NEW_ARRAY();\
    default:\
        if(IS_DEC_DIGIT(n) || n == '-'){\
            unsigned num, num_type;\
            switch(type){\
            case 0:\
                n = validate_number(loc, &num_type);\
                assert(n);\
                switch(num_type){\
                case SUBTYPE_HEX:\
                    num = hextoint(loc+2, n-2);\
                    break;\
                case TYPE_INTEGER:\
                    num = atouint(loc, n);\
                    break;\
                }\
                STORE_VALUE(num_type, num);\
                loc += n;\
                break;\
            default:\
                SET_ERROR(JSON_ERROR_INVALID_VALUE);\
            }\
            break;\
        }\
        SET_ERROR(JSON_ERROR_INVALID_VALUE);\
    }\
}\
else{\
    switch(n){\
    case '{':\
        if(type && type != TYPE_OBJECT)\
            SET_ERROR(JSON_ERROR_TYPE_MISMATCH);\
        NEW_OBJECT();\
    case 'f':                                        \
        if(type && (type != TYPE_BOOLEAN))      \
            SET_ERROR(JSON_ERROR_TYPE_MISMATCH);          \
        else if(MEM_COMPARE(loc, "false", 5))        \
            SET_ERROR(JSON_ERROR_INVALID_VALUE);     \
        STORE_VALUE(TYPE_BOOLEAN, 0);           \
        loc += 5;\
        break;\
    case 't':\
        if(type && (type != TYPE_BOOLEAN)){\
            SET_ERROR(JSON_ERROR_TYPE_MISMATCH);\
        }\
        else if(MEM_COMPARE(loc, "true", 4)){\
            SET_ERROR(JSON_ERROR_INVALID_VALUE);\
        }\
        STORE_VALUE(TYPE_BOOLEAN, 1);\
        loc += 4;\
        break;\
    case 'n':\
        if(MEM_COMPARE(loc, "null", 4))\
            SET_ERROR(JSON_ERROR_INVALID_VALUE);\
        STORE_VALUE(TYPE_NULL, 0);\
        loc += 4;\
        break;\
    default:\
        SET_ERROR(JSON_ERROR_INVALID_VALUE);\
    }\
}
*/


#define SKIP_WHITESPACE(state)\
    do {\
        if(*loc == ' ' || *loc == '\t')\
            loc++;\
        else if(*loc == '\r'){\
            if(loc[1] == '\n'){\
                loc+=2;\
                state.line++;\
                state.lnofs = (unsigned)loc;\
            }\
            else printf("Invalid char\n");\
	}\
        else if(*loc == '\n'){\
            state.line++;\
            loc++;\
            state.lnofs = (unsigned)loc;\
        }\
        else break;\
    } while(1)


