#define CURL_STATICLIB
#include <vector>
#include <iostream>
#include <windows.h>
#include <curl\curl.h>
#include <nlohmann\json.hpp>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}



int get_rates(std::map<std::string, std::vector<float> >& rates) {

	static int counter = 0;
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


		std::vector<float> rate;
		std::string key;
		float val;

		for (auto& el : jdata["Valute"]) {
			std::cout << el["CharCode"] << ":" << el["Value"] << std::endl;

			el["CharCode"].get_to(key);
			el["Value"].get_to(val);

			if (counter == 0) {

				rate.push_back(val);
				rates[key] = rate;
				rate.clear();
			}
			else {
				rates[key].push_back(val);
			}
		}
		counter++;
		std::cout << "================= " << counter * 10 << "sec" << std::endl;
		return(0);
	}
	return(1);

}


int divz(int a, int b)
{
	return a / b;
};

int main()
{

	bool stop = false;
	std::map<std::string, std::vector<float> > rates;
	float avg = 0;
	float median = 0.0;

	while (1) {

		if (!stop) {
			get_rates(rates);
		}
		else {
			std::cout << "ESC pressed!\n";

			for (auto const& x : rates)
			{
				std::cout << x.first << " ";

				for (int i = 0; i < x.second.size(); i++) {
					avg = avg + x.second[i];
				}

				if (x.second.size() % 2 == 0) {
					median = (x.second[divz(x.second.size(), 2)] + x.second[divz(x.second.size(), 2) + 1]) / 2;
				}
				else {
					if (x.second.size() != 1) median = x.second[divz(x.second.size(), 2) + 1];
					else median = x.second[0];
				}

				avg = avg / x.second.size();
				std::cout << "average: " << avg << " median: " << median << std::endl;
				avg = 0;
				median = 0.0;
			}

			exit(0);
		}

		for (int t = 0; t < 100; t++) {
			Sleep(100);
			if (GetAsyncKeyState(VK_ESCAPE)) { stop = true; break; }
		}
	}

	return 0;
}
