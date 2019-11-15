
unsigned atouint(const char *str, unsigned len)
{
unsigned c;
unsigned n = 0;
    for(c = *str; len--; c = *++str)
        n = ((n<<3) + (n<<1)) + (c - '0'); //n*10 + c-32
    return n;
}

unsigned hextoint(const char *str, unsigned len)
{
unsigned c = *str;
unsigned n = 0;
    for(c = *str; len--; c = *++str)
        n = (n<<4) + (c > '9' ? CHAR_TOUPPER(c) - 7 : c) - '0'; //n*16
    return n;
}


/**
 * @param
 * @return
**/
unsigned validate_phonenumber(const char *str)
{
return 0;
}

/*
** Converts an escape sequence to unicode character representation
*/
unsigned get_escaped_char(const char *str, int *buf)
{
int i;
    switch(*str){
        case '"':  *buf = '"';  return 1;
        case '\\': *buf = '\\'; return 1;
        case '/':  *buf = '/';  return 1;
        case 'b':  *buf = 0x8;  return 1;
        case 'f':  *buf = 0xC;  return 1;
        case 'n':  *buf = 0xA;  return 1;
        case 'r':  *buf = 0xD;  return 1; //carriage return
        case 't':  *buf = 0x9;  return 1; //horizonatal tab
        case 'x': //handle hex control chars
            if(*++str == '\0'); //EOF
            for(i = 0; i < 2; i++){
                if(str[i] == '\0'); //EOF
                if(!IS_HEX_DIGIT( str[i] )); //ERROR
	    }
	    i = hextoint(str, 2);
            *buf = i;
            return 2;
        case 'u': {//handle all 4 digit hex chars
            if(*++str == '\0') ; //EOF
            for(i = 0; i < 4; i++){
                if(str[i] == '\0'); //EOF
                if(!IS_HEX_DIGIT( str[i]) ); //ERROR
            }
            i = hextoint(str, 4);
            *buf = i;
            return 5;
        }
        case '\0': return 0; //EOF
        default: return 0;//ERROR
    }
}

#define HASH_DJB2(R, SEED, CHAR)\
 R = (((SEED << 5) + SEED) + CHAR_TOLOWER(c))

#define HASHING_ALGORITHM(R, SEED, CHAR) HASH_DJB2(R, SEED, CHAR)

/**
 * @param
 * @return
**/
unsigned is_valid_string(const char *str, unsigned long seed, unsigned long *hash)
{
unsigned c;
int buf;
const char *s = str;
    while(*s != '"'){
        if(*s == '\\'){
            s++;
            s += get_escaped_char(s, &buf); 
	    //printf("CHAR %c|\n", buf);
	    HASH_DJB2(seed, seed, buf);
        }
	else {
            c = *s;
            HASH_DJB2(seed, seed, c);
            s++;
	}
    }
    *hash = seed;
    return s - str + 1;
}

/*
if(IS_CHAR(c) || '_') loc++;
if(IS_CHAR(c) || IS_DEC_DIGIT(c) || '_')

long is_valid_strict_key()
{
    if(!IS_CHAR(c) && (c != '_') && (c != '$')) //error
    do {
        //hash char
        loc++;
    } while(IS_CHAR(c) || IS_DEC_DIGIT(c) || c = '_')
}
*/

long is_valid_key(const char *str, unsigned long seed, unsigned long *hash)
{
const char *s = str;
unsigned c = *s;
    if(!IS_CHAR(c) && (c != '_') && (c != '$')) return 0;
    do{
        HASH_DJB2(seed, seed, c);
        c = *++s;
    } while(IS_CHAR(c) || IS_DEC_DIGIT(c) || c == '_' || c == '$');
    *hash = seed;
    return s - str;
}

/**
 * @param
 * @return
**/
unsigned validate_uuid(const char *start)
{
const char *str = start;
short i;
    if(*str == '{') str++;
    for(i = 8; i; i--){
        if(!IS_HEX_DIGIT(*str)) return 0;
        str++;
    }
    if(*str != '-') return 0;
    str++;
    for(i = 4; i; i--){
        if(!IS_HEX_DIGIT(*str)) return 0;
        str++;
    }
    if(*str != '-') return 0;
    str++;
    for(i = 4; i; i--){
        if(!IS_HEX_DIGIT(*str)) return 0;
        str++;
    }
    if(*str != '-') return 0;
    str++;
    for(i = 4; i; i--){
        if(!IS_HEX_DIGIT(*str)) return 0;
        str++;
    }
    if(*str != '-') return 0;
    str++;
    for(i = 12; i; i--){
        if(!IS_HEX_DIGIT(*str)) return 0;
        str++;
    }
    if(*str == '}') str++;
return str - start;
}

