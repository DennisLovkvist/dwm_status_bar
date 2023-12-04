#pragma once
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_API_ENDPOINT_LENGTH  128
#define MAX_TICKER_SYMBOL_LENGTH 4
#define MAX_PRICE_FORMATTED_LENGTH 8

typedef struct Crypto Crypto;
struct Crypto
{
    char api_endpoint[MAX_API_ENDPOINT_LENGTH];
    char ticker_symbol[MAX_TICKER_SYMBOL_LENGTH];
    char price_formatted[MAX_PRICE_FORMATTED_LENGTH];
    double price;
};

void fetch_crypro_price_list(char *buffer, int buffer_size);