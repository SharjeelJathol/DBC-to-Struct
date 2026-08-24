#include <stdio.h>
#include <stdlib.h>

enum keywords{
    NONE,
    B,
    BO,
    BO_,
    S,
    SG,
    SG_,
};

// ./a.out <filename>
int main(int argc, char *argv[]){
    // Must provide a filename for execution
    if(argc < 2){
        perror("Please provide a filename");
        return -1;
    }

    FILE *dbc_file = fopen(argv[1], "r");

    if(dbc_file == NULL){
        perror("Please provide a valid filename");
        return -1;
    }

    char character;
    character = fgetc(dbc_file);
    enum keywords line = NONE;

    while(character != EOF){
        if(character == 'B' && line == NONE)
            line = B;
        else if(character == 'O' && line == B)
            line = BO;
        else if(character == '_' && line == BO){
            line = BO_;
            printf("BO_");
        }
        else if(character == '\n' && (line == BO_ || line == SG_)){
            line = NONE;
            printf("%c", character);
        }
        else if(character == 'S' && line == NONE)
            line = S;
        else if(character == 'G' && line == S)
            line = SG;
        else if(character == '_' && line == SG){
            line = SG_;
            printf("SG_");
        }
        else if(line == S || line == SG || line == B || line == BO)
            line = NONE;

        if(line == BO_ || line == SG_)
            printf("%c", character);
        character = fgetc(dbc_file);
    }

    printf("\n");

    fclose(dbc_file);
    return 0;
}