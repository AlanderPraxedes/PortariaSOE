#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

const std::string TOKEN = "5802864819:AAHfljVgO6D1GFvr7KjXBG769UDqzq-LkXM";

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* buffer) {
    size_t totalSize = size * nmemb;
    buffer->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

void send_message(const std::string& chat_id, const std::string& text) {
    std::string url = "https://api.telegram.org/bot" + TOKEN + "/sendMessage";
    std::string postData = "chat_id=" + chat_id + "&text=" + text;

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            std::cout << "Mensagem enviada com sucesso!" << std::endl;
        } else {
            std::cout << "Falha ao enviar a mensagem." << std::endl;
        }

        curl_easy_cleanup(curl);
    }
}

int main() {
    std::ifstream file("placas.json");
    if (!file.is_open()) {
        std::cout << "Falha ao abrir o arquivo JSON." << std::endl;
        return 1;
    }

    json data;
    file >> data;
    file.close();

    std::string placa = "DEF9012";
    bool placaEncontrada = false;

    for (const auto& obj : data) {
        if (obj["placa"] == placa) {
            std::string chat_id = obj["id_telegram"];
            std::string message_text = "Olá! Esta é uma mensagem enviada pelo bot do Telegram.";
            send_message(chat_id, message_text);
            placaEncontrada = true;
            break;
        }
    }

    if (!placaEncontrada) {
        std::cout << "Placa não encontrada no arquivo JSON." << std::endl;
    }

    return 0;
}
