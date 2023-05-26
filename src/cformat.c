#include <cformat.h>
#include <string.h>
#include <stack/stack.h>

#define MAX_STATES 241
#define LINEBUF_SIZE 241
#define MAX_STR_LEN 80
/** Список задач:
 * читать файл посимвольно                
 * все открывающие скобки добавлять на стек
 * все закрывающие скобки проверять на соответствие лежащей на стеке 
 * 
**/

static int cf_get_indent_levels(struct stack* stk) {
    int indent_levels = 0; 
    for (int i = 0; i < stack_size(stk); i++)
        if (stk->a[i] == '{') indent_levels++;
    return indent_levels;
}

/* static int cf_stack_checkpop(struct stack* stk, char sym) {
    char testsym;
    if (sym == ')') testsym == '(';
    if (sym == ']') testsym == '[';
    if (sym == '{') testsym == '}';
    return stk->a[stack_size(stk)] == testsym;
} */

/** обработка команд препроцессора
 * всё кроме #define (1 или 2 аргумента) однозначно
 * решение: разделитель (\n или # (начало другой команды препроцессора))
*/
/** чтение файла
 * задача - только расставить табуляцию 
 * осуществляется посимвольно до встречи некоторого из разделителей (";{}")
 * \n игнорируется
*/


static int cf_handlestate(char c, struct stack* stk) {
    const char* open_brackets = "{[(";
    const char* close_brackets = "}])"; 
    char str[2] = {c, '\0'};
    if (strspn(str, open_brackets)) 
        stack_push(stk, (int)c);
    if (strspn(str, close_brackets)){
        char prior = (char)stack_pop(stk);
        if ((prior == '{' && c == '}')
        || (prior == '(' && c == ')')
        || (prior == '[' && c == ']')) 
            return 0;
        else {
            stack_push(stk, prior);
            return -1;
        }
    }
    return 0;  
}

static int cf_isbranchcycle(char* linebuf){
    /* читать с предпоследнего символа, игнорируя пробелы
     ) - ищем до (, затем до буквы 
     <буква> - читаем до пробела
     циклы, ветвления: do, while, if, else, for
     любой другой случай - переносим скобку
    */
    char mirrorbuf[128] = {'\0'};
    struct stack* state = stack_create(8);   
    for (int i = strlen(linebuf)-1; i >= 0; i--) {
        if (linebuf[i] == ' ') {
            if (!strlen(mirrorbuf)) continue;
            else break;
        }
        if (linebuf[i] == ')') stack_push(state, ')');
        if(!stack_size(state)) 
            mirrorbuf[strlen(mirrorbuf)] = linebuf[i];
        if (linebuf[i] == '(') stack_pop(state);
    }

    char opbuf[128] = {'\0'};
    for (size_t i = 0; i < strlen(mirrorbuf); i++)
        opbuf[i] = mirrorbuf[strlen(mirrorbuf)-1-i];

    char* branchescycles[] = 
        {"if", "for", "else", "do", "while"};
    for (int i = 0; i < 5; i++) {
        if (!strcmp(opbuf, branchescycles[i])) {
            stack_free(state);
            return 1;
        }
    }
    stack_free(state);
    return 0;
}

static void print_split(char* str, int tabs){
    int len = (strlen(str) + tabs*4);
    if (len < 80) {
        
        for (int i = 0; i < tabs; i++) printf("    ");
        printf("%s\n", str);
        return;
    }
    
    char* closest = str;
    for (size_t i = 0; i < strlen(str); i++) {
        if ((i + (tabs*4)) > MAX_STR_LEN && closest != str)
            break;
        if (str[i] == ' ') closest = str + i;  
    }
    char tempclosest = closest[0];
    closest[0] = '\0';
    for (int i = 0; i < tabs; i++) printf("    ");
    printf("%s\n", str);
    closest[0] = tempclosest;
    if ((strlen(closest) + (tabs+1)*4) > MAX_STR_LEN)
        print_split(closest, tabs+1);
    else
    for (int i = 0; i < tabs+1; i++) printf("\t");
    printf("%s\n", closest);
}
 
static void cf_clear(char* linebuf){
    size_t len = strlen(linebuf);
    for (size_t i = 0; i < len; i++)
        linebuf[i] = '\0';
}

int cf_format(FILE* file) { 
    struct stack* state = stack_create(MAX_STATES);
    char linebuf[LINEBUF_SIZE] = {'\0'};
    char c = '\0';
    int stringmode = 0;

    while (fread(&c, 1, 1, file)) {
        // except for when we're inside the string
        size_t len = strlen(linebuf);
        if (c == '"' && linebuf[strlen(linebuf)-1] != '\\') 
            stringmode = !stringmode;
        if (stringmode) {
            linebuf[strlen(linebuf)] = c; 
            continue;
        }

        // skip useless spaces and \n's
        len = strlen(linebuf);
        if ((c=='\n')||((c == ' ') && (!len || linebuf[len - 1] == ' '))) 
            continue;
        
        // move braces
        if (c == '{'){
            if (!cf_isbranchcycle(linebuf)) 
                linebuf[strlen(linebuf)] = '\n';
            else 
                linebuf[strlen(linebuf)] = ' ';
        }

        // tabulate
        linebuf[strlen(linebuf)] = c;
        if (strspn(&c, ";{")) {
            int tabcount = 0;
            for (int i = 0; i < cf_get_indent_levels(state); i++) {
                tabcount++;
            }
            print_split(linebuf,tabcount);
            cf_clear(linebuf);
        }
        if (strspn(&c, "}")) {
            int tabcount = 0;
            for (int i = 0; i < cf_get_indent_levels(state)-1; i++) {
                tabcount++;
            }
            print_split(linebuf,tabcount);
            //cf_operator_format(linebuf, 0);
            cf_clear(linebuf);
        }
        if (cf_handlestate(c, state)) {
            stack_free(state); 
            return -1;
        }
    }
    stack_free(state);
    return 0;
}
