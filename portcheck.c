#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <assert.h>

int main(int argc, char *argv[]) {
    int sock;
    struct ifreq ifr;
    char *myip = NULL;
    char filename[256]; // assuming filename is declared somewhere

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';

    if (ioctl(sock, SIOCGIFADDR, &ifr) == -1) {
        perror("ioctl");
        close(sock);
        return 1;
    }

    close(sock);

    myip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    if (argc != 3) {
        printf("Usage: %s <IP> <Port>\n", argv[0]);
        return 1;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed\n");
        return 1;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed\n");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        //printf("Port %d on %s is closed.\n", port, ip);
        printf("%d",0);
        close(sockfd);
        return 0;
    }

    //sprintf(filename, "%s.txt", myip);
    //FILE* file = fopen(filename, "a");
    //if (file != NULL) {
    //    fprintf(file, "%s,%d\n", ip, port);
    //    fclose(file);
    //}

    //printf("Port %d on %s is open.\n", port, ip);
    printf("%d",1);
    close(sockfd);
    return 0;
}
