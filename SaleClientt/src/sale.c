#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h>
#include "program.h"
#include "report.h"
#include "sale.h"
#include "database.h"
int newReceiptNr = 0;
int lastReceiptNr = 0;
int receiptTotal = 0;

int getLastReceiptNumber(sqlite3 *db)
{
    int lastReceiptNr = 0;

    char query[100] = "SELECT MAX(ReceiptNo) FROM Receipt;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (rc == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            lastReceiptNr = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    else
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }

    return lastReceiptNr;
}
void saleProcess()
{
    int choice;

    sqlite3 *db;
    int rc = sqlite3_open("cashregister.db", &db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    lastReceiptNr=getLastReceiptNumber(db);
    newReceiptNr = lastReceiptNr + 1;
    receiptTotal = 0;

    do
    {
        printf("\nSale Menu\n");
        printf("1 - Sale Product\n");
        printf("2 - Pay\n");
        printf("3 - Close Receipt\n");
        printf("4 - Back to Main\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("Enter Plu Number: ");
            int pluNo;
            scanf("%d", &pluNo);
            char selectQuery[200];
            snprintf(selectQuery, sizeof(selectQuery), "SELECT * FROM Product WHERE PluNo=%d;", pluNo);
            // query varıable to select details of the product from the product table ın the database
            rc = sqlite3_exec(db, selectQuery, NULL, NULL, NULL);
            if (rc == SQLITE_OK)
            { // it means executed successfully
                // Fetch the product details
                sqlite3_stmt *stmt;
                rc = sqlite3_prepare_v2(db, selectQuery, -1, &stmt, 0); // same sqlite3_exec
                if (rc == SQLITE_OK)
                { // query derler db ıle ılıskılendırır stmt poınterda saklanır
                    if (sqlite3_step(stmt) == SQLITE_ROW)
                    {                                                             // it means there ıs a row ın the result set and
                                                                                  // code proceeds to extract data from that row
                        int productPluNo = sqlite3_column_int(stmt, 0);           // first column ındex 0
                        char *productName = (char *)sqlite3_column_text(stmt, 1); // second column ındex 1
                        int productPrice = sqlite3_column_int(stmt, 2);
                     char insertQuery[200];
                        // Add the product to ReceiptDetails table
                        snprintf(insertQuery, sizeof(insertQuery), "INSERT INTO ReceiptDetails (ReceiptNo, PluNo, PluName, Price) VALUES (%d, %d, '%s', %d);", newReceiptNr, productPluNo, productName, productPrice);

                        rc = sqlite3_exec(db, insertQuery, 0, 0, 0);
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db)); // error
                            break;                                                  // program will go back to main menu
                        }

                        receiptTotal += productPrice;
                        printf("Product sale success.\n");
                    }
                    else
                    {
                        printf("Product has not found.\n");
                    }
                    sqlite3_finalize(stmt);
                }
            }
            else
            {
                fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
            }

            sleep(2);
            // Wait for 2 seconds
            break;

        case 2:
            printf("Enter Payment Type: 1-Cash 2-Credit: ");
            int paymentType;
            scanf("%d", &paymentType);

            printf("Enter Amount: ");
            int amount;
            scanf("%d", &amount);

            if (newReceiptNr > 0)
            {
                char checkQuery[10000];

                // Check if this is the first payment for this receipt
                snprintf(checkQuery, sizeof(checkQuery), "SELECT * FROM Receipt WHERE ReceiptNo=%d;", newReceiptNr);
                // selects all columns from the receıpt table where the receıpt no matches the generated receipt number
                rc = sqlite3_exec(db, checkQuery, NULL, NULL, NULL);

                if (rc == SQLITE_OK)
                {
                    // Fetch the receipt details
                    sqlite3_stmt *stmt;
                    rc = sqlite3_prepare_v2(db, checkQuery, -1, &stmt, 0);

                    if (rc == SQLITE_OK)
                    {
                        // Step through the result (if any) to check if receipt exists
                        if (sqlite3_step(stmt) != SQLITE_ROW)
                        {   //
                            // first payment for this receipt
                            if (paymentType == 1)
                            {
                                snprintf(checkQuery, sizeof(checkQuery), "INSERT INTO Receipt (ReceiptNo, ReceiptTotal, CashPayment, CreditPayment) VALUES (%d, %d, %d, 0);", newReceiptNr, receiptTotal, amount);
                            }
                            else if (paymentType == 2)
                            {
                                snprintf(checkQuery, sizeof(checkQuery), "INSERT INTO Receipt (ReceiptNo, ReceiptTotal, CashPayment, CreditPayment) VALUES (%d, %d, 0, %d);", newReceiptNr, receiptTotal, amount);
                            }
                        }
                        else
                        {
                            // not the first payment, update accordingly // receıpt already existsw
                            if (paymentType == 1)
                            {
                                snprintf(checkQuery, sizeof(checkQuery), "UPDATE Receipt SET CashPayment=%d WHERE ReceiptNo=%d;", amount, newReceiptNr);
                            }
                            else if (paymentType == 2)
                            {
                                snprintf(checkQuery, sizeof(checkQuery), "UPDATE Receipt SET CreditPayment=%d WHERE ReceiptNo=%d;", amount, newReceiptNr);
                            }
                        }
                        sqlite3_finalize(stmt);
                    }
                    else
                    {
                        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
                    }
                }
                else
                {
                    fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
                }

                // Execute the final SQL query (either insert or update)
                rc = sqlite3_exec(db, checkQuery, NULL, NULL, NULL);
                if (rc != SQLITE_OK)
                {
                    fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
                }
            }
            sleep(2); // Wait for 2 seconds
            break;
        case 3:
            newReceiptNr += 1;
            receiptTotal = 0;
            break;
        case 4:
            printf("Returning to the main menu.\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);
    sqlite3_close(db);
}
