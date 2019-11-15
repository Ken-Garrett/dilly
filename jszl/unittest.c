#include <windows.h>
#include <stdio.h>

#define FG_BLACK           0
#define FG_DARK_BLUE       FOREGROUND_BLUE
#define FG_DARK_GREEN      FOREGROUND_GREEN
#define FG_DARK_CYAN       FOREGROUND_GREEN|FOREGROUND_BLUE
#define FG_DARK_RED        FOREGROUND_RED
#define FG_DARK_MAGENTA    FOREGROUND_RED|FOREGROUND_BLUE
#define FG_DARK_YELLOW     FOREGROUND_RED|FOREGROUND_GREEN
#define FG_DARK_GRAY       FOREGROUND_INTENSITY
#define FG_BRIGHT_GRAY     FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE
#define FG_BRIGHT_BLUE     FOREGROUND_BLUE|FOREGROUND_INTENSITY
#define FG_BRIGHT_GREEN    FOREGROUND_GREEN|FOREGROUND_INTENSITY
#define FG_BRIGHT_CYAN     FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY
#define FG_BRIGHT_RED      FOREGROUND_RED|FOREGROUND_INTENSITY
#define FG_BRIGHT_MAGENTA  FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY
#define FG_BRIGHT_YELLOW   FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY
#define FG_WHITE           FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY

#define BG_BLACK           0
#define BG_DARK_BLUE       BACKGROUND_BLUE
#define BG_DARK_GREEN      BACKGROUND_GREEN
#define BG_DARK_CYAN       BACKGROUND_GREEN|FOREGROUND_BLUE
#define BG_DARK_RED        BACKGROUND_RED
#define BG_DARK_MAGENTA    BACKGROUND_RED|BACKGROUND_BLUE
#define BG_DARK_YELLOW     BACKGROUND_RED|BACKGROUND_GREEN
#define BG_DARK_GRAY       BACKGROUND_INTENSITY
#define BG_BRIGHT_GRAY     BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE
#define BG_BRIGHT_BLUE     BACKGROUND_BLUE|BACKGROUND_BLUE
#define BG_BRIGHT_GREEN    BACKGROUND_GREEN|BACKGROUND_INTENSITY
#define BG_BRIGHT_CYAN     BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY
#define BG_BRIGHT_RED      BACKGROUND_RED|BACKGROUND_INTENSITY
#define BG_BRIGHT_MAGENTA  BACKGROUND_RED|BACKGROUND_BLUE|BACKGROUND_INTENSITY
#define BG_BRIGHT_YELLOW   BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_INTENSITY
#define BG_WHITE           BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY


#define fg_black "\e[30m"
#define fg_red   "\e[31m"

typedef enum  ConsoleColor {
  TERMCOLOR_DEFAULT
 ,TERMCOLOR_BLACK
 ,TERMCOLOR_BLUE
 ,TERMCOLOR_GREEN
 ,TERMCOLOR_CYAN
 ,TERMCOLOR_RED
 ,TERMCOLOR_MAGENTA
 ,TERMCOLOR_YELLOW
 ,TERMCOLOR_GRAY
 ,TERMCOLOR_LIGHTGRAY
 ,TERMCOLOR_LIGHTBLUE
 ,TERMCOLOR_LIGHTGREEN
 ,TERMCOLOR_LIGHTCYAN
 ,TERMCOLOR_LIGHTRED
 ,TERMCOLOR_LIGHTMAGENTA
 ,TERMCOLOR_LIGHTYELLOW
 ,TERMCOLOR_WHITE
} ConsoleColor;

#define TERMATTR_BOLD      0x01
#define TERMATTR_REVERSE   0x02
#define TERMATTR_BLINK     0x04
#define TERMATTR_UNDERLINE 0x08


#define BACKGROUND_COLOR_BITS (BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY)

#define FOREGROUND_COLOR_BITS (FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY)



unsigned long getbgcolor(enum ConsoleColor clr);
unsigned long getfgcolor(enum ConsoleColor clr);
void layerOS_Console_WriteLine(const char *string){}

