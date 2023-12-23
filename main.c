#include <stdio.h>
#include <string.h> 
#include "functions.h"

int main(int argc, char *argv[]){
 
    if(argc != 2){
        printf("Wrong number of arguments\n");
        return -1;
    }

    struct args args;
    char ip[MAX_SIZE];
    char buffer[1024];
    struct sockets sockets;

    if(parse_args(argv[1], &args) != 0){
        printf("Error parsing arguments\n");
        return -1;
    }

    if(print_args(&args) != 0){
        printf("Error printing arguments\n");
        return -1;
    }

    if(getIp(args.host, ip) != 0){
        printf("Error getting ip\n");
        return -1;
    }

    if((sockets.control_socket = createSocket(ip, DEFAULT_PORT)) < 0){
        printf("Error creating socket\n");
        return -1;
    }

    if(readSocketCode(sockets.control_socket, buffer, 1024) < 0){
        printf("Error reading socket\n");
        return -1;
    }

    if(strcmp(buffer, READY_CODE) != 0){
        printf("Server is not ready to receive information\n");
        return -1;
    }

    if(login(sockets.control_socket, args.user, args.pass) != 0){
        printf("Error logging in\n");
        return -1;
    }

    if(changeDirectory(sockets.control_socket, args.file_path) != 0){
        printf("Error changing directory\n");
        return -1;
    }

    int data_port;
    char* ip_address = malloc(MAX_SIZE);

    if(enter_passive_mode(sockets.control_socket, &data_port, ip_address) != 0){
        printf("Error entering passive mode\n");
        return -1;
    }

    printf("Data port: %d\n", data_port);
    printf("Ip address: %s\n", ip_address);

    if((sockets.data_socket = createSocket(ip_address, data_port)) < 0){
        printf("Error creating socket\n");
        return -1;
    }

    if(request_resource(sockets.control_socket, args.file_name) < 0){
        printf("Error requesting resource\n");
        return -1;
    }

    if(download_file(sockets.control_socket, sockets.data_socket, args.file_name) < 0){
        printf("Error downloading file\n");
        return -1;
    }

    printf("File downloaded sucessfully\n");
    close(sockets.control_socket);
    close(sockets.data_socket);


    return 0;

}