/**
 * @param
 * @param ipv6 returns the ipv6 address in binary network byte order
 * @return
 * Leading zeros in an octet wil cause an error
 * TODO Detect invalid on trailing digits, right now it confirms as valid addr
 * TODO detect leading zeroes in a hextet
 * TODO refactor to remove duplication
 * TODO handle multiple long running zero hextets.
 * TODO optimize memset to set 1 less zero since first zero is already set?
**/

#define IS_IPV6_DIGIT(c) (IS_DEC_DIGIT(c) || _BETWEEN_('a', c, 'f'))
unsigned validate_ipv6(const char *start, unsigned *err, union ipv6 *ipv6)
{
#define IPV6_TRUNCATED 0x01
#define IPV6_BRACKETED 0x02
const char *str = start;
char i, j;
short z, y = 0;
unsigned flags = 0;
    //if(*str == '[') { flags &= IPV6_BRACKETED; str++; }
    for(i = 0; i < 7; i++)
    {
        for(j = 0; j < 4; j++){
            if(!IS_IPV6_DIGIT(*str)) break;
            //if(j == 4 && *str == '0' && IS_IPV6_DIGIT(str[1])) return 0;
            str++;
        }
        z = hextoint(str-j, j);
        if(__LITTLE_ENDIAN__) z = ENDIAN16(z);
        *(((short*)ipv6)+i) = z;
        if(*str != ':'){ //should be colon
            if(flags & IPV6_TRUNCATED){
                MEM_MOVE( (((short*)ipv6)+(8-(i-y))),
                         (((short*)ipv6)+(y+1)),
                         sizeof(short)*(i-y) );  
                MEM_SET( (((short*)ipv6)+y), 0, sizeof(short)*(8-i) );
                return str - start;
            }
            else return 0;
        }
        else if(*(str-1) == ':'){
            if(flags & IPV6_TRUNCATED) return 0;
            else{
                flags |= IPV6_TRUNCATED;
                y = i; //mark truncated hextet
            }
        }
        str++;
    }
    for(j = 0; j < 4; j++)
    {
        if(!IS_IPV6_DIGIT(*str)) break;
        str++;
    }
    z = hextoint(str-j, j);
    if(__LITTLE_ENDIAN__) z = ENDIAN16(z);
    *(((short*)ipv6)+i) = z;
    return str - start;
}

/**
 * @param
 * @param addr returns the 32 bit ip address in big-endian format
 * @return
 * TODO refactor to remove duplication
 * TODO cleanup port handling. do we even want it?
 * TODO detect leading zeroes in an octet
**/
unsigned validate_ipv4(const char *start, unsigned *err, unsigned *addr)
{
const char *str = start;
short i, j;
unsigned x = 0;

    //printf("Validating IPv4 Address\n");
    for(i = 3; i; i--){
        for(j = 0; j < 3; j++){
            if(!IS_DEC_DIGIT(*str)){
                if(j == 0){
                    if(err) *err = 1;
                    return 0; //need at least 1 digit
                }
                break;
            }
            str++;
        }
        if(*str != '.') return 0; //should be decimal
            j = atouint(str-j, j);
        if(!_BETWEEN_(0, j, 255)) return 0;
            x |= j << (i<<3);
        str++;
    }
    for(j = 0; j < 3; j++){
        if(!IS_DEC_DIGIT(*str)){
            if(j == 0) return 0;
            break;
        }
        str++;
    }
    j = atouint(str-j, j);
    if(!_BETWEEN_(0, j, 255)) return 0;
    x |= j << (i<<3);
    *addr = x;

    //port handling
    if(*str == ':'){
        str++;
        for(j = 5; j; j--){
            if(!IS_DEC_DIGIT(*str)){
                if(j == 5) return 0;
                break;
            }
        str++;
        }
    }
    return str - start;
}