struct console{
  CONSOLE_SCREEN_BUFFER_INFO bufInfo;
  HANDLE out;
};

struct console_text_attributes {
  enum ConsoleColor bgcolor  : 8;
  enum ConsoleColor fgcolor  : 8;
  char reverse               : 1;
  char bold                  : 1;
  char underline             : 1;
};

void console_init(struct console *cons)
{
    cons->out = GetStdHandle(STD_OUTPUT_HANDLE);
    //GetConsoleScreenBufferInfo(cons->outHandle, &cons->bufInfo;
}

void clear_screen(struct console *cons)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD numWritten;
    COORD start = {0, 0};

    if(!GetConsoleScreenBufferInfo(cons->out, &csbi))
    {
        return; 
    }

    //FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), csbi.wAttributes, 

    if(!FillConsoleOutputCharacter(cons->out, ' ',
        csbi.dwSize.X * csbi.dwSize.Y, start, &numWritten))
    {
        return; 
    }

    SetConsoleCursorPosition(cons->out, start);
}

void console_write(struct console *cons,
	struct console_text_attributes *cta, char *string)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    CONSOLE_CURSOR_INFO cci;
    WORD attr = 0;
    DWORD numWritten;
    unsigned int len;
    COORD start = {0, 8000};

    if(!GetConsoleScreenBufferInfo(cons->out, &csbi))
    {
        return;
    }

    printf("bgcolor %u\n", cta->bgcolor);
    printf("fgcolor %u\n", cta->fgcolor);

    if(cta->bgcolor == TERMCOLOR_DEFAULT) attr |= csbi.wAttributes;
    attr = getbgcolor(cta->bgcolor) | getfgcolor(cta->fgcolor);
    if(cta->reverse) attr |= COMMON_LVB_REVERSE_VIDEO;
    if(cta->underline) attr |= COMMON_LVB_UNDERSCORE;
    SetConsoleTextAttribute(cons->out, attr);
    len = strlen(string);
    WriteConsole(cons->out, string, len, &numWritten, 0);
    GetConsoleCursorInfo(cons->out, &cci);
    FillConsoleOutputCharacter(cons->out, ' ', csbi.dwSize.X - len, start, &numWritten);   
    SetConsoleTextAttribute(cons->out, csbi.wAttributes);
}

void console_write_line(struct console *cons)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if(!GetConsoleScreenBufferInfo(cons->out, &csbi))
    {
    
    }
}

unsigned long getbgcolor(enum ConsoleColor color)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    switch(color){
    case TERMCOLOR_BLACK:
        return 0;
    case TERMCOLOR_BLUE:
        return BACKGROUND_BLUE; 
    case TERMCOLOR_GREEN:
        return BACKGROUND_GREEN;
    case TERMCOLOR_CYAN:
        return BACKGROUND_GREEN|BACKGROUND_BLUE;
    case TERMCOLOR_RED:
        return BACKGROUND_RED;
    case TERMCOLOR_MAGENTA:
        return BACKGROUND_RED|BACKGROUND_BLUE;
    case TERMCOLOR_YELLOW:
        return BACKGROUND_RED|BACKGROUND_GREEN;
    case TERMCOLOR_GRAY:
        return BACKGROUND_INTENSITY;
    case TERMCOLOR_LIGHTGRAY:
        return BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE;
    case TERMCOLOR_LIGHTBLUE:
        return BACKGROUND_BLUE|BACKGROUND_INTENSITY;
    case TERMCOLOR_LIGHTGREEN:
        return BACKGROUND_GREEN|BACKGROUND_INTENSITY;
    case TERMCOLOR_LIGHTCYAN:
        return BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
    case TERMCOLOR_LIGHTRED:
        return BACKGROUND_RED|BACKGROUND_INTENSITY;
    case TERMCOLOR_LIGHTMAGENTA:
        return BACKGROUND_RED|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
    case TERMCOLOR_LIGHTYELLOW:
        return BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_INTENSITY;
    case TERMCOLOR_WHITE:
        return BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
    default:
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.wAttributes & (BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY);

    }
}

