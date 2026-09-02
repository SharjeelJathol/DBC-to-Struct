#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MSG_NAME_SIZE       50
#define SENDER_NAME_SIZE    50
#define SIGNAL_NAME_SIZE    50
#define MULTIPLEXER_SIZE    10
#define UNIT_SIZE           10
#define RECEIVER_NAME_SIZE  50

enum line_type {
    NEWLINE,
    MESSAGE,
    SIGNAL,
};

// <message_id> <message_name>: <message_size> <sender_name>
struct msg_metadata {
    int message_id;             //atoi()
    char message_name[MSG_NAME_SIZE];
    int message_size;           //atoi() size in bytes?
    char sender_name[SENDER_NAME_SIZE];

    struct signal_metadata *signals_head;

    struct msg_metadata *next;
};

// M is used to define multiplexed. Will always be at the offset 0
struct signal_metadata {
    char signal_name[SIGNAL_NAME_SIZE];
    bool multiplexed;                       // M 
    int start_bit;
    int length;                             // from the start bit
    bool endian;                            // 1 of little, 0 for big
    bool unsigned_value;                    // 1 for unsigned, 0 for signed
    int factor;
    int offset;
    int min_value;
    int max_value;
    char value_unit[UNIT_SIZE];             // Measurement unit of value
    char receiver_name[RECEIVER_NAME_SIZE];

    struct signal_metadata *next;
};

// Print msg_metadata struct
void print_msg_metadata(struct msg_metadata *msg){
    printf("Message ID: %d\n", msg->message_id);
    printf("Message Name: %s\n", msg->message_name);
    printf("Message Size: %d\n", msg->message_size);
    printf("Sender Name: %s\n\n\n", msg->sender_name);
}

void print_signal_metadata(struct signal_metadata *signal){
    printf("Signal Name: %s\n", signal->signal_name);
    printf("Multiplexed: %d\n", signal->multiplexed);
    printf("Start Bit: %d\n", signal->start_bit);
    printf("Signal Length: %d\n", signal->length);
    printf("Endian System: %d\n", signal->endian);
    printf("Unsigned Value: %d\n", signal->unsigned_value);
    printf("Factor: %d\n", signal->factor);
    printf("Offset: %d\n", signal->offset);
    printf("Minimum Value: %d\n", signal->min_value);
    printf("Maximum Value: %d\n", signal->max_value);
    printf("Measurement Unit: %s\n", signal->value_unit);
    printf("Receiver Name: %s\n\n\n", signal->receiver_name);
}

void print_msg(struct msg_metadata *msg){
    if(msg != NULL){
        print_msg_metadata(msg);
        struct signal_metadata *current = msg->signals_head;
        while(current != NULL){
            print_signal_metadata(current);
            current = current->next;
        }
    }
    return;
}

void write_header_file(char* filename, struct msg_metadata *msg_head){
    // to uppercase and change extension to .hpp
    for(int i = 0; filename[i] != '\0'; i++){
        if(filename[i] == '.'){
            filename[i+1] = 'h';
            filename[i+2] = 'p';
            filename[i+3] = 'p';
            filename[i+4] = '\0';
            break;
        }
        if(filename[i] >= 'a' && filename[i] <= 'z'){
            filename[i] = filename[i] - 32;
        }
    }

    FILE *header_file = fopen(filename, "w");
    fputs("#ifndef HEADER_HPP\n", header_file);
    fputs("#define HEADER_HPP\n\n", header_file);

    

    struct msg_metadata *msg = msg_head;
    while(msg != NULL){
        fprintf(header_file, "struct %s {\n", msg->message_name);
        struct signal_metadata *current = msg->signals_head;
        while(current != NULL){
            fprintf(header_file, "    int %s\t\t: %d;\n", current->signal_name, current->length);
            current = current->next;
        }
        fprintf(header_file, "};\n\n");
        msg = msg->next;
    }

    
    fputs("\n#endif", header_file);
    fclose(header_file);
}

