#include "functions.h"


int parse_args(char* string, struct args* args){

    printf("Parsing the arguments...\n");

    char* path = malloc(MAX_SIZE); 
    path[0] = '\0'; 
    char* credentials_host = malloc(MAX_SIZE);

    bool firstIteration = true;
    char* token = strtok(string, ":");
    token = strtok(NULL, "/");   
    while(token != NULL){
        if(!firstIteration){
            strcat(path, "/"); 
            strcat(path, token); 
        }else{
            strcpy(credentials_host, token);

        }
        char* nextToken = strtok(NULL, "/");
        if(nextToken == NULL){
            strcpy(args->file_name, token);
        }
        firstIteration = false;
        token = nextToken;   
    }
    strcpy(args->file_path, path);

    args->file_path[strlen(args->file_path) - strlen(args->file_name)] = '\0';

    firstIteration = true;

    char* ret = strchr(credentials_host, '@');
    if(ret == NULL){
        strcpy(args->host, credentials_host);
        strcpy(args->pass, "password");
        strcpy(args->user, "anonymous");
    }
    else{
        token = strtok(credentials_host, "@");

        char* credentials = malloc(MAX_SIZE);
        while(token != NULL){
            if(!firstIteration){
                strcpy(args->host, token);
            }else{
                strcpy(credentials, token);
            }
            token = strtok(NULL, "@");
            firstIteration = false;
        }

        firstIteration = true;
        token = strtok(credentials, ":");

        while(token != NULL){
            if(!firstIteration){
                strcpy(args->pass, token);
            }else{
                strcpy(args->user, token);
            }
            token = strtok(NULL, ":");
            firstIteration = false;
        }
        free(credentials);
    }

    free(path);
    free(credentials_host);

    return !(args->file_name != NULL || args->file_path != NULL || args->host != NULL || args->pass != NULL || args->user != NULL);

}

int print_args(struct args* args){
    printf("Printing the arguments...\n");
    printf("User: %s\n", args->user);
    printf("Pass: %s\n", args->pass);
    printf("Host: %s\n", args->host);
    printf("File name: %s\n", args->file_name);
    printf("File path: %s\n", args->file_path);
    return 0;
}

int getIp(char* host, char* ip){
    struct hostent *h;

    if ((h=gethostbyname(host)) == NULL) {  
        herror("gethostbyname");
        return -1;
    }
    
    strcpy(ip, inet_ntoa(*((struct in_addr *) h->h_addr)));
    return 0;
}

int createSocket(char* address, int port){
    int sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);    
    server_addr.sin_port = htons(port);        

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }
    /*connect to the server*/
    if (connect(sockfd,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect()");
        return -1;
    }

    return sockfd;
}

int readSocketCode(int socket, char* buffer, int size){
    ssize_t total_bytes_read = 0;

    while (1)
    {
        usleep(100000);
        ssize_t bytes_read = recv(socket, buffer, size - total_bytes_read, MSG_DONTWAIT);
        if (bytes_read < 0)
        {
            buffer[total_bytes_read] = '\0';
            break;
        }
        total_bytes_read += bytes_read;
    }
    buffer[3] = '\0';

    return 0;
    
}

int login(int socket, char* user, char* pass){

    char comando[MAX_SIZE];
    strcpy(comando, "user ");
    strcat(comando, user);
    strcat(comando, "\n");

    int bytes_written = write(socket, comando, 5 + strlen(user) + 1);
    if(bytes_written != 5 + strlen(user) + 1){
        printf("Error writing user to socket\n");
        return -1;
    }

    char response[MAX_SIZE];
    if(readSocketCode(socket, response, MAX_SIZE) < 0){
        printf("Error reading socket\n");
        return -1;
    }
    if(strcmp(response, USERNAME_CODE) != 0){
        printf("Username not accepted\n");
        return -1;
    }

    memset(comando, 0, MAX_SIZE);
    strcpy(comando, "pass ");
    strcat(comando, pass);
    strcat(comando, "\n");


    bytes_written = write(socket, comando, 5 + strlen(pass) + 1);
    if(bytes_written != 5 + strlen(pass) + 1){
        printf("Error writing pass to socket\n");
        return -1;
    }

    char login_response[MAX_SIZE];
    if(readSocketCode(socket, login_response, MAX_SIZE) < 0){
        printf("Error reading socket\n");
        return -1;
    }

    if(strcmp(login_response, LOGIN_SUCESSFUL) != 0){
        if(strcmp(login_response, WRONG_CREDENTIALS) == 0){printf("Wrong Credentials\n");}
        else {printf("Login failed\n");}
        return -1;
    }
    
    return 0;
}

int enter_passive_mode(int socket, int *port, char* ip_address){
    char* comando = "pasv \n";
    if(write(socket, comando, strlen(comando)) != strlen(comando)){
        return -1;
    }
    int port1, port2, ip1, ip2, ip3, ip4;
    char* response = malloc(MAX_SIZE);
    char* code = malloc(4);

    while(true){
        int bytes_read = read(socket, response, MAX_SIZE);
        if(bytes_read >= 3 && response[0] >= '1' && response[0] <= '5'){
            response[bytes_read] = '\0';
            printf("%s", response);

            strncpy(code, response, 3);
            code[3] = '\0';  

            if(strcmp(code, PASSIVE_MODE) != 0){
                printf("Error entering passive mode\n");
                return -1;
            }

            sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip1, &ip2, &ip3, &ip4, &port1, &port2);
            sprintf(ip_address, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);

            break; 
        }
    }
    
    *port = port1 * 256 + port2;
    
    free(response);
    free(code);

    return 0;
}

int request_resource(int socket, char* filename){
    char* comando = malloc(6 + strlen(filename));
    sprintf(comando, "retr %s\n", filename);
    int bytes_written = write(socket, comando, strlen(comando));
    
    if(bytes_written != strlen(comando)){
        printf("Error writing request resource command\n");
        return -1;
    }

    char buffer[MAX_SIZE];
    if(readSocketCode(socket, buffer, MAX_SIZE) < 0){
        return -1;
    }

    if(strcmp(buffer, READY_TO_TRANSFER) == 0){
        printf("File found, beginning transfer\n");
        return 0;
    }else if(strcmp(buffer, NO_FILE) == 0){
        printf("The file you requested does not exist in this directory\n");
        return -1;
    }

    return 0;
}

int download_file(int control_socket, int data_socket, char* filename){
    FILE* file = fopen(filename, "wb");
    if(file == NULL){
        printf("Error creating file\n");
        return -1;
    }

    char buf[1024];
    int bytes;

    printf("Beginning to download file\n");
    do{
        bytes = read(data_socket, buf, 1024);
        fwrite(buf, 1, bytes, file);
    }while(bytes != 0);
    printf("Finished downloading file\n");
    fclose(file);

    char response[MAX_SIZE];
    if(readSocketCode(control_socket, response, MAX_SIZE) < 0){
        return -1;
    }


    return 0;
}


int changeDirectory(int control_socket, char* filepath){
    char* command = malloc(5 + strlen(filepath));
    sprintf(command, "cwd %s\n", filepath);
    int bytes_written = write(control_socket, command, strlen(command));

    if(bytes_written != strlen(command)){
        printf("Error writing command to change directory\n");
        return -1;
    }

    char buffer[MAX_SIZE];
    if(readSocketCode(control_socket, buffer, MAX_SIZE) < 0){
        return -1;
    }
    if(strcmp(buffer, DIRECTORY_CHANGED_SUCESS) != 0){
        return -1;
    }

    return 0;
}
