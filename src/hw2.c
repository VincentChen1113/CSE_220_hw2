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


void store_values(unsigned int packets[], char *memory){
    unsigned int packets_index = 0;
    while(packets[packets_index] != 0){
        unsigned int header0 = packets[packets_index++];//0 -> 1
        unsigned int header1 = packets[packets_index++];//1 -> 2
        unsigned int header2 = packets[packets_index++];//2 -> 3

        unsigned int packet_type = (header0 >> 10) & 0x3FFFFF;
        unsigned int length = header0 & 0x3FF;
        unsigned int address = header2 & 0x3FFFFFFF;
        unsigned int last_BE = (header1 >> 4) & 0xF;
        unsigned int first_BE = header1 & 0xF;

        
        unsigned int memory_index = address;
        unsigned int data = packets[packets_index++];
        if(packet_type != 0x100000){
            packets_index += length;
            continue;
        }
        if(address < 0x100000){
            //writing memory for 1st BE
            if(first_BE & 1) // 0b0001
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

            if(first_BE & 8)//0b1000
                memory[memory_index++] = (char)((data >> 24) & 0xFF);
            else
                memory_index++;

            if(length == 1)
                continue; // check for only one data
            else if(length > 2){
                for(unsigned int i = 1; i < length - 1; i++){
                    data = packets[packets_index++];
                    memory[memory_index++] = (data & 0xFF);
                    memory[memory_index++] = ((data >> 8) & 0xFF);
                    memory[memory_index++] = ((data >> 16)& 0xFF);
                    memory[memory_index++] = ((data >> 24) & 0xFF);
                }
            }

            //writing memory for last BE
            unsigned int last_data = packets[packets_index];
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
            packets_index++;// move to next request
        }
        else{
            break;//Do nothing and go to next packet
        }
    }
    
}

