#include <stdio.h>
#include <string.h>

int main (void){
    int choice = 0;
    char name = 0;

    char port[10][6]  = {"aapl" , "tsla"}; // array of portfolio ticker symbols Will be updated when a stock is sold or bought. Current max: 10

    /*  ****** This is currently a hard coded place-holder ******
     *  To limit the amount of api calls made during development this file will not update live
     *  This is also to avoid having to install CURL on every computer we wish to build on
     *  The final project behavior will be the same and api implementation will not change code developed with this place-holder
    */
    double portPrice[10] ={147.92, 460.33}; // array of portfolio prices. prices will be updated via api call.

// Starting the Program
    printf("Stock Market Tracker & Portfolio\n");
    printf("What is your name?\n");
    scanf("%s", &name);

// Prompting the user input

    printf("What would you like to do?\n1. View Portfolio\n2. Purchase Stocks\n3. Sell Stocks\n4. View Market Prices\n");
    scanf("%d", &choice);
    if (choice == 1){
        puts("Your Portfolio");

        for (int i = 0; i < sizeof(port) / sizeof(port[0]); i++ ){
            if (strcmp(port[i],""))
            printf("%d: %s -- $%4.2f\n", i+1, port[i], portPrice[i]);
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