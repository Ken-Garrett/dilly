#ifndef JSZL_H
#define JSZL_H

#undef DEBUG_PARAMS
#define DECLPARAMS const char *__file__, unsigned __line__, const char *__function__
#define DEBUG_PARAMS() __FILE__, __LINE__, __FUNCTION__
#define PUBLIC

// Macros

/* VALUE VALIDATORS
 * CheckFalse  e.g. if(c == 'f')          CheckFalse
 * CheckTrue   e.g. if(c == 't')          CheckTrue
 * CheckNull   e.g. if(c == 'n')          CheckNull
 * CheckObject e.g. if(c == BEGIN_OBJECT) CheckObject
 * CheckArray  e.g. if(c == BEGIN_ARRAY)  CheckArray
 * CheckNumber e.g. if((c > 47 && c < 58) || c == '-')  CheckNumber
 * CheckString e.g. if(c == '"')          CheckString
*/

//http://json-schema.org/draft-07/schema#
//http://json-schema.org/draft-06/schema#
//http://json-schema.org/draft-04/schema#
//http://json-schema.org/draft-03/schema#
//http://json-schema.org/draft-02/schema#
//http://json-schema.org/draft-01/schema#

/*json schema formats
 * https://json-schema.org/understanding-json-schema/reference/string.html
 * date-time
 * time
 * date
 * email
 * idn-email
 * hostname
 * idn-hostname
 * ipv4
 * ipv6
 * uri
 * uri-reference
 * iri
 * iri-reference
 * uri-template
 * json-pointer
 * relative-json-pointer
 * regex
 */

/* "$id" : "https://example.com/person.schema.json"
 * "$schema" : "http://json-schema.org/draft-07/schema#"
 * "title" : "MyInstanceDataModel"
 * "type" : "object"
 * "properties": {}
*/


#define JSZLPARSE_CASE_SENSITIVE_KEYS
#define JSONFLAG_CASE_SENSITIVE_KEYS 0x01  // key match sensitivity. JSON default
#define JSONFLAG_RECURSIVE_SERIALIZATION 0x02

#define CAST(type, var) ((type)(var))

typedef unsigned long jszlHandle;

//typedef void * jszlstring;

/*
typedef union jszlstring {
  const char    *a;
  const wchar_t *w;
} jszlstring;
*/

typedef enum jszlopresult {
  JSZLOP_FAILED
 ,JSZLOP_SUCCESS
} jszlopresult;

// jszl initialization flags
typedef enum jszlinitopt{
  JSZLINIT_MULTITHREADED = 0x01
 ,JSZLINIT_LOGFILE       = 0x02
} jszlinitopt;

typedef enum jszlstropt {
  JSZLSTROPT_ANSI    = 0x01
 ,JSZLSTROPT_UNICODE = 0x02
} jszlstropt;


typedef enum jszle {
 JSZLE_NONE = -1 
,JSON_ERROR_SYNTAX
,JSON_ERROR_INVALID_VALUE
,JSON_ERROR_TYPE_MISMATCH //need to clean up instances of this error
,JSZLE_TYPE_MISMATCH
,JSON_ERROR_SCHEMA_ERROR
,JSZLE_DUP_KEY
,JSZLE_KEY_UNDEF
,JSZLERR_NO_FILE_EXISTS
,JSZLE_NO_FILE
,JSON_ERROR_OUT_OF_RANGE
,JSON_ERROR_CONTINUE     //fatal
,JSZLE_BAD_PARAM
,JSON_ERROR_INVALID_STRING
,JSON_ERROR_INVALID_NUMBER
,JSZLE_NO_MEMORY   //fatal
,JSON_ERROR_MUST_BE_ARRAY_OR_OBJECT
,JSON_ERROR_MAX_NESTED_NAMESPACES
,JSZLE_NO_ROOT
,JSZLE_NOT_EQUAL
,JSZLE_BAD_PATH
,JSZLE_BAD_HANDLE
,JSZLE_BAD_ERRCODE

,MAX_JSZLERR
} jszle;

typedef enum jszlopt {
  JSZLOPT_ENCODE_FORMAT
 ,JSZLOPT_READ_BUFFER
} jszlopt;

enum jszlencode {
  JSZLENCODE_UTF8
 ,JSZLENCODE_UTF16
 ,JSZLENCODE_UTF32
};

