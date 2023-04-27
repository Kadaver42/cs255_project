#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <pthread_compat.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 2
/*GLOBAL VAR
 *
 * */

CURL *hnd;
CURL *hnd2;

struct string { // c string things
    char *ptr;
    size_t len;
};

void init_string(struct string *s) { // creation of string struct used for results. allocating memory and the likes
    s->len = 0;
    s->ptr = malloc(s->len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

char* subString (const char* input, int offset, int len, char* dest) { // copy substring of any length from one location on a string to another.
    int input_len = strlen (input);

    if (offset + len > input_len)
    {
        return NULL;
    }

    strncpy (dest, input + offset, len);
    return dest;
}

void chop(char *str, size_t n) { // remove specified amount of characters from the beginning of string.
    assert(n != 0 && str != 0);
    size_t len = strlen(str);
    if (n > len)
        return;  // Or: n = len;
    memmove(str, str+n, len - n + 1);
}


struct string apiRes;// global var because im lazy.
char url[1000];





size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) //function to properly allocate mem for the incoming data string.
{
    size_t new_len = s->len + size*nmemb;  // get length of results
    s->ptr = realloc(s->ptr, new_len+1); // alloc apt mem
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size*nmemb;
}



struct stock{ // struct to store portfolio data. each struck holds all pertinent info for a single instance of a stock.
    char owner[30];
    char tick[6];
    double price;
    int qty;
    double put;
    double call;
};



struct stock port[100]; // create global portfolio of stocks.
int numStock = 0;

char name[30] = "";

void priceUpt(char *data  ){ // parse api output and update the current market price of all stocks in the portfolio.

    char *sub; // substring


    for (int i =0; i < sizeof(port)/sizeof(port[0]) ; i++ ){ // loop through all stocks in portfolio. ORDER MATTERS. Order relies on api call being augmented correctly.
        int index;
        sub = strstr(data, "regularMarketPrice" );//pointer for substring for start of market price
        if (sub == NULL)
            continue;

        index = sub-data + 21; //pointer subtraction to find index of substring in original string. Add length of trigger word

        subString(data, index, 6, sub ); // get actual substring of price.

        port[i].price = round(strtod(sub,NULL) * 100.0) / 100.0; //convert string to double and force rounding to 2 decimal places store in stock.

        chop(data, index + 6); // remove all data from string that has already been parsed.

    }

    //free(data->ptr);

}

_Noreturn void *portUpdate(void *vargp){
    sleep(1);

   // int *threadId = (int *)vargp;
    //printf("THREAD: %d has started updating!\n", 3);

    time_t  rawTime = time(NULL);
    struct tm *local = localtime(&rawTime);

    while (1){

        if(local->tm_wday == 0 || local->tm_wday == 6){
            sleep(3600);
            puts("weekend update");
        }
        else if ((local->tm_hour > 8 && local->tm_min < 30) || local->tm_hour < 8 || local->tm_hour > 3 ){
            sleep(1080);
            puts("after hours update");
        }
        else {
            sleep(1080);
            puts("Market update");
            struct string temp;
            init_string(&temp);
            curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &temp);
            curl_easy_perform(hnd);
            sleep(5);
            priceUpt(temp.ptr);

            for (int i = 0; i < sizeof(port) / sizeof(port[0]); i++){
                if(port[i].price < port[i].call ) {
                    port[i].qty = 0;
                    printf("Call option executed for %s @ %.2f\n",port[i].tick,port[i].price);
                }
                if(port[i].price > port[i].put){
                    port[i].qty = 0;
                    printf("Put option executed for %s @ %.2f\n",port[i].tick,port[i].price);
                }
            }
        }


    }
}

void buyStock( char nm[30]  ){


    struct stock buy;
    int owned = -1;
    int menu = 0;
    puts("Enter Ticker Symbol!");
    scanf("%s", buy.tick);
    puts("Enter Quantity to buy!");
    scanf("%d", &buy.qty);
    puts("Would you like to assign a call price?");
    printf("1. Yes\n2. No\n");
    scanf("%d", &menu);
        if(menu == 1){
            puts("Enter call price");
            scanf("%lf", &buy.call);
        }
    menu = 0;
    puts("Would you like to assign a put price?");
    printf("1. Yes\n2. No\n");
    scanf("%d", &menu);
        if(menu == 1){
            puts("Enter put price");
            scanf("%lf", &buy.put);
        }
    menu = 0;

    for (int i = 0; i < sizeof(port) / sizeof(port[0]); i++) {
        if (strcmp(port[i].tick, buy.tick) == 0 && strcmp(port[i].owner, nm) == 0 ) {
            owned = i;
        }
    }

    if (owned == -1){

        strcpy(buy.owner, nm);

        port[numStock] = buy;

        numStock++;

        strcat(buy.tick, "%2C");
        strcat(url, buy.tick);
        curl_easy_setopt(hnd, CURLOPT_URL, url);
       // printf("%s\n", url);

    }
    else {

        port[owned].qty += buy.qty;
        port[owned].call = buy.call;
        port[owned].put = buy.put;
    }
    CURLcode res2;
    struct string temp;
    init_string(&temp);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &temp);
    curl_easy_perform(hnd);
    //printf("CODE: %d \n",res2);
    puts("Updating prices.");
    sleep(7);
    //print string data
    priceUpt(temp.ptr);
    //printf("%s\n", temp.ptr);
    puts("Success!");
}

