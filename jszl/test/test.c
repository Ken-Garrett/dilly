#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//#include "..\..\core.h"
#include "..\jszl.h"

#pragma comment(lib, "ws2_32.lib")

#define RUN_TEST(fn)  printf("%s: %s\n", #fn, (fn() ? "Passed" : "Failed"))

#define INI_MEM_SIZE 8192
int test_ini()
{
int success = 1;
/*
int i = 0;
char *str = 0; int len = 0;
char memory[INI_MEM_SIZE];
struct ini *cnf;

    cnf = _ini_ctor("H:\\Dev\\code\\settings.ini");    
    ini_load(cnf, memory, INI_MEM_SIZE);
    //ini_value(cnf, "RT_RCDATA", "RESID_MYDATA", &str, &len);
    
    for(i = ini_value(cnf, "INCLUDE", "PATH", &str, &len); i;
        len = 0, i = ini_next(cnf, &str, &len)) //Must make length 0
    {
        printf("%.*s\n", len, str);
    }
    ini_dtor(cnf);

*/
    return success;
}

void *Allocate(void *data, unsigned type, unsigned size, void *pb)
{
    return malloc(size);
}

int json_error_handler(unsigned err, const char *msg, unsigned line, void * pb)
{
    switch(err)
    {
    case JSON_ERROR_SYNTAX:
        printf("Error %u: Erroneous syntax (%u)\n", err, line);
        break;
    case JSON_ERROR_INVALID_VALUE:
        printf("Error %u: Invalid value (%u)\n", err, line); 
        break;
    case JSZLE_DUP_KEY:
        printf("Error %u: Key already exists (%u)\n", err, line);
        break;
    default:
        printf("Error %u: Unhandled error (%u)\n", err, line);
        break;
    }
    return 1;
}

struct fileObject {
  HANDLE hFile;
  HANDLE hFmap;
  void *map;
};

void *loadFileInMemory(const char *file, struct fileObject *fo)
{
SECURITY_ATTRIBUTES sa = {0};

    fo->hFile = CreateFileA(file,
        GENERIC_READ | GENERIC_WRITE, 0, &sa,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(INVALID_HANDLE_VALUE == fo->hFile){
        printf("Error (loadFileInMemory): Could not open file\n"); 
	return 0;
    }

    fo->hFmap = CreateFileMappingA(
        fo->hFile, 0, PAGE_READWRITE, 0, 0, 0
    );
    if(!fo->hFmap){
        printf("Error: Could not create file mapping\n"); 
	return 0;
    }

    fo->map = MapViewOfFile(fo->hFmap,
        FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
    if(!fo->map){
        printf("Error: Could not map file\n");
	return 0;
    }
    return fo->map;
}

void unloadFileFromMemory(struct fileObject *fo)
{
    UnmapViewOfFile(fo->map);
    CloseHandle(fo->hFmap);
    CloseHandle(fo->hFile);
}


#ifndef offsetof
#define offsetof(TYPE, ELEMENT) ((unsigned long)&(((TYPE *)0)->ELEMENT))
#endif

struct field_desc desc[] = {
  { offsetof(struct field_desc, size), TYPE_ARRAY, sizeof(long), 1, "size" }
};

void *membind(void *ctx, unsigned size)
{
    return malloc(size);
}

int test_read_empty_object()
{

//struct _jszlHandle handle = {0};

    jszle err;
    jszlHandle handle;
    const char * json = "{}";

    handle = jszlThreadInit();

    //msg = json_read(&state, &handle, json); 
    return 1;
}

int test_json()
{

HANDLE hfile1, hmap1, hfile2, hmap2;
const char *fmap1, *fmap2;
unsigned err;
struct fileObject fo;


//PJSON_OBJECT obj = &jobj;


    INIT_TIMER(1);

jszle  e;
jszlHandle handle;


    struct jszlvtable vt = {0};

    jszlGlobalInit(&vt, JSZLINIT_MULTITHREADED);

    handle = jszlThreadInit();



    const char *str1 = ".\\test\\test2.json";
    const char *str2 = ".\\test\\test.json";
    const char *str3 = ".\\test\\draft-07.json";
    fmap1 = loadFileInMemory(str1, &fo);


    TIME_US("\nLoad schema and apply to json\n",
        //msg = json_read(&state, &handle, fmap1); 
    );
    char buf[16];
    char jsonPath[64];
    short count;
    jszlGetArrayCount(handle, &count, "$.properties.fakeArray");
    printf("Count: %d\n", count);

    for(int i = 0; i < count; i++){
        sprintf(jsonPath, "$.properties.required[%d]", i);
        //msg = json_query_get_string(&handle, jsonPath, buf, 16, 0);  
        //if(msg == JSON_SUCCESS) printf("%s\n", buf);
    }

    //msg = json_query_path_exists(&handle, "$.properties.required[4]");
    //if(msg == JSON_SUCCESS) printf("Path exists!\n");
    //else printf("Path does not exists!\n");

    //json_query_get_boolean(&handle, "$.properties.aliases.uniqueItems", &count);
    printf("Boolean: %d\n", count);

    //jszl_thread_init(0, 0, 0);

    unloadFileFromMemory(&fo);

    return 1;
}

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))


  struct Date{
    char month; 
    char day;
    short year;
  };
  struct field_desc DateDesc[] = {
   {offsetof(struct Date, month), TYPE_NUMBER, sizeof(char), 1, "month"}
  ,{offsetof(struct Date, day), TYPE_NUMBER, sizeof(char), 1, "day"}
  ,{offsetof(struct Date, year), TYPE_NUMBER, sizeof(char), 1, "year"} 
  };


  struct Person{
    char name[64];
    char age;
    char height[8];
    struct Date birthday;
    char * nicknames[4];
  };
  struct field_desc PersonDesc[] = {
   {offsetof(struct Person, name), TYPE_STRING, 64, 1, "name"}
  ,{offsetof(struct Person, age), TYPE_NUMBER, sizeof(char), 1, "age"}
  ,{offsetof(struct Person, height), TYPE_STRING, 64, 1, "height"}
  ,{offsetof(struct Person, birthday),
	  TYPE_OBJECT, DateDesc, 1, "birthday"}
  ,{offsetof(struct Person, nicknames),
          TYPE_ARRAY,32, 4, "nicknames"}
  };


