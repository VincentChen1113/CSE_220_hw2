#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <arpa/inet.h>
#include <string.h>

#include "hw2.h"

void print_packet(unsigned int packet[])
{
    unsigned int header0 = packet[0];
    unsigned int header1 = packet[1];
    unsigned int header2 = packet[2];

    unsigned int packet_type = (header0 >> 10) & 0x3FFFFF;
    unsigned int length = header0 & 0x3FF;
    unsigned int address = header2 & 0x3FFFFFFF;
    unsigned int requester_ID = (header1 >> 16) & 0xFFFF;
    unsigned int tag = (header1 >> 8) & 0xFF;
    unsigned int last_BE = (header1 >> 4) & 0xF;
    unsigned int first_BE = header1 & 0xF;


    if(packet_type == 0x100000){
        printf("Packet Type: Write\n");
    }
    else if(packet_type == 0){
        printf("Packet Type: Read\n");
    }

    printf("Address: %d\n", address);
    printf("Length: %d\n", length);
    printf("Requester ID: %d\n", requester_ID);
    printf("Tag: %d\n", tag);
    printf("Last BE: %d\n", last_BE);
    printf("1st BE: %d\n", first_BE);
    printf("Data: ");

    if(packet_type == 0x100000){
        int data_start_point = 3;
        for(unsigned i = 0; i < length; i++){
            if(i == length - 1){
                printf("%d \n", (int)packet[data_start_point]);
            }
            else{
                printf("%d ", (int)packet[data_start_point++]);
            }
        }
    }
    else{
        printf("\n");
    }
}


void store_values(unsigned int packet[], char *memory){
    unsigned int header0 = packet[0];
    unsigned int header1 = packet[1];
    unsigned int header2 = packet[2];

    //unsigned int packet_type = (header0 >> 10) & 0x3FFFFF;
    unsigned int length = header0 & 0x3FF;
    unsigned int address = header2 & 0x3FFFFFFF;
    unsigned int last_BE = (header1 >> 4) & 0xF;
    unsigned int first_BE = header1 & 0xF;

    
    unsigned int memory_index = address;
    int data_index = 3;
    unsigned int data = packet[data_index];


        if(length > 0 && address < 0x100000){
            //writing memory for 1st BE
            if(first_BE & 1)
                memory[memory_index++] = (data & 0xFF);
            else 
                memory_index++;

            if(first_BE & 2)
                memory[memory_index++] = ((data >> 8) & 0xFF);
            else
                memory_index++;

            if(first_BE & 4)
                memory[memory_index++] = ((data >> 16)& 0xFF);
            else
                memory_index++;

            if(first_BE & 8)
                memory[memory_index++] = ((data >> 24) & 0xFF);
            else
                memory_index++;

            if(length > 2){
                for(unsigned int i = 1; i < length - 1; i++){
                    data_index = data_index + i;
                    memory[memory_index++] = (data & 0xFF);
                    memory[memory_index++] = ((data >> 2) & 0xFF);
                    memory[memory_index++] = ((data >> 4)& 0xFF);
                    memory[memory_index++] = ((data >> 6) & 0xFF);
                }
            }

            //writing memory for last BE
            unsigned int last_data_index = length + 2;
            unsigned int last_data = packet[last_data_index];
            if(last_BE & 1)
                memory[memory_index++] = (last_data & 0xFF);
            else
                memory_index++;
            if(last_BE & 2)
                memory[memory_index++] = ((last_data >> 8) & 0xFF);
            else
                memory_index++;
            if(last_BE & 4)
                memory[memory_index++] = ((last_data >> 16) & 0xFF);
            else
                memory_index++;
            if(last_BE & 8)
                memory[memory_index++] = ((last_data >> 24) & 0xFF);
            else
                memory_index++;
        }
    
}

unsigned int* create_completion(unsigned int packets[], const char *memory)
{
    (void)packets;
    (void)memory;
	return NULL;
}
