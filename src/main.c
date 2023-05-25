#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <cformat.h>
#include <stack/stack.h>


int main (int argc, char* argv[]) {
    // (Only) argument?
    if (argc != 2) {
        printf("cformat -- code formatter\nUsage: cformat <filename>\n");
        return -1; 
    }
  
    // Can we open the file?
    FILE* input; 
    if (!(input = fopen(argv[1], "r"))){
        printf("Could not open \'%s\': %s\n", argv[1], strerror(errno));
        return -1;
    }

    // Process the file!
    int err = cf_format(input);
    if (err == -1) {
        char c;
        fseek(input, -1, SEEK_CUR);
        fread(&c, 1, 1, input);
        printf("=== error: unexpected closing bracket '%c' ===\n", c);
    }
    return 0;
}