void parse_by_line(char *filename){
    FILE *dbc_file = fopen(filename, "r");

    if(dbc_file == NULL){
        perror("Please provide a valid filename");
        return;
    }

    // Buffer space ot read lines into
    const int buffer_size = 200;
    char buffer [buffer_size];

    // By default we are at a new line
    enum line_type line = NEWLINE;

    struct msg_metadata *msg_head = NULL;
    struct msg_metadata *msg = NULL;

    while(fgets(buffer, buffer_size, dbc_file)){

        if(buffer[0] == 'B' && buffer[1] == 'O' && buffer[2] == '_'){
            printf("%s", buffer);
            line = MESSAGE;

        }
        else if(buffer[0] == ' ' && buffer[1] == 'S' && buffer[2] == 'G' && buffer[3] == '_'){
            printf("%s", buffer);
            line = SIGNAL;
        }
        else if(line == SIGNAL){
            // if(msg) print_msg(msg);
            if(msg){
                if(msg_head){
                    struct msg_metadata *current = msg_head;
                    while(current->next != NULL){
                        current = current->next;
                    }
                    current->next = msg;
                }
                else{
                    msg_head = msg;
                }
            }
            // write_header_file(filename, msg);
            // free(msg);
            line = NEWLINE;
        }
        else{
            line = NEWLINE;
        }
        

        if(line == MESSAGE){
            msg = (struct msg_metadata*)malloc(sizeof(struct msg_metadata));
            msg->signals_head = NULL;
            msg->next = NULL;
            // current_signal = NULL;

            int line_index = 4;
            while(buffer[line_index] != '\n'){
                char temp_buffer[10];

                // Read the Message ID
                int index = 0;
                while(buffer[line_index] != ' '){
                    temp_buffer[index++] = buffer[line_index++];
                }
                temp_buffer[index] = '\0';
                msg->message_id = atoi(temp_buffer);
                
                while(buffer[line_index] == ' ' || buffer[line_index] == ':'){
                    line_index++;
                }

                // Read the Message Name
                index = 0;
                while(buffer[line_index] != ' ' && buffer[line_index] != ':'){
                    msg->message_name[index++] = buffer[line_index++];
                }
                msg->message_name[index++] = '\0';

                while(buffer[line_index] == ' ' || buffer[line_index] == ':'){
                    line_index++;
                }
                
                // Read the Message Size
                index = 0;
                while(buffer[line_index] != ' '){
                    temp_buffer[index++] = buffer[line_index++];
                }
                temp_buffer[index++] = '\0';
                msg->message_size = atoi(temp_buffer);

                while(buffer[line_index] == ' ' || buffer[line_index] == ':'){
                    line_index++;
                }
                
                // Read the Sender Name
                index = 0;
                while(buffer[line_index] != ' '){
                    msg->sender_name[index++] = buffer[line_index++];
                }
                msg->sender_name[index++] = '\0';

                // Print the read Message Definition
                // print_msg_metadata(msg);

                while(buffer[line_index] != '\n'){
                    line_index++;
                }
            }
            
        }
        else if(line == SIGNAL){
            struct signal_metadata *signal = (struct signal_metadata*)malloc(sizeof(struct signal_metadata));
            int line_index = 5;
            char integer_buffer[10];
            while(buffer[line_index] != '\n'){
                int index = 0;
                // Get the Signal
                while(buffer[line_index] != ' '){
                    signal->signal_name[index++] = buffer[line_index++];
                }
                signal->signal_name[index++] = '\0';
                index = 0;

                // parse past the spaces
                while(buffer[line_index] == ' '){
                    line_index++;
                }

                // Check if the signal is multiplexed or not
                if(buffer[line_index] != ':'){
                    // set the multiplexed flag
                    signal->multiplexed = true;
                }else{
                    signal->multiplexed = false;
                }

                // parse past the multiplexed tag
                while(buffer[line_index] != ' '){
                    line_index++;
                }

                // parse past the spaces and colon
                while(buffer[line_index] == ' ' || buffer[line_index] == ':'){
                    line_index++;
                }

                // Read the start bit
                index = 0;
                while(buffer[line_index] != '|'){
                    integer_buffer[index++] = buffer[line_index++];
                }
                integer_buffer[index] = '\0';
                signal->start_bit = atoi(integer_buffer);

                // parse past the '|'
                line_index++;

                // Length
                index = 0;
                while(buffer[line_index] != '@'){
                    integer_buffer[index++] = buffer[line_index++];
                }
                integer_buffer[index] = '\0';
                signal->length = atoi(integer_buffer);

                // parse past the '@'
                line_index++;

                // Endian
                if(buffer[line_index] == '1')
                    signal->endian = 1;     // little endian
                else if(buffer[line_index] == '0')
                    signal->endian = 0;     // big endian
                line_index++;

                // unsigned value
                if(buffer[line_index] == '+')
                    signal->unsigned_value = 1;     // Signed value
                else if(buffer[line_index] == '-')
                    signal->unsigned_value = 0;     // Unsigned value
                line_index++;

                // parse past the spaces
                while(buffer[line_index] == ' '){
                    line_index++;
                }

                // Factor
                index = 0;
                while(buffer[++line_index] != ','){
                    integer_buffer[index++] = buffer[line_index];
                }
                integer_buffer[index] = '\0';
                signal->factor = atoi(integer_buffer);

                // Offset
                index = 0;
                while(buffer[++line_index] != ')'){
                    integer_buffer[index++] = buffer[line_index];
                }
                integer_buffer[index] = '\0';
                signal->offset = atoi(integer_buffer);
                line_index++;

                // parse past the spaces
                while(buffer[line_index] == ' '){
                    line_index++;
                }

                // min
                index = 0;
                while(buffer[++line_index] != '|'){
                    integer_buffer[index++] = buffer[line_index];
                }
                integer_buffer[index] = '\0';
                signal->min_value = atoi(integer_buffer);

                // max
                index = 0;
                while(buffer[++line_index] != ']'){
                    integer_buffer[index++] = buffer[line_index];
                }
                integer_buffer[index] = '\0';
                signal->max_value = atoi(integer_buffer);
                line_index++;

                // parse past the spaces
                while(buffer[line_index] == ' '){
                    line_index++;
                }

                // unit
                index = 0;
                line_index++;
                while(buffer[line_index] != '\"'){
                    signal->value_unit[index++] = buffer[line_index++];
                }
                signal->value_unit[index] = '\0';
                line_index++;

                // parse past the spaces
                while(buffer[line_index] == ' '){
                    line_index++;
                }

                // receiver name
                index = 0;
                while(buffer[line_index] != '\n'){
                    signal->receiver_name[index++] = buffer[line_index++];
                }
                signal->receiver_name[index] = '\0';

                // print_signal_metadata(signal);

                while(buffer[line_index] != '\n'){
                    line_index++;
                }
            }

            if(msg->signals_head == NULL){
                msg->signals_head = signal;
                signal = NULL;
            }
            else{
                struct signal_metadata *current = msg->signals_head;
                struct signal_metadata *prev = NULL;
                while(current != NULL){
                    if(signal->start_bit == current->start_bit) break;
                    else if(signal->start_bit < current->start_bit){
                        if(prev == NULL){
                            signal->next = current;
                            msg->signals_head = signal;
                            signal = NULL;
                            break;
                        }
                        prev->next = signal;
                        signal->next = current;
                        signal = NULL;
                        break;
                    }
                    else if(current->next == NULL){
                        current->next = signal;
                        signal = NULL;
                        break;
                    }
                    prev = current;
                    current = current->next;
                }
            }
        }

    }
    
    printf("\n");
    write_header_file(filename, msg_head);
    free(msg_head);
    fclose(dbc_file);
}

// ./a.out <filename>
int main(int argc, char *argv[]){
    // Must provide a filename for execution
    if(argc < 2){
        perror("Please provide a filename");
        return -1;
    }

    parse_by_line(argv[1]);

    return 0;
}