#define CURL_STATICLIB
#include <iostream>
#include <windows.h>
#include <curl\curl.h>
#include <nlohmann\json.hpp>


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main()
{


    
    while (1) {

        std::string readBuffer;
        CURL* curl;

        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.cbr-xml-daily.ru/daily_json.js"); // задаем url на нужный json
        curl_easy_setopt(curl, CURLOPT_POST, 0); // выключаем POST
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);// включаем GET
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // функция вызываемая для записи полученных данных в буфер
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer); // указатель на буфер с данными куда их писать

        CURLcode res = curl_easy_perform(curl); //  Выполняем запрос
        curl_easy_cleanup(curl); // очистка

        if (res == 0) { // проверяем код ошибки, если 0, то все ок
            auto jdata = nlohmann::json::parse(readBuffer); // преобразуем полученную JSON строку в объект 
            std::cout << jdata << std::endl;
        }

        Sleep(1000);

    }
    
    return 0;
}