#include <stdio.h>

int main (void){
    int choice = 0;
    char name = 0;

// Starting the Program
    printf("Stock Market Tracker & Portfolio\n");
    printf("What is your name?\n");
    scanf("%s", &name);

// Prompting the user input

    printf("What would you like to do?\n1. View Portfolio\n2. Purchase Stocks\n3. Sell Stocks\n4. View Market Prices\n");
    scanf("%d", &choice);
    if (choice == 1){
        puts("Your Portfolio");
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