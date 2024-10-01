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
    unsigned int header2 = ntohl(packet[2]);

    unsigned int packet_type = header0 >> 10;
    unsigned int length = header0 & 0x3FF;
    unsigned int address = htonl(header2 >> 2);
    unsigned int requester_ID = (header1 >> 16) & 0xFFFF;
    unsigned int tag = (header1 >> 8) & 0xFF;
    unsigned int last_BE = (header1 >> 4) & 0xF;
    unsigned int first_BE = header1 & 0xF;

    if(packet_type == 0x1FFFFF){
        printf("Packet Type: Write");
    }
    else if(packet_type == 0){
        printf("Packet Type: Read");
    }

    printf("Address: %d", address);
    printf("Length: %d", length);
    printf("Requester ID: %d", requester_ID);
    printf("Tag: %d", tag);
    printf("Last BE: %d", last_BE);
    printf("1st BE: %d", first_BE);

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
