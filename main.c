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
    double qty;
    double put;
    double call;
};

struct stock port[10]; // create global portfolio of stocks.

void priceUpt(struct string *data  ){ // parse api output and update the current market price of all stocks in the portfolio.

    char *sub; // substring

    for (int i =0; i < sizeof(port)/sizeof(port[0]) ; i++ ){ // loop through all stocks in portfolio. ORDER MATTERS. Order relies on api call being augmented correctly.
        int index;
        sub = strstr(data->ptr, "regularMarketPrice" );//pointer for substring for start of market price
        if (sub == NULL)
            continue;

        index = sub-data->ptr + 21; //pointer subtraction to find index of substring in original string. Add length of trigger word

        subString(data->ptr, index, 6, sub ); // get actual substring of price.

        port[i].price = round(strtod(sub,NULL) * 100.0) / 100.0; //convert string to double and force rounding to 2 decimal places store in stock.

        chop(data->ptr, index + 6); // remove all data from string that has already been parsed.

    }



}

_Noreturn void *portUpdate(void *vargp){
    sleep(1);

   // int *threadId = (int *)vargp;
    //printf("THREAD: %d has started updating!\n", 3);

    priceUpt(&apiRes);

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
            curl_easy_perform(hnd);
            sleep(5);
            priceUpt(&apiRes);
        }


    }
}

int main() {
    malloc(sizeof(struct stock));
    int choice = 0;
    char name[30] = "";

    for (int i =0; i < sizeof(port)/sizeof(port[0]) ; i++ ){
        strcpy(port[i].owner, "name");
        }

    CURLcode res;
    hnd = curl_easy_init();  //start curl

     // make string
    init_string(&apiRes);

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_URL, "https://mboum-finance.p.rapidapi.com/qu/quote?symbol=AAPL,TSLA,GOOGL"); // curl setup to pull from finance api this specific one gets aapl

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "X-RapidAPI-Key: 4b7b4a34bamshfd1fdd84e4fd488p1e26e4jsnd492e851a547");
    headers = curl_slist_append(headers, "X-RapidAPI-Host: mboum-finance.p.rapidapi.com"); //api keys
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);


    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writefunc); //write data
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &apiRes);
    res = curl_easy_perform(hnd);

    //printf("%s\n", apiRes.ptr); //print string data

    pthread_t threads[NUM_THREADS];
    priceUpt(&apiRes);
    sleep(6);
    int rc = pthread_create(&threads[3], NULL, portUpdate, (void *)3);

    if(rc) {
        puts("error");
        exit(-1);
    }

    printf("Stock Market Tracker & Portfolio\n");
    printf("What is your name?\n");

    fgets(name, 30, stdin);  //buffer protected input (can handle spaces)
    if ((strlen(name) > 0) && (name[strlen (name) - 1] == '\n')) //delete newline
        name[strlen (name) - 1] = '\0';



    strcpy(port[0].owner, name);

    strcpy(port[0].tick, "APPL");

    strcpy(port[1].owner, name);
    strcpy(port[1].tick, "TSLA");



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




    /* always cleanup */
    curl_easy_cleanup(hnd);
    free(apiRes.ptr); //free string
    //pthread_exit(NULL);
    //puts("clean");

    while(1){
        sleep(10);
    }
    return 0;
}
