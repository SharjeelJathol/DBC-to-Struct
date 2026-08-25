#include <stdio.h>
#include <stdlib.h>

enum keywords{
    NONE,
    B,
    BO,
    BO_,
    S,
    SG,
    SG_
};

enum line_type {
    NEWLINE,
    MESSAGE,
    SIGNAL,
    REJECT
};

enum metadata {
    MSG_ID,
    MSG_NAME,
    MSG_SIZE,
    SENDER_NAME
};

// <message_id> <message_name>: <message_size> <sender_name>
struct msg_metadata {
    int message_id;             //atoi()
    char message_name[100];
    size_t message_size;        //atoi()
    char sender_name[100];
};

// Print msg_metadata struct
void print_msg_metadata(struct msg_metadata *metadata){
    printf("Message ID: %d\n", metadata->message_id);
    printf("Message Name: %s\n", metadata->message_name);
    printf("Message Size: %zu\n", metadata->message_size);
    printf("Sender Name: %s\n", metadata->sender_name);
}

void parse_by_character(char *filename){
    FILE *dbc_file = fopen(filename, "r");

    if(dbc_file == NULL){
        perror("Please provide a valid filename");
        return -1;
    }

    char character;
    character = fgetc(dbc_file);
    enum keywords line = NONE;
    enum line_type type = NEWLINE; 

    int count = 0;

    while(character != EOF){
        // if(line == BO_ && character != '\n'){
        //     if(character == ' ') {
        //         break;
        //     }

        //     if(line == SENDER_NAME){}
        //     else if(line == MSG_SIZE){}
        //     else if(line == MSG_NAME){}
        //     else if(line == MSG_ID){}
        //     // else
        // }
        // if(character == '\n'){
        //     line = NONE;
        //     type = NEWLINE;

        // }

        if(line == BO_ || line == SG_){
            printf("%c", character);
        }
        
        if(character == '\n'){
            line = NONE;
            type = NEWLINE;
        }
        // else if(line == BO_ || line == SG_){
        //     printf("%c", character);
        // }
        else if(type == NEWLINE && line == NONE && (character != 'B' || character == '\t')){
            // type = REJECT;  // maybe later set it to SIGNAL
            type = SIGNAL;
        }
        else if(character == 'B' && line == NONE && type == NEWLINE){
            line = B;
            type = MESSAGE;
        }
        else if(character == 'O' && line == B && type == MESSAGE ){
            line = BO;
        }
        else if(character == '_' && line == BO && type == MESSAGE){
            line = BO_;
            printf("BO_");
        }

        // else if(character == '\n' && (line == BO_)){// || line == SG_)){
        //     line = NONE;
        //     type = NEWLINE;
        //     printf("%c", character);
        // }

        else if(character == 'S' && line == NONE && type == SIGNAL){
            line = S;
            type = SIGNAL;
        }
        else if(character == 'G' && line == S && type == SIGNAL){
            line = SG;
        }
        else if(character == '_' && line == SG && type == SIGNAL){
            line = SG_;
            printf("SG_");
        }
        else if(line == B || line == BO || line == S || line == SG || type == SIGNAL){
            // line = NONE;
            type = REJECT;
            // type = NEWLINE;
        }
        // else if(line == S || line == SG || line == B || line == BO){
        //     line = NONE;
        //     type = NEWLINE;
        // }
        
        character = fgetc(dbc_file);
    }

    printf("\n");

    fclose(dbc_file);
}

// ./a.out <filename>
int main(int argc, char *argv[]){
    // Must provide a filename for execution
    if(argc < 2){
        perror("Please provide a filename");
        return -1;
    }

    parse_by_character(argv[1]);


    return 0;
}