#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <unistd.h>
#include "program.h"
#include "report.h"
#include "sale.h"
#include <string.h>

void reptProcess(){
    int choice;
    do
    {
        printf("\nReport Menu\n");
        printf("1 - Summary\n");
        printf("2 - Detail\n");
        printf("3 - Back to Main\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1: // Summary
            writeSummaryReport();
            break;

        case 2: // Detail
            writeDetailReport();
            break;

        case 3: // Back to Main
            printf("Returning to the main menu.\n");
            break;

        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 3);
}

void writeSummaryReport()
{
    sqlite3 *db;
    int rc = sqlite3_open("cashregister.db", &db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // Delete and create a new summary.txt file
    FILE *summaryFile = fopen("report/summary.txt", "w");
    if (summaryFile == NULL)
    {
        fprintf(stderr, "Error opening summary.txt for writing.\n");
        sqlite3_close(db);
        return;
    }

    
    sqlite3_stmt *stmt;// Retrieve records from the Receipt table
    rc = sqlite3_prepare_v2(db, "SELECT * FROM Receipt;", -1, &stmt, 0); //* means all columns
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        fclose(summaryFile);
        return;
    }

    // Recorded to summary.txt in JSON format
    int isFirstRecord = 1; //flag ıs used to determıne fırst record or not 
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int receiptNo = sqlite3_column_int(stmt, 0);
        int receiptTotal = sqlite3_column_int(stmt, 1);
        int cashPayment = sqlite3_column_int(stmt, 2);
        int creditPayment = sqlite3_column_int(stmt, 3);

        if (!isFirstRecord)//check the first record or not 
        {
            fprintf(summaryFile, ",\n");//if it is not the first record a comma and a newline chrachert are written to the file
        }
        isFirstRecord = 0;

        fprintf(summaryFile, "{\n");
        fprintf(summaryFile, "\"ReceiptNo\": %d,\n", receiptNo);
        fprintf(summaryFile, "\"ReceiptTotal\": %d,\n", receiptTotal);
        fprintf(summaryFile, "\"CashPayment\": %d,\n", cashPayment);
        fprintf(summaryFile, "\"CreditPayment\": %d\n", creditPayment);
        fprintf(summaryFile, "}");
    }
// after process all rows code writes a closıng square bracket.thıs means end of the json array
    sqlite3_finalize(stmt);//prepared statement stmt ıs fınalızed usıng sqlite3_finalize
    sqlite3_close(db); // db connectıon ıs closed 
    fclose(summaryFile);
    printf("Summary report generated successfully.\n");
}


void writeDetailReport() {
    sqlite3 *db;
    int rc = sqlite3_open("cashregister.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // Delete and create a new details.txt file
    FILE *detailFile = fopen("report/details.txt", "w");
    if (detailFile == NULL) {
        fprintf(stderr, "Error opening details.txt for writing.\n");
        sqlite3_close(db);
        return;
        }
    // Retrieve records from Receipt join ReceiptDetails table
    char query[200];
    snprintf(query, sizeof(query), "SELECT * FROM Receipt LEFT JOIN ReceiptDetails ON Receipt.ReceiptNo=ReceiptDetails.ReceiptNo;");
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        fclose(detailFile);
        return;
    }

    // Write records to details.txt in JSON format
    int currentReceiptNo = -1; // To keep track of the current receipt number
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int receiptNo = sqlite3_column_int(stmt, 0);
        int receiptTotal = sqlite3_column_int(stmt, 1);
        int cashPayment = sqlite3_column_int(stmt, 2);
        int creditPayment = sqlite3_column_int(stmt, 3);
        int pluNo = sqlite3_column_int(stmt, 5);
        const char *pluName = (const char*)sqlite3_column_text(stmt, 6);
        int price = sqlite3_column_int(stmt, 7);

        if (receiptNo != currentReceiptNo) {
            // Close the previous receipt block if it is not the first receipt-
            if (currentReceiptNo!=-1) {
                fprintf(detailFile, "\n]\n");
                fprintf(detailFile, "},\n");
                //
            }
            
            // Start a new receipt block
            fprintf(detailFile, "{\n");
            fprintf(detailFile, "\"ReceiptNo\": %d,\n", receiptNo);
            fprintf(detailFile, "\"ReceiptTotal\": %d,\n", receiptTotal);
            fprintf(detailFile, "\"CashPayment\": %d,\n", cashPayment);
            fprintf(detailFile, "\"CreditPayment\": %d,\n", creditPayment);
            fprintf(detailFile, "\"ReceiptDetails\": [\n");
            
            currentReceiptNo = receiptNo;
        } else {
            // Separate receipt details with a comma
            fprintf(detailFile, ",\n");  // continue
        }
        
        // Write the receipt detail
        fprintf(detailFile, "{\n");
        fprintf(detailFile, "\"PluNo\": %d,\n", pluNo);
        fprintf(detailFile, "\"PluName\": \"%s\",\n", pluName);
        fprintf(detailFile, "\"Price\": %d\n", price);
        fprintf(detailFile, "}");
    }
    // Close the last receipt block
    if (currentReceiptNo!=-1) {
        fprintf(detailFile, "\n]\n");
        fprintf(detailFile, "}");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    fclose(detailFile);
    printf("Detail report generated successfully.\n");
}