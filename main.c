#include <stdio.h>
#include <stdlib.h>

#define MSG_NAME_SIZE       50
#define SENDER_NAME_SIZE    50


enum line_type {
    NEWLINE,
    MESSAGE,
    SIGNAL,
};

// <message_id> <message_name>: <message_size> <sender_name>
struct msg_metadata {
    int message_id;             //atoi()
    char message_name[MSG_NAME_SIZE];
    int message_size;           //atoi()
    char sender_name[SENDER_NAME_SIZE];
};

// Print msg_metadata struct
void print_msg_metadata(struct msg_metadata *metadata){
    printf("Message ID: %d\n", metadata->message_id);
    printf("Message Name: %s\n", metadata->message_name);
    printf("Message Size: %d\n", metadata->message_size);
    printf("Sender Name: %s\n", metadata->sender_name);
}

void parse_by_line(char *filename){
    FILE *dbc_file = fopen(filename, "r");

    if(dbc_file == NULL){
        perror("Please provide a valid filename");
        return;
    }

    const int buffer_size = 200;
    char buffer [buffer_size];

    enum line_type line = NEWLINE;

    while(fgets(buffer, buffer_size, dbc_file)){
        
        if(buffer[0] == 'B' && buffer[1] == 'O' && buffer[2] == '_'){
            printf("%s", buffer);
            line = MESSAGE;

        }
        // else if(buffer[0] == ' ' && buffer[1] == 'S' && buffer[2] == 'G' && buffer[3] == '_'){
        //     printf("%s", buffer);
        //     line = SIGNAL;
        // }
        else
            line = NEWLINE;
        

        if(line == MESSAGE){
            struct msg_metadata msg;
            int line_index = 4;
            while(buffer[line_index] != '\n'){
                char temp_buffer[10];

                // Read the Message ID
                int index = 0;
                while(buffer[line_index] != ' '){
                    temp_buffer[index++] = buffer[line_index++];
                }
                temp_buffer[index] = '\0';
                msg.message_id = atoi(temp_buffer);
                
                while(buffer[line_index] == ' ' || buffer[line_index] == ':'){
                    line_index++;
                }

                // Read the Message Name
                index = 0;
                while(buffer[line_index] != ' ' && buffer[line_index] != ':'){
                    msg.message_name[index++] = buffer[line_index++];
                }
                msg.message_name[index++] = '\0';

                while(buffer[line_index] == ' ' || buffer[line_index] == ':'){
                    line_index++;
                }
                
                // Read the Message Size
                index = 0;
                while(buffer[line_index] != ' '){
                    // printf("%c", buffer[line_index]);
                    temp_buffer[index++] = buffer[line_index++];
                }
                temp_buffer[index++] = '\0';
                msg.message_size = atoi(temp_buffer);

                while(buffer[line_index] == ' ' || buffer[line_index] == ':'){
                    line_index++;
                }
                
                // Read the Sender Name
                index = 0;
                while(buffer[line_index] != ' '){
                    msg.sender_name[index++] = buffer[line_index++];
                }
                msg.sender_name[index++] = '\0';

                // Print the read Message Definition
                print_msg_metadata(&msg);

                while(buffer[line_index] != '\n'){
                    line_index++;
                }
            }
            
        }

    }

    fclose(dbc_file);
}

// ./a.out <filename>
int main(int argc, char *argv[]){
    // Must provide a filename for execution
    if(argc < 2){
        perror("Please provide a filename");
        return -1;
    }

    // parse_by_character(argv[1]);

    parse_by_line(argv[1]);




    return 0;
}