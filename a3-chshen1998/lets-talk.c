#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include "list.h"
#include <pthread.h>
#define NUM_THREADS 4

List* list_send;
List* list_receive;
int encryption_key;
int sockfd;
struct sockaddr_in my_addr, send_addr;
bool exit_cond = false;
bool status_check = false;

void *receiver_t(void *list_receive) {
    List *list = (List*) list_receive;

    char dec_msg[256], buffer[256];
    int len = sizeof(send_addr);
    while (true) {
        int n = recvfrom(sockfd, (char *)buffer, 256, 0, (struct sockaddr *)&send_addr, &len);
        buffer[n] = '\0';

        memset(&dec_msg[0], 0, sizeof(dec_msg));
        for (int i=0; i<strlen(buffer); i++) {
            dec_msg[i] = (buffer[i] - encryption_key + 256) % 256;
        }
        
        if (strcmp(dec_msg, "!status") == 0) {
            int status = List_append(list_send, "!status_online");
        } else if ((strcmp(dec_msg, "!status_online") == 0) & (status_check == true)) {
            int status = List_append(list_receive, "Online");
            status_check = false;
        } else {
            int status = List_append(list_receive, dec_msg);
        }
    }
}

void *display_t(void *list_receive) {
    List *list = (List*) list_receive;

    while (true) {
        if (List_curr(list) != NULL) {
            void *vmsg = List_remove(list);
            char *msg = (char *) vmsg;
            printf("%s \n", msg);

            if (strcmp(msg, "!exit") == 0) {
                exit_cond = true;
            }
        }
    }
}

void *sender_t(void *list_send) {
    List *list = (List*) list_send;

    char enc_msg[256], dec_msg[256];
    int len = sizeof(send_addr);
    while (true) {
        if (List_curr(list) != NULL) {
            memset(&enc_msg[0], 0, sizeof(enc_msg));

            void *vmsg = List_remove(list);
            char *msg = (char*) vmsg;

            for (int i=0; i<strlen(msg); i++) {
                enc_msg[i] = (msg[i] + encryption_key) % 256;
            }

            int s = sendto(sockfd, (const char *)enc_msg, strlen(enc_msg), 0, (struct sockaddr *)&send_addr, len);

            if (strcmp(msg, "!exit") == 0) {
                exit_cond = true;
            } else if (strcmp(msg, "!status") == 0) {
                status_check = true;
                sleep(1);
            }
        }
    }
}

void *keyboard_t(void *list_send) {
    List *list = (List*) list_send;
    char input[256];
    int status;

    printf("Welcome to Lets-Talk! Please type your messages now.\n");
    while(true) {
        fgets(input, 256, stdin);
        input[strcspn(input, "\r\n")] = 0; 
        status = List_append(list_send, input);
        if (status == -1) {
            printf("Failed to add message to list");
        }
    }
}

int main(int argc, char **argv) {
    // Port numbers & IP address
    char *ip_address;
    int port1, port2;
    port1 = atoi(argv[1]);
    ip_address = argv[2];
    port2 = atoi(argv[3]);

    // Shared lists
    list_send = List_create();
    list_receive = List_create();
    encryption_key = 5;

    // Setup UDP 
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port1);
    my_addr.sin_addr.s_addr = inet_addr(ip_address);

    if ((bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr))) < 0) {
        perror("Failed to bind");
        exit(EXIT_FAILURE);
    }

    memset(&send_addr, 0, sizeof(send_addr));
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(port2);
    send_addr.sin_addr.s_addr = inet_addr(ip_address);

    pthread_t threads[NUM_THREADS];
    pthread_create(&threads[0], NULL, receiver_t, list_receive);
    pthread_create(&threads[1], NULL, display_t, list_receive);
    pthread_create(&threads[2], NULL, sender_t, list_send);
    pthread_create(&threads[3], NULL, keyboard_t, list_send);

    while (!exit_cond) {
        // wait for exit command
    }

    for (int i=0; i<NUM_THREADS; i++) {
        pthread_kill(threads[i], 1);
    }

    pthread_exit(NULL);
    return 0;
}