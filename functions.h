#pragma once

#include "macros.h"
#include "stdio.h"
#include <unistd.h>
#include <sys/socket.h>
#include "stdlib.h"
#include <stdbool.h> 
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//Function to parse arguments. 0 upon sucess, 1 otherwise.
int parse_args(char* string, struct args* args);

//Function to print arguments parsed
int print_args(struct args* args);

//Function to get ip from host
int getIp(char* host, char* ip);

//Function to create socket
int createSocket(char* address, int port);

//Function to read from socket
int readSocketCode(int socket, char* buffer, int size);

//Function to do login
int login(int socket, char* user, char* pass);

//Function to enter passive mode
int enter_passive_mode(int socket, int *port, char* ip_address);

//Function to request resource
int request_resource(int socket, char* file_name);

//Function to download file
int download_file(int control_socket, int data_socket, char* filename);

//Function to disconnect
int disconnect(int control_socket);

//Function to change directory
int changeDirectory(int control_socket, char* filepath);