unsigned long getfgcolor(enum ConsoleColor color)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    switch(color){
    case TERMCOLOR_BLACK:
        return 0;
    case TERMCOLOR_BLUE:
        return FOREGROUND_BLUE;
    case TERMCOLOR_GREEN:
        return FOREGROUND_GREEN;
    case TERMCOLOR_CYAN:
        return FOREGROUND_GREEN|FOREGROUND_BLUE;
    case TERMCOLOR_RED:
        return FOREGROUND_RED;
    case TERMCOLOR_MAGENTA:
        return FOREGROUND_RED|FOREGROUND_BLUE;
    case TERMCOLOR_YELLOW:
        return FOREGROUND_RED|FOREGROUND_GREEN;
    case TERMCOLOR_GRAY:
        return FOREGROUND_INTENSITY;
    case TERMCOLOR_LIGHTGRAY: 
        return FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE;
    case TERMCOLOR_LIGHTBLUE:
        return FOREGROUND_BLUE|FOREGROUND_INTENSITY;
    case TERMCOLOR_LIGHTGREEN:
        return FOREGROUND_GREEN|FOREGROUND_INTENSITY;
    case TERMCOLOR_LIGHTCYAN:
        return FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY;
    case TERMCOLOR_LIGHTRED:
        return FOREGROUND_RED|FOREGROUND_INTENSITY;
    case TERMCOLOR_LIGHTMAGENTA:
        return FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY;
    case TERMCOLOR_LIGHTYELLOW:
        return FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
    case TERMCOLOR_WHITE:
        return FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY;
    default:
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.wAttributes & (FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);


        return 0;
    }
}

void change_background_color(struct console *cons, enum ConsoleColor color)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD numWritten;
    COORD start = {0, 0};
    WORD attr;

    if(!GetConsoleScreenBufferInfo(cons->out, &csbi))
    {
        return; 
    }

    csbi.wAttributes &= ~BACKGROUND_COLOR_BITS;
    printf("0x%x\n", csbi.wAttributes);
    csbi.wAttributes |= getbgcolor(color);

    if(!FillConsoleOutputAttribute(cons->out, csbi.wAttributes,
        csbi.dwSize.X * csbi.dwSize.Y, start, &numWritten)) 
    {
        return; 
    }
    SetConsoleTextAttribute(cons->out, csbi.wAttributes);
}

HANDLE outHandle;

void layerOS_Console_Write(const char *string, WORD attr)
{
    DWORD numWritten;
    CONSOLE_SCREEN_BUFFER_INFO bufInfo;

    outHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(outHandle, &bufInfo); 
    if(!attr) attr = bufInfo.wAttributes;
    SetConsoleTextAttribute(outHandle, attr); 
    WriteConsole(outHandle, string, strlen(string), &numWritten, 0);
    SetConsoleTextAttribute(outHandle, bufInfo.wAttributes);
}

typedef int (*testfn)(void *testctx, void *appctx);

struct test_descriptor {
    const char *description;
    testfn fn;
};

void runtest(struct test_descriptor *desc, int num)
{
    //struct 
    for(int i = 0; i < num; i++)
    {
        //desc[i].fn();
    }
}

void set_default_attributes(struct console_text_attributes *textattr)
{
    char fgcolor, bgcolor;
    WORD attr = 0;

    fgcolor = textattr->bgcolor;
    bgcolor = textattr->fgcolor;
}

int main(int argc, char *argv[])
{
    struct console cons;
    struct console_text_attributes cta;
    //th.outHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    WORD attr;

    cons.out = GetStdHandle(STD_OUTPUT_HANDLE);

    //change_background_color(&cons, TERMCOLOR_BLACK);

    //clear_screen(&cons);

    cta.bgcolor = TERMCOLOR_BLACK;
    cta.fgcolor = TERMCOLOR_RED;
    cta.reverse = 1;
    cta.underline = 0;
    console_write(&cons, &cta, "Hello World\n");

    printf("Sizeof %u\n", sizeof(struct console_text_attributes));
}

