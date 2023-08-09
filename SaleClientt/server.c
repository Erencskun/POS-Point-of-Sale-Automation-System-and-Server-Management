#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <sqlite3.h>
struct User {
    char id[20];
    char passwordHash[SHA_DIGEST_LENGTH * 2 + 1]; // Hexadecimal representation
};
    
#define MAX_USERS 100

struct User users[MAX_USERS];
int userCount = 0;

void addUser(const char *id, const char *password) {
    if (userCount < MAX_USERS) {
        strcpy(users[userCount].id, id);

        unsigned char hashedPassword[SHA_DIGEST_LENGTH];
        SHA1((unsigned char *)password, strlen(password), hashedPassword);

        // Convert hashedPassword to a hexadecimal string
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
            sprintf(&(users[userCount].passwordHash[i * 2]), "%02x", hashedPassword[i]);
                    //printf("%s",users[0].passwordHash[0]);       
        }
users[userCount].passwordHash[SHA_DIGEST_LENGTH * 2] = '\0'; // Add null terminato
        userCount++;
    }
}

int main() {
    // Database connection
    
    sqlite3 *db;
        char *errMessage = 0;
    int rc = sqlite3_open("mydatabase.db", &db);
    if (rc) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
   const char *createTableSQL = "CREATE TABLE users ("
                                 "id TEXT NOT NULL,"
                                 "password TEXT NOT NULL);";
    rc = sqlite3_exec(db, createTableSQL, 0, 0, &errMessage);        
    const char *insertDataSQL = "INSERT INTO users (id, password) VALUES"
                                "('0479', 'helloworld'),"
                                "('0550', 'password'),"
                                "('8795', 'merhaba');"  ;              
    const char *query = "SELECT id, password FROM users";
    rc = sqlite3_exec(db, insertDataSQL, 0, 0, &errMessage);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Error %s\n", errMessage);
        sqlite3_free(errMessage);
    } else {
        fprintf(stdout, "Data added successfuly.\n");
    }
    sqlite3_stmt *stmt;

    rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char *id = (const char *)sqlite3_column_text(stmt, 0);
        const char *passwordHash = (const char *)sqlite3_column_text(stmt, 1);
        addUser(id, passwordHash);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    int sockfd, newsockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation error");
        exit(1);
    }

  int optval = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
    perror("setsockopt");
    exit(1);
  }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(22663);  // Choose a suitable port
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding error");
        exit(1);
    }

    if (listen(sockfd, 10) == -1) {
        perror("Listening error");
        exit(1);
    }

    printf("Server is listening...\n");

    addr_size = sizeof(clientAddr);
    newsockfd = accept(sockfd, (struct sockaddr *)&clientAddr, &addr_size);
    if (newsockfd == -1) {
        perror("Accepting error");
        exit(1);
    }
 
    printf("Client connected \n");
    int isauthenticated = 0;

    while (!isauthenticated) {
     
        char receivedData[100]; // Adjust the size as needed
        int response = 2; // Default response is User not found

        // Receive combined string from client
        ssize_t bytesReceived = recv(newsockfd, receivedData, sizeof(receivedData), 0);
        if (bytesReceived <= 0) {
            perror("Error receiving data");
            close(newsockfd);
            close(sockfd);
            exit(1);
        }

    // Extract the components from receivedData
    char cmd [4];
    char receivedLengthHex[5];
    char receivedIdHex[9];
    char receivedPasswordHashHex[SHA_DIGEST_LENGTH * 2 + 1 + 1]; // +1 for null terminator

    sscanf(receivedData, "%2s%2s%8s%64s", receivedLengthHex, cmd, receivedIdHex,receivedPasswordHashHex);
         printf("receivedLengthHex%s\n", receivedLengthHex);
          printf("cmd  %s\n", cmd);
         printf("receivedIdHex  %s\n", receivedIdHex);
         printf("receivedPasswordHashHex  %s\n", receivedPasswordHashHex);
         printf("receivedData  %s\n", receivedData);

    // Convert receivedLengthHex to an integer
    size_t receivedLength;
    sscanf(receivedLengthHex, "%zx", &receivedLength);

    // Convert receivedIdHex and receivedPasswordHashHex to their original forms
    char receivedId[5];
    char receivedPasswordHash[SHA_DIGEST_LENGTH];
    for (int i = 0; i < (int)(strlen(receivedIdHex) )/ 2; i++) {
        sscanf(receivedIdHex + 2 * i, "%2hhx", &receivedId[i]);
    }
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sscanf(receivedPasswordHashHex + 2 * i, "%2hhx", &receivedPasswordHash[i]);
    }

    receivedId[4] = '\0';

    printf("Received User ID: %s\n", receivedId);
    //printf("Received Password Hash: %s\n", receivedPasswordHashHex);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].id, receivedId) == 0) {
            if (memcmp(users[i].passwordHash, receivedPasswordHashHex, SHA_DIGEST_LENGTH) == 0) {
                response = 0; // Authentication successful
               isauthenticated=1;

            } else {
                response = 1; // Wrong password
            }
            break;
        }
    }

    // Send response to client
    send(newsockfd, &response, sizeof(response), 0);
}close(newsockfd);
               close(sockfd);
    return 0;
 }
   