unsigned int* create_completion(unsigned int packets[], const char *memory){

    unsigned int *completed_pockets = malloc(262144 * sizeof(unsigned int)); //1 MB pocket to store and return
    if(completed_pockets == NULL)
        return NULL;

    unsigned int current_packets_index = 0;
    unsigned int complete_read_index = 0;

    unsigned int header_type = 0x25;
    header_type <<= 25;
    unsigned int completer_id = 0xDC;
    completer_id <<= 16;
    unsigned int byte_count = 0;

    while(packets[current_packets_index] != 0){
        unsigned int header0 = packets[current_packets_index++];//0 -> 1
        unsigned int header1 = packets[current_packets_index++];//1 -> 2
        unsigned int header2 = packets[current_packets_index++];//2 -> 3

        unsigned int packet_type = (header0 >> 10) & 0x3FFFFF;
        unsigned int length = header0 & 0x3FF;
        unsigned int address = header2 & 0x3FFFFFFF;
        unsigned int requester_ID = (header1 >> 16) & 0xFFFF;
        unsigned int tag = (header1 >> 8) & 0xFF;
        unsigned int last_BE = (header1 >> 4) & 0xF;
        unsigned int first_BE = header1 & 0xF;
        byte_count = length * 4;
        unsigned int lower_address = address & 0x7F;
        unsigned int memory_index = address;



        if(packet_type == 0){
            if((address + length) >= 0x4000){
                unsigned int exceed = (address + length) - 0x4000;
                completed_pockets[complete_read_index++] = (header_type | (length - exceed)); //completion header0 wrote
                completed_pockets[complete_read_index++] = (completer_id | (byte_count & 0xFFF)); //completion header1 wrote
                completed_pockets[complete_read_index++] = ((requester_ID << 16) | (tag << 8) | lower_address ); ////completion header2 wrote
                
                unsigned int data = 0;

                if(first_BE & 1) // 0b0001
                    data |= (memory[memory_index++] & 0xFF);
                else 
                    memory_index++;

                if(first_BE & 2) // 0b0010
                    data |= ((memory[memory_index++] << 8) & 0xFF00);
                else
                    memory_index++;

                if(first_BE & 4)
                    data |= ((memory[memory_index++] << 16) & 0xFF0000);
                else
                    memory_index++;

                if(first_BE & 8)//0b1000
                    data |= ((memory[memory_index++] << 24) & 0xFF000000);
                else
                    memory_index++;
                
                completed_pockets[complete_read_index++] = data;

                for(unsigned int i = 1; i < (length - exceed); i++){
                    data = 0;
                    data |= (memory[memory_index++] & 0xFF);
                    data |= ((memory[memory_index++] << 8) & 0xFF00);
                    data |= ((memory[memory_index++] << 16) & 0xFF0000);
                    data |= ((memory[memory_index++] << 24) & 0xFF000000);
                    completed_pockets[complete_read_index++] = data;// put in data
                } //address exceed 0x4000 split to next packet

                length -= exceed; //update length
                byte_count = length * 4; //update byte count
                lower_address = 0; //update low address
                completed_pockets[complete_read_index++] = (header_type | length); //completion header0 wrote
                completed_pockets[complete_read_index++] = (completer_id | (byte_count & 0xFFF)); //completion header1 wrote
                completed_pockets[complete_read_index++] = ((requester_ID << 16) | (tag << 8) | lower_address); ////completion header2 wrote
                
                for(unsigned int i = 0; i < length - 1; i++){
                    data = 0;
                    data |= (memory[memory_index++] & 0xFF);
                    data |= ((memory[memory_index++] << 8) & 0xFF00);
                    data |= ((memory[memory_index++] << 16) & 0xFF0000);
                    data |= ((memory[memory_index++] << 24) & 0xFF000000);
                    completed_pockets[complete_read_index++] = data;// put in data
                }

                data = 0; //reset data
                
                if(last_BE & 1)
                    data |= (memory[memory_index++] & 0xFF);
                else
                    memory_index++;
                if(last_BE & 2)
                    data |= ((memory[memory_index++] << 8) & 0xFF00);
                else
                    memory_index++;
                if(last_BE & 4)
                    data |= ((memory[memory_index++] << 16) & 0xFF0000);
                else
                    memory_index++;
                if(last_BE & 8)
                     data |= ((memory[memory_index++] << 24) & 0xFF000000);
                else
                    memory_index++;  
                
                completed_pockets[complete_read_index++] = data;              
                                
            }
            else{
                completed_pockets[complete_read_index++] = (header_type | length); //completion header0 wrote
                completed_pockets[complete_read_index++] = (completer_id | (byte_count & 0xFFF)); //completion header1 wrote
                completed_pockets[complete_read_index++] = ((requester_ID << 16) | (tag << 8) | lower_address); ////completion header2 wrote
                unsigned int data = 0;
                
                if(first_BE & 1) // 0b0001
                    data |= (memory[memory_index++] & 0xFF);
                else 
                    memory_index++;

                if(first_BE & 2)
                    data |= ((memory[memory_index++] << 8) & 0xFF00);
                else
                    memory_index++;

                if(first_BE & 4)
                    data |= ((memory[memory_index++] << 16) & 0xFF0000);
                else
                    memory_index++;

                if(first_BE & 8)//0b1000
                    data |= ((memory[memory_index++] << 24) & 0xFF000000);
                else
                    memory_index++;
                
                completed_pockets[complete_read_index++] = data;

                for(unsigned int i = 1; i < length - 1; i++){
                    data = 0;
                    data |= (memory[memory_index++] & 0xFF);
                    data |= ((memory[memory_index++] << 8) & 0xFF00);
                    data |= ((memory[memory_index++] << 16) & 0xFF0000);
                    data |= ((memory[memory_index++] << 24) & 0xFF000000);
                    completed_pockets[complete_read_index++] = data;// put in data
                }

                data = 0; //reset data
                
                if(last_BE & 1)
                    data |= (memory[memory_index++] & 0xFF);
                else
                    memory_index++;
                if(last_BE & 2)
                    data |= ((memory[memory_index++] << 8) & 0xFF00);
                else
                    memory_index++;
                if(last_BE & 4)
                    data |= ((memory[memory_index++] << 16) & 0xFF0000);
                else
                    memory_index++;
                if(last_BE & 8)
                     data |= ((memory[memory_index++] << 24) & 0xFF000000);
                else
                    memory_index++; 
                
                completed_pockets[complete_read_index++] = data;
            }    
        }
        else{ // request type is not read
            continue;
        }
    }
    
	return completed_pockets;
}
