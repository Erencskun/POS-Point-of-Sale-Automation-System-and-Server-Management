#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sqlite3.h"
#include "program.h"

void prgmProcess() {
    int choice;

    do {
        printf("\nProgram Menu\n");
        printf("1 - Sync Product\n");
        printf("2 - Back to Main\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                syncProduct();
                break;
            case 2:
                printf("Returning to the main menu.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 2);
}

void syncProduct() {
    FILE* file = fopen("program/product.txt", "r");
    if (file == NULL) {
        printf("product.txt file is not found.\n");
        sleep(2); // Delay ,Wait for 2 seconds
        return;
    }
    // Read the file
    int pluNo, price, vatRate;
    char name[100], barcode[200];

    sqlite3* db;
    int rc = sqlite3_open("cashregister.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
  while (fscanf(file, "%d, %99[^,], %d, %d, %99s\n", &pluNo, name, &price, &vatRate, barcode) == 5) {
     // %99[^,],meanıng read ınput until a comma character,max 99 character
        //%99s it reads up  99 non whıtesapace character
        char insertQuery[500];
     snprintf(insertQuery, sizeof(insertQuery), "INSERT INTO Product (PluNo, Name, Price, VatRate, Barcode) VALUES (%d, '%s', %d, %d, '%s')", pluNo, name, price, vatRate, barcode);
        // create sql query strıng and store ıt ın the insertquery array
        rc = sqlite3_exec(db, insertQuery, 0, 0, 0);   // check success or faılure of the query execution
        if (rc != SQLITE_OK) {                    // if rc =0 meanıng that it has a problem
            fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
            continue;                
        }
        printf("Record with PluNo %d saved successfully.\n", pluNo);
        sleep(2); // Wait for 2 seconds
    }
    sqlite3_close(db);
    fclose(file);
}