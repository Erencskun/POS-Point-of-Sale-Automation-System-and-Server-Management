#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <sys/select.h>
#include <openssl/evp.h>
#include <sys/time.h>

void padWithZeros(char *str, int width) {
    int len = strlen(str);
    
    if (len < width) {
        int zerosToAdd = width - len;
        char paddedStr[width + 1];
        
        for (int i = 0; i < zerosToAdd; i++) {
            paddedStr[i] = '0';
        }
        
        strcpy(paddedStr + zerosToAdd, str);
        
        strcpy(str, paddedStr);
    }
}
int main() {

    //setvbuf(stdout,NULL,_IONBF,0);
    char ip[20];
    int port;

    printf("Server IP address: ");
    scanf("%s", ip);

    printf("Server port number: ");
    scanf("%d", &port);

    int sockfd;
    struct sockaddr_in serverAddr;

while(1){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation error");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection error");
        close(sockfd);
        exit(1);
    }
   char cmd=0x01;
    char userId[5];
    char password[20];
   int authenticated=0;

  while(!authenticated){
    sleep(1);
    printf("User ID:");
    scanf("%s", userId);

    printf("Password: ");
    scanf("%s", password); 

 padWithZeros(userId,4);

    unsigned char hashedPassword[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *)password, strlen(password), hashedPassword);

         printf("useridnew%s\n", userId);


   char userIdHex[9];  // 4 byte * 2 (hex) + null terminator
    for (int i = 0; i < (int)(strlen(userId)); i++) {
        sprintf(&(userIdHex[i * 2]), "%02x", userId[i]);
    }

    printf("useridhex %s\n", userIdHex);
    // Convert hashedPassword to a hexadecimal string

    char hashedPasswordHex[SHA_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(&(hashedPasswordHex[i * 2]), "%02x", hashedPassword[i]);
    }

char cmdHex[3];  // 1 byte * 2 (hex) + null terminator
    snprintf(cmdHex, sizeof(cmdHex), "%02x", cmd);

    // Concatenate cmd, userIdHex, and hashedPasswordHex
    char combinedStr[2+2+8+SHA_DIGEST_LENGTH * 2 + 1 +4]; // cmdHex + userIdHex + hashedPasswordHex + null terminator
    snprintf(combinedStr, sizeof(combinedStr), "%s%s%s",cmdHex, userIdHex, hashedPasswordHex);
    
    size_t length =(strlen(combinedStr) / 2);
                                         
      printf("%zu\n",length);
        
       char combinedStrLengthHex[5]; // 4 k + null karakter

       snprintf(combinedStrLengthHex, sizeof(combinedStrLengthHex), "%zx", length);

       snprintf(combinedStr, sizeof(combinedStr), "%s%s%s%s",combinedStrLengthHex,cmdHex, userIdHex, hashedPasswordHex);

      printf("Combined String: %s\n", combinedStr);

    
    // Send user credentials to server
    send(sockfd, combinedStr, sizeof(combinedStr), 0);

    int response;
    recv(sockfd, &response, sizeof(response), 0);
 

    if (response == 0) {

        printf("Authentication successful\n");
        authenticated=1;
       int result = system("make -f Makefile run");         
       if (result == -1) {
        perror("system");
         }
       exit(1);
    } else if (response == 1) {
        printf("wrong password\n");

    }else  if (response==2){
        
        printf("User not found\n");
       }

    }
    close(sockfd);
    break;
}   
struct timeval timeout;
    timeout.tv_sec = 5;  // 5 seconds
    timeout.tv_usec = 0;

fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);

    int result = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
    if (result == -1) {
        perror("Select error");
        exit(1);
    } else if (result == 0) {
        printf("Connection failure: Server did not respond within 5 seconds.\n");
        close(sockfd);
        exit(1);
    }
    return 0;
}