enum NumberType {
  _NumberTypeHex = 1
 ,_NumberTypeDecimal
 ,_NumberTypeOctal
 ,_NumberTypeFloat
 ,_NumberTypeDouble
 ,_NumberTypeScientific
};

/**
 * @param
 * @return
 * TODO fix incomplete exponent validation
**/
unsigned is_valid_number(const char *start, unsigned *numtype, unsigned *isNegative)
{
const char *str = start;
char flags = 0;

    *numtype = _NumberTypeDecimal;
    *isNegative = 0;

    if(*str == '-'){
        *isNegative = 1;
        str++;
    }
    if(str[0] == '0'){
        if(str[1] == 'x' || str[1] == 'X'){ //hex
            str+=2;
            //if(!IS_DEC_DIGIT(*str) || _BETWEEN_('a', *str, 'f') || _BETWEEN_('A', *str, 'F') ) return 0;
            while(IS_DEC_DIGIT(*str) ||
            _BETWEEN_('a', *str, 'f') ||
            _BETWEEN_('A', *str, 'F'))
                str++;
            *numtype = _NumberTypeHex;
        }
        else if(BETWEEN_('0', str[1], '7')){ //octal
            str+=2;
            while(_BETWEEN_('0', *str, '7')) str++;
	    *numtype = _NumberTypeOctal;
        }
        else return 1;
    }
    else if(BETWEEN_('0', *str, '9')){ //decimal
        while(IS_DEC_DIGIT(*str)) str++;
        if(*str == '.'){ //floating point
            *numtype = _NumberTypeDouble;
            str++;
            while(IS_DEC_DIGIT(*str)) str++;
            if(*str == 'e' || *str == 'E'){
                *numtype = _NumberTypeScientific;
                str++;
                if(*str == '+'){
                    str++;
                }
                else if(*str == '-'){
                    str++;	
                }
                if(!IS_DEC_DIGIT(*str)) return 0;
                while(IS_DEC_DIGIT(*str)) str++;
            }
        }
        else if(*str == 'e' || *str == 'E'){ //exponent
            *numtype = _NumberTypeScientific;
            str++;
            if(*str == '+'){
                str++;
            }
            if(*str == '-'){
                str++; 
            }
            if(!IS_DEC_DIGIT(*str)) return 0;
            while(IS_DEC_DIGIT(*str)) str++;
        }
    }
    else return 0;

    return str - start;
}

#define DATE_MMDDYYYY
#define DATE_DDMMYYYY
#define DATE_YYYYDDMM
#define DATE_YYYYMMDD

#define IS_NUMERIC(x) ((x) >= '0' && (x) <= '9')

GetValidDate_MMDDYYY(const char *datestr)
{
const char *str = datestr;
int i = 0;

MONTH:
    while(IS_NUMERIC(*str)){
        if(i == 2) return 0;
        str++;
        i++;       
    }
    if(i == 0) return 0;
    if(*str++ != '/') return 0;
DAY:
    i = 0;
    while(IS_NUMERIC(*str)){
        if(i == 2) return 0;
        str++;
        i++;	
    }
    if(i == 0) return 0;
    if(*str++ != '/') return 0;
YEAR:
    i = 0;
    while(IS_NUMERIC(*str)){
        if(i == 4) break;
        str++;	
	i++;
    }
    if(i != 4) return 0;
    return str - datestr;
}


/**
 *  @param
 *  @return
**/
unsigned validate_date(const char *start)
{
return 0;
}

#define MAX_SCHEME_SIZE 7


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

union ipv4
{
unsigned int dw1;
};


struct uri {
unsigned scheme;
    union {union ipv6 ipv6; union ipv4 ipv4; };
unsigned short port;
};

unsigned validate_uri(const char *uri)
{
const char *str = uri;
struct uri uri_s;
char i = 0;
unsigned hash_val;
unsigned long hashes[] = {0};
    
    while((uri[i] != ':') && (i < MAX_SCHEME_SIZE)) i++;
    if(uri[i] != ':') return 0;

    str = uri + i;
    for(hash_val = djb2(uri, i), i = 0; hashes[i] != hash_val; i++);  
    uri_s.scheme = i;
    //if()
    return 0;
}