#define SET_TYPE(Type, SubType) ((jszltype)((SubType << 3) | Type))
//definitive type list
#define JSZLTYPE_NUMBER_INT8     SET_TYPE(TYPE_NUMBER, SUBTYPE_INT8)
#define JSZLTYPE_NUMBER_INT16    SET_TYPE(TYPE_NUMBER, SUBTYPE_INT16)
#define JSZLTYPE_NUMBER_INT32    SET_TYPE(TYPE_NUMBER, SUBTYPE_INT32)
#define JSZLTYPE_NUMBER_INT64    SET_TYPE(TYPE_NUMBER, SUBTYPE_INT64)
#define JSZLTYPE_NUMBER_UINT8    SET_TYPE(TYPE_NUMBER, SUBTYPE_UINT8)
#define JSZLTYPE_NUMBER_UINT16   SET_TYPE(TYPE_NUMBER, SUBTYPE_UINT16)
#define JSZLTYPE_NUMBER_UINT32   SET_TYPE(TYPE_NUMBER, SUBTYPE_UINT32)
#define JSZLTYPE_NUMBER_UINT64   SET_TYPE(TYPE_NUMBER, SUBTYPE_UINT64)
#define JSZLTYPE_NUMBER_DOUBLE   SET_TYPE(TYPE_NUMBER, SUBTYPE_DOUBLE)

#define JSZLTYPE_STRING_UTF8     SET_TYPE(TYPE_STRING, SUBTYPE_UTF8)
#define JSZLTYPE_STRING_UTF16    SET_TYPE(TYPE_STRING, SUBTYPE_UTF16)
#define JSZLTYPE_STRING_UUID     SET_TYPE(TYPE_STRING, SUBTYPE_UUID)
#define JSZLTYPE_STRING_IPV4     SET_TYPE(TYPE_STRING, SUBTYPE_IPV4)
#define JSZLTYPE_STRING_IPV6     SET_TYPE(TYPE_STRING, SUBTYPE_IPV6)
#define JSZLTYPE_STRING_EMAIL    SET_TYPE(TYPE_STRING, SUBTYPE_EMAIL)
#define JSZLTYPE_STRING_PHONE    SET_TYPE(TYPE_STRING, SUBTYPE_PHONE) //E.164 formatting
#define JSZLTYPE_STRING_DATE     SET_TYPE(TYPE_STRING, SUBTYPE_DATE)
#define JSZLTYPE_STRING_BASE64   SET_TYPE(TYPE_STRING, SUBTYPE_BASE64)
#define JSZLTYPE_STRING_BASE16   SET_TYPE(TYPE_STRING, SUBTYPE_BASE16)

#define JSZLTYPE_ARRAY_FIXED     SET_TYPE(TYPE_ARRAY, SUBTYPE_FIXED_ARRAY)

typedef unsigned short jszltype;


//accepted number formats: decimal, binary, octal, hexadecimal


enum jszltype {
  TYPE_NULL = 1
 ,TYPE_ARRAY
 ,TYPE_BOOLEAN
 ,TYPE_NUMBER
 ,TYPE_OBJECT
 ,TYPE_STRING
};

#define JSON_DESCRIPTOR_TYPE_POINTER 3


enum value_types{
  SUBTYPE_NONE
 ,SUBTYPE_INT8
 ,SUBTYPE_INT16
 ,SUBTYPE_INT32
 ,SUBTYPE_INT64
 ,SUBTYPE_UINT8
 ,SUBTYPE_UINT16
 ,SUBTYPE_UINT32
 ,SUBTYPE_UINT64
 ,SUBTYPE_UTF8
 ,SUBTYPE_UTF16
 ,SUBTYPE_EMAIL
 ,SUBTYPE_IPV4
 ,SUBTYPE_IPV6
 ,SUBTYPE_UUID
 ,SUBTYPE_FLOAT
 ,SUBTYPE_SCIENTIFIC
 ,SUBTYPE_DATE
};

//Document vs Functional Errors
//Recoverable vs Unrecoverable Errors

/*
** Field descriptor for serializing and deserializing C-style structs/objects.
** A struct must have a descriptor defined representing its memory layout if it is
** to be properly serialized.
*/

struct field_desc {
  const long offset;   /* field offset */
  const long type;     /* the field type of the object */
  union {
    unsigned long size;      /* total size of each element */
    struct field_desc *desc;
  };
  const long max_elem; /* maximum number number of elements in array */
  const char *name;    /* name of the field */
};

struct field_instance {
  struct field_desc *desc; /* the field desc that describes this field */
  long num_elem;    /* number of elements in array - must not be more than max_elem */
  long length;  /* the length of a string or number of fields in an object */
  long pval;    /* pointer to the data to be serialized */
};

