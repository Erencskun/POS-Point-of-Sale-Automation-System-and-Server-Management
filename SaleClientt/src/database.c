#include <stdio.h>
#include <sqlite3.h>
#include "program.h"
#include "report.h"
#include "sale.h"

 void createdatabase() {
    sqlite3 *db; // I declare pointer to a sqlite3 
    char *errMsg = 0; // pointer,hata mesajının tutması için
    
    int rc = sqlite3_open("cashregister.db", &db); //create the database
    //0 indicates success 1 ındıcates error
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        //return(0);
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }
    
    // SQL statements to create the tables
    const char *productTableSQL = 
        "CREATE TABLE IF NOT EXISTS Product (" \
        "PluNo INTEGER PRIMARY KEY," \
        "Name TEXT NOT NULL," \
        "Price INTEGER NOT NULL," \
        "VatRate INTEGER NOT NULL," \
        "Barcode TEXT NOT NULL);";

    const char *receiptTableSQL =
        "CREATE TABLE IF NOT EXISTS Receipt (" \
        "ReceiptNo INTEGER PRIMARY KEY," \
        "ReceiptTotal INTEGER NOT NULL," \
        "CashPayment INTEGER NOT NULL," \
        "CreditPayment INTEGER NOT NULL);";

    const char *receiptDetailsTableSQL =
        "CREATE TABLE IF NOT EXISTS ReceiptDetails (" \
        "ReceiptNo INTEGER NOT NULL," \
        "PluNo INTEGER NOT NULL," \
        "PluName TEXT NOT NULL," \
        "Price INTEGER NOT NULL," \
        "FOREIGN KEY (ReceiptNo) REFERENCES Receipt(ReceiptNo)," \
        "FOREIGN KEY (PluNo) REFERENCES Product(PluNo));";

    // Execute the SQL statements
    rc = sqlite3_exec(db, productTableSQL, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    rc = sqlite3_exec(db, receiptTableSQL, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    rc = sqlite3_exec(db, receiptDetailsTableSQL, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    // Close the database
    sqlite3_close(db);
    
}