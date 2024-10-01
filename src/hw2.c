#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <arpa/inet.h>

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

   //printf("%02X %02X %02X %02X \n",((unsigned char *)(&header0))[0], ((unsigned char *)(&header0))[1], ((unsigned char *)(&header0))[2], ((unsigned char *)(&header0))[3]);
    //printf("%02X %02X %02X %02X \n",((unsigned char *)(&header1))[0], ((unsigned char *)(&header1))[1], ((unsigned char *)(&header1))[2], ((unsigned char *)(&header1))[3]);
   // printf("%02X %02X %02X %02X \n",((unsigned char *)(&header2))[0], ((unsigned char *)(&header2))[1], ((unsigned char *)(&header2))[2], ((unsigned char *)(&header2))[3]);
    
    printf("type: %08X\n", packet_type);

    if(packet_type == 0x1FFFFF){
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

    if(packet_type == 0x1FFFFF){
        printf("Data: ");
        int data_start_point = 3;
        for(unsigned i = 0; i < length; i++){
            if(i == length - 1){
                printf("%d ", (int)packet[data_start_point]);
            }
            printf("%d, ", (int)packet[data_start_point++]);
        }
    }
}

void store_values(unsigned int packets[], char *memory)
{
    (void)packets;
    (void)memory;
}

unsigned int* create_completion(unsigned int packets[], const char *memory)
{
    (void)packets;
    (void)memory;
	return NULL;
}