/*

  struct field_desc date_desc[] = {
    {TYPE_CHAR,  1, 1, "month"}
   ,{TYPE_CHAR,  1, 1, "day"}
   ,{TYPE_SHORT, 2, 1, "year"}
  };

  struct field_desc nickname = {"nickname"};

  struct field_desc person_desc[] = {
    {TYPE_CHAR, 1, 64, "name"} 
   ,{TYPE_CHAR, 1, 1, "age"}
   ,{TYPE_OBJECT, sizeof(struct date), 1, "birthday"}
   ,{TYPE_POINTER, sizeof(void *), 4, "nicknames"}
  };

  struct Date{
    char month; 
    char day;
    short year;
  };

  struct Person{
    char name[64];
    char age;
    struct date birthday;
    char * nicknames[4];
  };

  struct Person person;

  struct field_instance birthday_instance[] = {
    {&birthday_desc[0], , , &person.birthday.month}
   ,{&birthday_desc[1], , , &person.birthday.day}
   ,{&birthday_desc[2], , , &person.birthday.year} 
  };

  struct field_instance person_instance[] = {
    {&person_desc[0], 1, 0, person.name}
   ,{&person_desc[1], , , &person.age}
   ,{&person_desc[2], , , &birthday_instance}
   ,{&person_desc[3], , , &
  };

  struct field_desc birthday_desc[] = {
    {TYPE_CHAR ,0 ,sizeof(person.birthday.month) ,"month" ,&person.birthday.month}
   ,{TYPE_CHAR ,0 ,sizeof(person.birthday.day) ,"day" ,&person.birthday.day}
   ,{TYPE_SHORT ,0 ,sizeof(person.birthday.year) ,"year" ,&person.birthday.year}
  };

  {TYPE_STRING ,64 ,"name"}
  struct field_desc person_desc[] = {
    {TYPE_STRING ,1 ,0 ,64 ,"name" ,person.name} 
   ,{TYPE_CHAR   ,1 ,0 ,1 ,"age" ,&person.age}
   ,{TYPE_OBJECT ,1 ,3 ,sizeof(person.birthday) ,"birthday" ,&birthday_desc}
   ,{TYPE_ARRAY  ,4 ,3 ,sizeof(person.nicknames) ,"nicknames" ,person.nicknames}
  };

*/

typedef struct {
  union {
    const char * ascii;
    //const wchar_t * unicode;
  };
  short length;
  short type;
} JSON_STRING;

typedef struct {
  const char *string;
  short length;
  char type;
  char subtype;
} JSON_QUERY, JSON_RESPONSE;

typedef int (*JSON_QUERY_FOREACH_CALLBACK)(
  struct jszlValue *value 
 ,void * pb
);

typedef int (*JSON_ERROR_HANDLER)(
  unsigned err
 ,const char *msg
 ,unsigned line
 ,void *pb
);


/*
** *Note* an app is not allowed to set membind without
** setting memfree (or vice versa)
*/
struct jszlvtable{
  void * (*memalloc)(void *ctx, unsigned long size, int doalloc);
  void (*errorlog)(void *ctx, jszle err, const char *msg);
};

#if defined(_WIN32) || defined(_WIN64)
  #define JSZLEXPORT __declspec(dllexport)

#elif defined(__linux__) || defined(__GNUC__)
  #define JSZLEXPORT __attribute__((visibility("default")))

#else
  #define JSZLEXPORT

#endif
  

/*
An atom is a pointer to a unique, immutable sequence of zero or more arbitrary bytes. There is only one occurence of any atom.

The atom table should exists in memory as long as the atom pool.
*/

//__FUNCTION__

/*
#define DECLARE(funcname, ...) funcname(DECLPARAMS, __VA_ARGS__)
DECLARE(jszl_test, const char *string, unsigned int len)
*/

#define jszlGlobalInit(VTABLEREF, OPTIONS)\
__jszlGlobalInit(DEBUG_PARAMS(), VTABLEREF, OPTIONS)
void __jszlGlobalInit(DECLPARAMS
                     ,struct jszlvtable *vtblref
                     ,unsigned long options);

#define jszlSetOpt(Handle, Option, ...)\
__jszlSetOpt(DEBUG_PARAMS(), Handle, Option, __VA_ARGS__)
jszle __jszlSetOpt(DECLPARAMS
                  ,jszlHandle hdl
                  ,jszlopt opt
                  ,...);