int callback(void *ptr, int type)
{
    return 0;
}



int main(int argc, char **argv)
{
    struct Person person = {0};
    char buf[256];
    //Run all test here
    
    void *passback = 0;
    
    //RUN_TEST(test_dict);
    //RUN_TEST(test_json);
    
    IN6_ADDR in6;
    if(!inet_pton(AF_INET6, "fe80::f816:3eff:fe5f:419", &in6)){
        printf("Invalid ip address\n"); 
	return 0;
    }


    struct jszlvtable vt = {0};
    jszlHandle handle;


    jszlGlobalInit(&vt, JSZLINIT_MULTITHREADED);
    handle = jszlThreadInit();

    jszlParseLocalFile(handle, "H:\\Dev\\code\\jszl\\test.json");
    //jszlParseLocalFile(handle, "H:\\Dev\\code\\jszl\\schema.json");

    jszlSetEncode(handle, JSZLENCODE_UTF8);

    jszlSetOpt(handle, JSZLOPT_ENCODE_FORMAT, "utf8");
    jszlSetOpt(handle, JSZLOPT_READ_BUFFER, buf, 256); 


    if(!jszlSetScope(handle, "#.Object1")){
        printf("Error: could not set scope 1\n");
    }

    if(!jszlIsRoot(handle, "#")){
        printf("Not root\n"); 
    }


    jszlDeserializeObject(handle, &person, PersonDesc,
        ARRAY_SIZE(PersonDesc), "$.Person");

    //jszlIterate(handle, callback, passback, "$.Array");

    printf("Name: %s\n", person.name);
    printf("Age: %u\n", person.age);
    printf("Height: %s\n", person.height);

    //jszlForEach(handle, callback, pb, "$.Person");

    //jszlSetEncoding(handle, JSZLENCODE_UTF8);

    //jszlSetUserContext(handle, ptr); 

    //0xE29880
#define UTF8_CHAR 0x8098E2

#define UTF8_CHAR_LEN(byte) (((0xE5000000 >> ((byte >> 3) & 0x1E)) & 3) +1)
    //printf("%lc : %u : %u\n", UTF8_CHAR, UTF8_CHAR, UTF8_CHAR_LEN(UTF8_CHAR));

    printf("%u\n", ((0x24 >> 3) & 0x1E));
    printf("%u\n",   ((0xE5000000 >> ((0xF8 >> 3) & 0x1E)) & 3) +1 ); //& 3) + 1));

    printf("0x%x\n", 0); 
    /*
    printf("0x%x\n", in6.u.Word[0]);
    printf("0x%x\n", in6.u.Word[1]);
    printf("0x%x\n", in6.u.Word[2]);
    printf("0x%x\n", in6.u.Word[3]);
    printf("0x%x\n", in6.u.Word[4]);
    printf("0x%x\n", in6.u.Word[5]);
    printf("0x%x\n", in6.u.Word[6]);
    printf("0x%x\n", in6.u.Word[7]);
    */

    return 0;
}
