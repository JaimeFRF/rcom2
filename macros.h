#pragma once

#define MAX_SIZE 200
#define DEFAULT_PORT 21
#define READY_CODE "220"
#define USERNAME_CODE "331"
#define LOGIN_SUCESSFUL "230"
#define WRONG_CREDENTIALS "530"
#define PASSIVE_MODE "227"
#define READY_TO_TRANSFER "150"
#define NO_FILE "550"
#define DISCONNECT "221"
#define DIRECTORY_CHANGED_SUCESS "250"

struct args {
    char user[MAX_SIZE];
    char pass[MAX_SIZE];
    char host[MAX_SIZE];
    char file_name[MAX_SIZE];
    char file_path[MAX_SIZE];
};

struct sockets {
    int control_socket;
    int data_socket;
};