/**
* Set the encoding pattern for JSON text
* @param hdl the handle to the JSizzle context
* @param enc the encoding pattern to use
*/
#define jszlSetEncode(Handle, Encode)\
__jszlSetEncode(DEBUG_PARAMS(), Handle, Encode)
JSZLEXPORT jszle __jszlSetEncode(DECLPARAMS
                     ,jszlHandle hdl
                     ,enum jszlencode enc);


#define jszlThreadInit()\
__jszlThreadInit(DEBUG_PARAMS())
JSZLEXPORT jszlHandle __jszlThreadInit(DECLPARAMS);


#define jszlParseLocalFile(Handle, File)\
__jszlParseLocalFile(DEBUG_PARAMS(), Handle, File)
JSZLEXPORT jszle __jszlParseLocalFile(DECLPARAMS
                          ,jszlHandle handle
                          ,const char *file);


#define jszlIterate(Handle, Callback, Passback, Path)\
__jszlIterate(DEBUG_PARAMS(), Handle, Callback, Passback, Path)
JSZLEXPORT jszle __jszlIterate(DECLPARAMS
                   ,jszlHandle handle
                   ,int (*callback)(void *, int)
                   ,void *passback
                   ,const char *path);

/*
** Parse and cache a JSON document
*/
#define jszl_load(HANDLE, JSONSTR)\
__jszlRead(DEBUG_PARAMS(), HANDLE, JSONSTR)
jszle __jszl_load(DECLPARAMS
                 ,jszlHandle hdl
                 ,const char *jsonstr);



#define json_read(PSTATE, POBJECT, JSON_STR)\
__json_read(DEBUG_PARAMS(), PSTATE, POBJECT, JSON_STR)
int __json_read(DECLPARAMS
               ,struct jszlparserstate *pstate
               ,struct jszlcontext *handle
               ,const char *str
);

/*
** Set the node scope of a handle
*/

//#define jszl_setscope(handle, pathstr, stropt)
#define jszlSetScope(Handle, Path)\
__jszlSetScope(DEBUG_PARAMS(), Handle, Path)
JSZLEXPORT jszle __jszlSetScope(DECLPARAMS
                    ,jszlHandle handle
                    ,const char *pathstr);


/*
** Check to see if an object or array is set as the root
** '#' should always equal the current root
*/
#define jszlIsRoot(Handle, Path)\
__jszlIsRoot(DEBUG_PARAMS(), Handle, Path)
jszle __jszlIsRoot(DECLPARAMS
                  ,jszlHandle handle
                  ,const char *pathstr);

/*
** Query a JSON document to check is a value exist
*/
#define jszlValueExists(Handle, Path)\
__jszlValueExists(DEBUG_PARAMS(), Handle, Path)
jszle __jszlValueExists(DECLPARAMS
                       ,jszlHandle handle
                       ,const char *path);


/*
** Deserialize a string value
*/
#define jszl_deserialize_string(HANDLE, ENDPOINT, STRBUF, BUFSIZE, PSIZE)\
_jszl_deserialize_string(DEBUG_PARAMS(), HANDLE, ENDPOINT, STRBUF, BUFSIZE, PSIZE)
jszle _jszl_deserialize_string(DECLPARAMS
                              ,jszlHandle handle
                              ,const char *endpoint
                              ,char strbuf[]
                              ,int bufsize
                              ,int *psize
);

#define json_query_get_string(POBJ, PATH, STRBUF, BUFSIZE, PSIZE)\
_json_query_get_string(DEBUG_PARAMS(), POBJ, PATH, STRBUF, BUFSIZE, PSIZE)
int _json_query_get_string(DECLPARAMS
                          ,struct jszlcontext *handle
                          ,const char * path
                          ,char strbuf[]
                          ,int bufsize
                          ,int *psize
);


/*
** Deserialize a number value 
*/
#define jszl_deserialize_number(HANDLE, ENDPOINT, PNUM)\
_jszl_deserialize_number(DEBUG_PARAMS(), HANDLE, ENDPOINT, PNUM)
jszle _jszl_deserialize_number(DECLPARAMS
                              ,jszlHandle handle
                              ,const char *endpoint
                              ,long *pnum
);
#define json_query_get_number(POBJECT, PATH, PNUMBER)\
_json_query_get_number(DEBUG_PARAMS(), POBJECT, PATH, PNUMBER)
int _json_query_get_number(DECLPARAMS
                          ,struct jszlcontext *handle
                          ,const char *path
                          ,long *number
);


