#include <stdio.h>
#include "database.h"
#include "program.h"
#include "report.h"
#include "sale.h"
#include"Createfolders.h"
int main() {
    int choice;
  Checkfolders() ;
  createdatabase();
    do {
        // Display the menu
        printf("\nMenu:\n");
        printf("1 - Program\n");
        printf("2 - Sale\n");
        printf("3 - Report\n");
        printf("4 - Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        // Process user's choice
        switch (choice) {
            case 1:
                prgmProcess();
                break;
            case 2:
                saleProcess();
                break;
            case 3:
                reptProcess();
                break;
            case 4:
                printf("Exiting the application.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);

    return 0;
}
