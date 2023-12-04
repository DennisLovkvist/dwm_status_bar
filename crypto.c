#include "crypto.h"

void crypto_init(Crypto *crypto, char *ticker_symbol, char *api_endpoint)
{
    int api_endpoint_length = strlen(api_endpoint);
    api_endpoint_length = (api_endpoint_length < MAX_API_ENDPOINT_LENGTH) ? api_endpoint_length : MAX_API_ENDPOINT_LENGTH-1;
    memcpy(&crypto->api_endpoint[0], api_endpoint, sizeof(char)*api_endpoint_length);
    crypto->api_endpoint[api_endpoint_length] = '\0';

    int ticker_symbol_length = strlen(ticker_symbol);
    ticker_symbol_length = (ticker_symbol_length < MAX_TICKER_SYMBOL_LENGTH) ? ticker_symbol_length : MAX_TICKER_SYMBOL_LENGTH-1;
    memcpy(&crypto->ticker_symbol[0], ticker_symbol, sizeof(char)*ticker_symbol_length);
    crypto->ticker_symbol[ticker_symbol_length] = '\0';

    memset(&crypto->price_formatted[0],'\0', sizeof(char)*MAX_PRICE_FORMATTED_LENGTH);

    crypto->price = 0;
}

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) 
{
    Crypto *crypto = (Crypto*)userp;

    char* str = (char*)contents;

    char *token = strtok(str, ",");
    int count = 0;

    while (token != NULL) 
    {
        count++;
        if (count == 7)
        {
            crypto->price = strtod(token, NULL);
            break;
        }
        token = strtok(NULL, ",");
    }


    size_t realsize = size * nmemb;
    return realsize;
}

void update_crypto_price(Crypto *cryptos)
{
    CURL* handles[5];
    CURLM* multi_handle;
    int still_running;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    multi_handle = curl_multi_init();

    for (int i = 0; i < 5; ++i)
    {
        handles[i] = curl_easy_init();

        if (handles[i]) 
        {
            curl_easy_setopt(handles[i], CURLOPT_URL, &cryptos[i].api_endpoint[0]);
            curl_easy_setopt(handles[i], CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(handles[i], CURLOPT_WRITEDATA, &cryptos[i]);


            curl_multi_add_handle(multi_handle, handles[i]);
        }
    }

    curl_multi_perform(multi_handle, &still_running);

    do
    {
        curl_multi_wait(multi_handle, NULL, 0, 1000, NULL);
        curl_multi_perform(multi_handle, &still_running);
    }
    while (still_running);

    for (int i = 0; i < 5; ++i) 
    {
        curl_easy_cleanup(handles[i]);
    }

    curl_multi_cleanup(multi_handle);

    curl_global_cleanup();

}
void format_crypto_price(Crypto *cryptos)
{
    for (int i = 0; i < 5; i++)
    {
        Crypto *crypto = &cryptos[i];

        if(crypto->price > 1000)
        {
            int n = (int)round(crypto->price / 1000.0);
            snprintf(&crypto->price_formatted[0],sizeof(char)*MAX_PRICE_FORMATTED_LENGTH, "%ik", n);

        }
        else if(crypto->price < 1 && crypto->price > 0)
        {
            snprintf(&crypto->price_formatted[0],sizeof(char)*MAX_PRICE_FORMATTED_LENGTH, "%.1f", crypto->price);
        }
        else
        {
            snprintf(&crypto->price_formatted[0],sizeof(char)*MAX_PRICE_FORMATTED_LENGTH, "%.0f", crypto->price);
        }
    }
    
}
void fetch_crypro_price_list(char *buffer, int buffer_size)
{
    Crypto cryptos[5];
    crypto_init(&cryptos[0],"BTC","https://api-pub.bitfinex.com/v2/ticker/tBTCUSD");
    crypto_init(&cryptos[1],"ETH","https://api-pub.bitfinex.com/v2/ticker/tETHUSD");
    crypto_init(&cryptos[2],"ADA","https://api-pub.bitfinex.com/v2/ticker/tADAUSD");
    crypto_init(&cryptos[3],"XMR","https://api-pub.bitfinex.com/v2/ticker/tXMRUSD");
    crypto_init(&cryptos[4],"BAT","https://api-pub.bitfinex.com/v2/ticker/tBATUSD");

    update_crypto_price(&cryptos[0]);
    format_crypto_price(&cryptos[0]);
    
    memset(&buffer[0],'\0', sizeof(char)*buffer_size);

    int caret = 0;
    for (int i = 0; i < 5; i++)
    {
        Crypto *crypto = &cryptos[i];
        int chars_written = snprintf(&buffer[caret],sizeof(char)*buffer_size, "$%s:%s ",&crypto->ticker_symbol[0], &crypto->price_formatted[0]);
        if (chars_written < 0)return;
        caret += chars_written;

    }   
}