#define jszlDeserializeObject(Handle, Buffer, Table, Count, Path)\
__jszlDeserializeObject(DEBUG_PARAMS(), Handle, Buffer, Table, Count, Path)
jszle __jszlDeserializeObject(DECLPARAMS
                             ,jszlHandle handle
                             ,void *buffer
                             ,struct field_desc table[]
                             ,int count
                             ,const char *path);


/*
** Returns: JSON_SUCCESS JSON_ERROR_TYPE_MISMATCH
*/
#define jszl_deserialize_boolean(HANDLE, ENDPOINT, PBOOL)\
_jszl_deserialize_boolean(DEBUG_PARAMS(), HANDLE, ENDPOINT, PBOOL)
jszle _jszl_deserialize_boolean(DECLPARAMS
                               ,jszlHandle handle
                               ,const char *endpoint
                               ,long *boolean
);
#define json_query_get_boolean(POBJECT, PATH, PBOOLEAN)\
_json_query_get_boolean(DEBUG_PARAMS(), POBJECT, PATH, PBOOLEAN)
int _json_query_get_boolean(DECLPARAMS
                           ,struct jszlcontext *handle
                           ,const char * path
                           ,long *boolean 
);

/*
 * jszl_value_exists()
 * jszl_get_type(handle, 
 * jszl_deserialize(handle, type, buf, size, psize) 
 * jszl_deserialize_string()
 * jszl_deserialize_number()
 * jszlDeserializeBoolean
 * jszlValueExists
 * jszlQueryType
 * jszlQueryArrayCount
*/


/*
** Returns:
** JSON_INVALID_NODE - node pointer out of range
*/
/*
#define jszl_query_type(HANDLE, ENDPOINT, PTYPE)\
_jszl_query_type(DEBUG_PARAMS(), HANDLE, ENDPOINT, PTYPE)
jszle _jszl_query_type(DECLPARAMS
  jszlHandle hdl
 ,const char *endpoint
 ,jszltype *ptype
);
*/
#define json_query_get_type(POBJECT, PATH, PTYPE)\
_json_query_get_type(DEBUG_PARAMS(), POBJECT, PATH, PTYPE)
jszle _json_query_get_type(DECLPARAMS
                          ,struct jszlcontext *handle
                          ,const char *path
                          ,short *ptype);

#define json_query_get_object(POBJECT, PATH, PSTRUCT, PDESC_TABLE, DESC_TABLE_COUNT)\
_json_query_get_object(DEBUG_PARAMS(), POBJECT, PATH, PSTRUCT, PDESC_TABLE, DESC_TABLE_COUNT)
jszle _json_query_get_object(DECLPARAMS
                            ,struct jszlcontext *handle
                            ,const char *path
                            ,void * pstruct //pointer to a structure of data
                            ,struct field_desc desc_table[]
                            ,int count); //num elements in table

/*
**
*/
#define jszl_query_array_count(HANDLE, ENDPOINT, PCOUNT)\
_jszl_query_array_count(DEBUG_PARAMS(), HANDLE, ENDPOINT, PCOUNT)
jszle _jszl_query_array_count(DECLPARAMS
                             ,const char *pathstr 
                             ,jszlstropt stropt
                             ,unsigned long *pcount);

#define json_query_get_array_count(POBJECT, PATH, PCOUNT)\
_json_query_get_array_count(DEBUG_PARAMS(), POBJECT, PATH, PCOUNT)
jszle_json_query_get_array_count(DECLPARAMS
                                ,struct _jszlcontext *handle
                                ,const char *path
                                ,short *pcount);

/*
** Get the string error message from a given error code
*/
#define jszlGetErrMsg(ErrCode, ErrMsg)\
__jszlGetErrMsg(DEBUG_PARAMS(), ErrCode, ErrMsg)
jszle __jszlGetErrMsg(DECLPARAMS
                     ,jszle errcode
                     ,char **errmsg);


/*
** Returns:
** JSON_TYPE_MISMATCH - if the node is not a structural node
*/
/*
#define json_query_foreach(PNODE, CALLBACK, PASSBACK)\
_json_foreach(__FILE__, __LINE__, PNODE, CALLBACK, PASSBACK)
int _json_foreach(
  const char *__file__, unsigned __line__
 ,JSON_OBJECT * pobj 
 ,JSON_QUERY_FOREACH_CALLBACK cb
 ,void *pb
);
*/


#undef DECLPARAMS

#endif
