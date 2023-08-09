#include <stdio.h>
#include <sys/stat.h>

void Checkfolders() {
    // Check and create the program folder
    if (mkdir("program", 0777) == 0) {
        printf("Created 'program' folder.\n");
    } else {
        printf("'program' folder already exists.\n");
    }

    // Check and create the report folder
    if (mkdir("report", 0777) == 0) {
        printf("Created 'report' folder.\n");
    } else {
        printf("'report' folder already exists.\n");
    }
}
