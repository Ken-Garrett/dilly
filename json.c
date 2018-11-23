#include <assert.h>
//#include "..\core.h"
#include "dictionary.h"
#include "fmap.h"
#include "json.h"


#define BEGIN_OBJECT    '{'
#define END_OBJECT      '}'
#define BEGIN_ARRAY     '['
#define END_ARRAY       ']'
#define NAME_SEPARATOR  ':'
#define VALUE_SEPARATOR ','

//#define ESCAPE '?' '\'

#define WHITE_SPACE (' ' || '\t' || '\n' || '\r')

//TLT(Object) -> STRING -> NAME:SEPARATOR -> VALUE
//TLT(Array)  -> VALUE  -> 

// Literals : false, true, null


/**
 * 1. STRIP WHITESPACE
 * 2. CHECK BEGIN_OBJECT OR BEGIN_ARRAY
 * 3. IF OBJECT:
 *        CHECK_STRING 
 *    IF ARRAY:
 *        CHECK_VALUE
**/

/**
 * TOP LEVEL TOKEN
 * 1. TRIM WHITESPACE
 * 2. BEGIN_OBJECT OR BEGIN_ARRAY
 * 3. CHECK FOR VALUE
 *     ENTER VALUE STATE
 * 4. CHECK END_ARRAY 
**/


/* VALUE VALIDATORS
 * CheckFalse  e.g. if(c == 'f')          CheckFalse
 * CheckTrue   e.g. if(c == 't')          CheckTrue
 * CheckNull   e.g. if(c == 'n')          CheckNull
 * CheckObject e.g. if(c == BEGIN_OBJECT) CheckObject
 * CheckArray  e.g. if(c == BEGIN_ARRAY)  CheckArray
 * CheckNumber e.g. if((c > 47 && c < 58) || c == '-')  CheckNumber
 * CheckString e.g. if(c == '"')          CheckString
*/

//static unsigned trim_whitespace(char **ptr);


union ipv6
{
struct {
    unsigned char
    b1, b2, b3, b4, b5, b6, b7, b8,
    b9, b10, b11, b12, b13, b14, b15, b16;
    } b1;
unsigned char b[16];

struct {
    unsigned short
    w1, w2, w3, w4, w5, w6, w7, w8;
    } w1;
unsigned short w[8];

struct {
    unsigned int
    dw1, dw2, dw3, dw4; 
    } dw1;
unsigned int dw[4];
};

struct json{
struct dictionary *dict;
HANDLE hfile;
HANDLE hmap;
void *fmap;
};

typedef struct parser{
unsigned tlt; //Top level token
unsigned err;
//char errmsg[128];
unsigned ln; //current line
unsigned lnofs; //current line offset
unsigned (*fn)(struct state*, char **pstr);
const char *loc; //current location
struct dictionary *dict;
} PARSER;

JsonT _json_ctor(const char *file, const char *_file, unsigned _line)
{
struct json *jso;

    jso = malloc(sizeof(struct json));
    assert(jso);

    jso->dict = dict_ctor(malloc, free);
    assert(jso->dict);

    jso->fmap = create_filemap(file, &jso->hfile, &jso->hmap);
    return jso;
}

//static unsigned validate_value(STATE *s, unsigned);
static unsigned enter_object(PARSER *p);

#include "shared.c"

enum phase{
    PHASE_KEY = 1,
    PHASE_VALUE,
    PHASE_COMMA,
    PHASE_COLON
};

#define NEW_LINE(p, str) p->ln++; p->lnofs = (unsigned)str

#define TRIM_WHITESPACE(pparser, str, c)\
    do{\
        c = *str;\
        if(c == ' ' || c == '\t') str++;\
        else\
        if(c == '\r'){\
            if(str[1] == '\n'){\
                str+=2;\
                NEW_LINE(pparser, str);\
            }\
            else printf("Invalid char\n");\
        }\
        else\
        if(c == '\n'){\
            NEW_LINE(pparser, str);\
            str++;\
        }\
        else break;\
    }while(1)

#define VALIDATE_VALUE(p, str, type)\
    switch(*str){\
    case 'f': \
        if(type && type != DICT_TYPE_BOOLEAN) return 0;\
        if(memcmp(str, "false", 5)){\
            printf("Error: false\n");\
            return 0;}\
        str += 5;\
        break;\
    case 't':\
        if(type && type != DICT_TYPE_BOOLEAN) return 0;\
        if(memcmp(str, "true", 4)){\
            printf("Error: true %c\n", *str);\
            return 0;}\
        str += 4;\
        break;\
    case 'n':\
        if(memcmp(str, "null", 4)){\
            printf("Error: null\n");\
            return 0;}\
        str += 4;\
        break;\
    case '"':\
        str++;\
        switch(type){\
        case 0:\
        case DICT_TYPE_STRING:\
            n = validate_string(str);\
            assert(n);\
            str += n-1;\
            break;\
        case DICT_TYPE_IPV4:\
            n = validate_ipv4(str, 0, &n);\
            if(!n) errfn(JSON_ERROR_TYPE, p->ln, 0);\
            str += n; break;\
        case DICT_TYPE_IPV6: {\
            union ipv6 ipv6 = {0}; \
            n = validate_ipv6(str, 0, &ipv6);\
            if(!n) errfn(JSON_ERROR_TYPE, p->ln, 0);\
            printf("%u\n", ipv6.w[0]);\
            printf("%u\n", ipv6.w[1]);\
            str += n; break; }\
        case DICT_TYPE_UUID:\
            n = validate_uuid(str);\
            if(!n) errfn(JSON_ERROR_TYPE, p->ln, 0);\
            str += n; break;\
        case DICT_TYPE_DATE:\
            n = validate_date(str);\
            if(!n) errfn(JSON_ERROR_TYPE, p->ln, 0);\
            str+=n; break;\
       default:\
            printf("Type Error\n");\
            return 0;\
        }\
        if(*str != '"'){\
            printf("Expecting end string %c\n", *str);\
            return 0;}\
        str++;\
        break;\
    case '{':\
        if(type && type != DICT_TYPE_OBJECT) return 0;\
        str++;\
        assert(enter_object(p));\
        break;\
    case '[':\
        str++;\
        assert(enter_array(p, type));\
        break;\
    default:\
        n = *str;\
        if(IS_DEC_DIGIT(n) || n == '-'){\
            switch(type){\
            case 0:\
                n = validate_number(str);\
                assert(n);\
                str += n;\
                break;\
            default:\
                printf("Type Error line %d (%d)\n",\
                   p->ln, (unsigned)str - p->lnofs);\
                return 0;\
            }\
            break;\
        }\
        errfn(JSON_ERROR_VALUE, p->ln, p->lnofs);\
        return 0;\
    }