void sellStock( char nm[30]  ){
    char tick[6];
    int qty;
    int owned = 0;
    puts("Enter Ticker Symbol!");
    scanf("%s", tick);
    puts("Enter Quantity to sell!");
    scanf("%d", &qty);

    for (int i = 0; i < sizeof(port) / sizeof(port[0]); i++) {
        if (strcmp(port[i].owner, nm) == 0) {
            if(strcmp(port[i].tick,tick)==0){
                port[i].qty -= qty;
                owned = 1;
            }
        }
    }
    if(owned == 1)
        puts("Success!");

    else
        puts("Sorry you do not own this stock!");

}

void lookup(char tick[6]){
    char url2[1000] = "https://mboum-finance.p.rapidapi.com/qu/quote?symbol=";
    strcat(url2, tick);

    curl_easy_setopt(hnd2, CURLOPT_URL, url2);

    struct string temp;
    init_string(&temp);
    curl_easy_setopt(hnd2, CURLOPT_WRITEDATA, &temp);
    CURLcode res = curl_easy_perform(hnd2);


    char *sub; // substring
    int index;
    sub = strstr(temp.ptr, "regularMarketPrice" );//pointer for substring for start of market price
    index = sub-temp.ptr + 21; //pointer subtraction to find index of substring in original string. Add length of trigger word


    subString(temp.ptr, index, 6, sub ); // get actual substring of price.

    printf("The price of %s is $%.2f\n",tick,round(strtod(sub,NULL) * 100.0) / 100.0);

}

int main() {
    malloc(sizeof(struct stock));
    int choice = 0;



    CURLcode res;
    hnd = curl_easy_init();  //start curl
    hnd2 = curl_easy_init();
     // make string
    init_string(&apiRes);

    strcpy(url, "https://mboum-finance.p.rapidapi.com/qu/quote?symbol=");

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd2, CURLOPT_CUSTOMREQUEST, "GET");

    curl_easy_setopt(hnd, CURLOPT_URL, url); // curl setup to pull from finance api

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "X-RapidAPI-Key: 4b7b4a34bamshfd1fdd84e4fd488p1e26e4jsnd492e851a547");
    headers = curl_slist_append(headers, "X-RapidAPI-Host: mboum-finance.p.rapidapi.com"); //api keys
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(hnd2, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(hnd2, CURLOPT_WRITEFUNCTION, writefunc);//write data

    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &apiRes);
    res = curl_easy_perform(hnd);
    //printf("CODE: %d \n",res);
    //printf("%s\n", apiRes.ptr); //print string data

    pthread_t threads[NUM_THREADS];
    priceUpt(apiRes.ptr);
    sleep(6);
    int rc = pthread_create(&threads[3], NULL, portUpdate, (void *)3);

    if(rc) {
        puts("error");
        exit(-1);
    }

    printf("Stock Market Tracker & Portfolio\n");
    printf("Enter your username\n");

    fgets(name, 30, stdin);  //buffer protected input (can handle spaces)
    if ((strlen(name) > 0) && (name[strlen (name) - 1] == '\n')) //delete newline
        name[strlen (name) - 1] = '\0';


// Starting the Program


// Prompting the user input
    while(choice != 6){

        printf("What would you like to do?\n1. View Portfolio\n2. Purchase Stocks\n3. Sell Stocks\n4. View Market Prices\n5. Sign Out\n6. Exit\n");
        scanf("%d", &choice);

        if (choice == 1) {
            printf("%s's Portfolio\n", name);
            for (int i = 0; i < sizeof(port) / sizeof(port[0]); i++) {
                if (strcmp(port[i].owner, name) == 0 && port[i].qty > 0) {
                    printf("%d: %s -- $%4.2f || QTY: %d @ $%4.2f each || Call: $%4.2f Put $%4.2f\n", i + 1, port[i].tick, port[i].price*port[i].qty,port[i].qty, port[i].price,port[i].call,port[i].put);
                }
            }
            printf("\n");
        } else if (choice == 2) {
            puts("Buy Stock");
            buyStock(name);

        } else if (choice == 3) {
            puts("Sell Stocks");
            sellStock(name);

        } else if (choice == 4) {
            puts("View Prices");
            puts("Enter Ticker Symbol!");
            char tick[6];
            scanf("%s", tick);
            lookup(tick);


        } else if (choice == 5) {
            puts("Signed Out");
            printf("Enter your username\n");
            fgets(name, 30, stdin);  //buffer protected input (can handle spaces)
            if ((strlen(name) > 0) && (name[strlen (name) - 1] == '\n')) //delete newline
                name[strlen (name) - 1] = '\0';

        } else if (choice == 6) {
            puts("Exit");
        } else {
            puts("Not a valid response.");
        }

        sleep(3);
    }


    /* always cleanup */
    curl_easy_cleanup(hnd);
    free(apiRes.ptr); //free string
    //pthread_exit(NULL);
    //puts("clean");

    return 0;
}
