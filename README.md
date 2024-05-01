
***POS (Point of Sale) Automation System and Server Management***


The server ensures a secure Server-Client connection through TCP/IP, utilizing robust user authentication with SHA1 password hashing. Interacting with an SQL database following SQL-92 standards, it manages user data transactions securely. 
The Sale Application efficiently manages folder creation and database setup, establishing essential tables like Product, Receipt, and ReceiptDetails. It ensures a logical sales process with dynamic updates to the total and details. The flexible payment system supports both cash and credit transactions, while comprehensive summary and detail reports in JSON format maintain synchronized data with the server.



 ***Server Application***

 
 The working logic of the server section is as follows:
 
 • Uponstartup, the client application will prompt the user to input the IP address and
 port number. These will serve as the connection details for the server. After obtaining
 the IP and port information, the client application will request the user to enter their
 user ID and password.
 
 • Theclient application will hash the provided user ID and password and send them to
 the server over TCP.
 
 • Theserver application, on startup, will initiate a TCP server to listen for incoming
 connections from client
 
 • Whenaclient connects, the server will process the incoming message: The server will
 search for the user ID received in the message.
 
 • If the ID is found, the server will retrieve the corresponding password and calculate
 its hash(SHA1).
 
 • Theserver will compare the hash from the request message with the calculated hash.
 • If the hashes match, the server will fetch the user ID and password from the database
 and response send them to the client

• If the ID is not found or the passwordhash doesn’t match, the server will send an
 appropriate error code


***Sale Application***


 The working logic of the sale section is as follows:
 
 • The application verifies the existence of the folders upon launch. The application
 creates the directories if none already exist.
 
         1- program folder 2- report folder

 
 • At startup, application checks the database. If "cashregister.db" does not exists, it
 creates a database with the name of "cashregister.db"
 
 Puts following tables to "cashregister.db" database
 
 1- Product Table Columns:
 
         PluNo (int), Name (Text), Price (int), VatRate(int), Barcode (Text)
 
 2- Receipt Table Columns:
 
         ReceiptNo(int), ReceiptTotal(int), CashPayment(int), CreditPayment(int)
 
 3- ReceiptDetails Table Columns:
 
         ReceiptNo(int), PluNo (int), PluName(Text), Price(int)

• Atstartup, program lists 3 menu items

         1- Program 2- Sale 3- Report 4- Exit
 
 • if user selects 1, program checks the file "product.txt" in "program folder".if there is
 no such file,program prints "file is not found". The message is displayed for 2 seconds.
 Then, prints program menu items again:
 
          1- Sync Product 2- Back to Main
 
 if there is "product.txt" in the "program folder", the program reads the file.Sample data
 in "product.txt" 1, MOSFET, 100, 8, 1234567890123.Each line in the file is parsed to
 get PluNo, Name, Price, VatRate, Barcode.Each line in the file is parsed to get PluNo,
 Name, Price, VatRate, Barcode.After save operation is completed, the application
 displays the "record success" message for 2 seconds
 
 • If the user selects 2, the following menu items will be displayed:
 
          1- Sale Product 2- Pay 3- Close Receipt 4- Back to Main
 
 If the user chooses 1, the program prompts the user to enter the PLU number: "Enter
 PLU Number." The program then searches the database for the product based on
 the value entered by the user. If the record is not found, the program displays an
 error message, "Product not found," for 2 seconds. If the record is found, the program
 adds a record to the ReceiptDetails table using newReceiptNr and the product’s PLU
 number, name, and price values. Additionally, it adds the price of the product to the
 receiptTotal variable, and a "Product sale success" message is displayed for 2 seconds.
 After either displaying the "Product not found" or "Product sale success" messages,
 the sale menu is displayed again.
 
 If the user selects 2, the program prompts the user
 for the payment type. "Enter Payment Type: 1-Cash 2-Credit"
 if user selects 1 or 2, then user asks for amount : "Enter Amount: If the user selects 3,
 newReceiptNr will be increased. and receiptTotal will be set to zero. if 4 is selected,
 turn backs and the main menu is displayed again:
 
 • If the user selects 3, the following menu items are displayed:
 
           1- Summary 2-Detail 3- Back to Main
 
 If "Summary" is selected, the program retrieves records from the Receipt table and
 writes them into "summary.txt" in the "report" folder. If "summary.txt" already exists
in the folder, it is first deleted, and then a new one is created. The summary report
 contains lines for each record in the Receipt table in JSON format.
 
 If "Detail" is selected, the program retrieves records from the Receipt table joined with
 the ReceiptDetails table and writes them into "details.txt" in the "report" folder. If
 "details.txt" already exists, it is first deleted, and then a new one is created. The detail
 report contains lines for each record found by joining the Receipt and ReceiptDetails
 tables

***Flowchart of the project***

![saleclient (4)](https://github.com/Erencskun/Saleclient/assets/141026983/c0c74df1-92cb-4fca-b195-42a23a255c35)





To run the code, run the command "make -f Makefile1 run" in the file containing the code

Server IP address: 127.0.0.1 (same device)
Server port number: 22663

If you want to increase the user information, you can write it in the corresponding line in the code or you can enter 
it yourself in the database.

Currently registered Ids and passwords:

"('0479', 'helloworld'),"
"('0550', 'password'),"
 "('8795', 'merhaba'),"