unsigned errfn(unsigned error, unsigned line, unsigned offset)
{
    switch(error){
    case JSON_ERROR_SYNTAX:
        printf("Syntax error on line %u (%u)\n", line, offset); 
	exit(1);
    case JSON_ERROR_VALUE:
        printf("Error in value on line %u (%u)\n", line, offset);
        exit(1);
    case JSON_ERROR_TYPE:
        printf("Type Error %u (%u)\n", line, offset);
        exit(1);
    }
    return 1;
}
 
static unsigned enter_array(PARSER *p, unsigned type)
{
unsigned n;
const char *str = p->loc;
char phase = PHASE_VALUE;

//printf("BEGIN ARRAY\n");
    while(1){
        TRIM_WHITESPACE(p, p->loc, n);

        switch(phase){
        case PHASE_VALUE:
            VALIDATE_VALUE(p, p->loc, type);
            phase = PHASE_COMMA;
            break;
        case PHASE_COMMA:
            if(*p->loc == ','){
                phase = PHASE_VALUE;	    
                p->loc++;
                break;
            }
            else if(*p->loc == ']'){
                p->loc++;
                //printf("END ARRAY\n");
                return 1;
            }
            else{
                if(!errfn(JSON_ERROR_SYNTAX, p->ln, p->lnofs))
                    return 0;
            }
        }
    }
}

/** TODO
 * Fix issue where error reporting innacurate offset when looking for comma
 * This issue is caused because white space is being trimmed before
 * the error is detected
**/

static unsigned enter_object(PARSER *p)
{
struct dictns *ns;
unsigned n;
const char *str = p->loc;
char phase = PHASE_KEY;
unsigned short type = 0;

//printf("BEGIN OBJECT\n");
    while(1){
        TRIM_WHITESPACE(p, p->loc, n);

        switch(phase){
//hash the key 
        case PHASE_KEY:
            if(*p->loc == '"'){
                p->loc++;
                str = p->loc;
                for(; *p->loc != '"'; p->loc++)
                    if(!p->loc) return 0;
                if(!memcmp("IPV4", str, 4)) type = DICT_TYPE_IPV4;
		else if(!memcmp("UUID", str, 4)) type = DICT_TYPE_UUID;
		else if(!memcmp("Object", str, 6)) type = DICT_TYPE_OBJECT;
		else if(!memcmp("IPV6", str, 4)) type = DICT_TYPE_IPV6;
                p->loc++;
                phase = PHASE_COLON;
                break;
            }
            else if(*p->loc == '}'){
                p->loc++;
                //printf("END OBJECT\n"); 
                return 1;
            }
            else{
                if(!errfn(JSON_ERROR_SYNTAX, p->ln, (unsigned)str - p->lnofs));
		    return 0;
            }
        case PHASE_COLON:
            if(*p->loc != ':'){
                if(!errfn(JSON_ERROR_SYNTAX, p->ln, (unsigned)str - p->lnofs))
                    return 0;
            }
            p->loc++;
            phase = PHASE_VALUE;
            break;
	case PHASE_COMMA:
            if(*p->loc == ','){
                p->loc++;
                phase = PHASE_KEY;
                break;
            }
            else if(*p->loc == '}'){
                p->loc++;
                //printf("END OBJECT\n");
                return 1;
            }
            else{
                if(!errfn(JSON_ERROR_SYNTAX, p->ln, (unsigned)str - p->lnofs))
                    return 0;
            }

	case PHASE_VALUE:
            VALIDATE_VALUE(p, p->loc, type);
            type = 0;
            phase = PHASE_COMMA;
            break;
        }
    }
}

unsigned _json_load(struct json *jso, const char *_file, unsigned _line)
{
int cnt;
PARSER parser = {0};

    parser.ln = 1;
    // state machine
    parser.loc = jso->fmap;

    //parser.loc += trim_whitespace(parser.loc);
    TRIM_WHITESPACE((&parser), parser.loc, cnt);
    
    if(*parser.loc == '['){
        parser.loc++;
        enter_array(&parser, 0);
    }
    else if(*parser.loc == '{'){
        parser.loc++;
        enter_object(&parser);
    }
    else printf("Error: Invalid token\n");

    return 1;
}
