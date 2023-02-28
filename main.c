#include <stdio.h>
#include <string.h>


struct stock{
    char owner[25];
    char tick[6];
    double price;
};

int main (void){

    int choice = 0;
    char name[25] = "";

    struct stock port[10]; // array of portfolio ticker symbols Will be updated when a stock is sold or bought. Current max: 10

    /*  ****** This is currently a hard coded place-holder ******
     *  To limit the amount of api calls made during development this file will not update live
     *  This is also to avoid having to install CURL on every computer we wish to build on
     *  The final project behavior will be the same and api implementation will not change code developed with this place-holder
    */
    printf("Stock Market Tracker & Portfolio\n");
    printf("What is your name?\n");
    scanf("%s", &name);



    strcpy(port[0].owner, &name);

    strcpy(port[0].tick, "APPL");
    port[0].price = 147.92;

    strcpy(port[1].owner, &name);

    strcpy(port[1].tick, "TSLA");
    port[1].price = 462.45;

    strcpy(port[5].owner, "name");
// Starting the Program


// Prompting the user input

    printf("What would you like to do?\n1. View Portfolio\n2. Purchase Stocks\n3. Sell Stocks\n4. View Market Prices\n");
    scanf("%d", &choice);

    if (choice == 1){
        printf("%s's Portfolio\n", name );
        for (int i =0; i < sizeof(port)/sizeof(port[0]) ; i++ ){
            if (strcmp( port[i].owner, name ) == 0) {
                printf("%d: %s -- %4.2f\n", i+1, port[i].tick, port[i].price);
            }
        }

    }

    else if (choice == 2) {
        puts("Buy Stock");

    }

    else if (choice == 3) {
        puts("Sell Stocks");
    }

    else if (choice == 4) {
        puts ("View Prices");
    }

    else {
        puts("Not a valid response.");
    }


    return (0);
}