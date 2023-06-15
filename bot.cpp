#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace std;
using json = nlohmann::json;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

const string TOKEN = "5802864819:AAHfljVgO6D1GFvr7KjXBG769UDqzq-LkXM"; //Token utilizado.

void send_message(const string& chat_id, const string& text) {
    string url = "https://api.telegram.org/bot" + TOKEN + "/sendMessage";
    json params = {
        {"chat_id", chat_id},
        {"text", text}
    };

    http_client client(url);
    client.request(methods::POST, "", params.dump(), "application/json").then([](http_response response) {
        if (response.status_code() == status_codes::OK) {
            cout << "Mensagem enviada com sucesso!" << endl;
        } else {
            cout << "Falha ao enviar a mensagem." << endl;
        }
    }).wait();
}

int main() {
    
    ifstream file("registro_placas.json"); // Leitura do arquivo das placas
    json data;
    file >> data;

    // Função fictícia para reconhecimento de placa usando OpenCV
    string recognize_license_plate() {
        // Aqui você pode adicionar o código para usar o OpenCV e reconhecer a placa
        
        // Neste exemplo, retornaremos uma placa fictícia
        string placa_lida = "DEF9012";
        return placa_lida;
    }

    // Obter a placa usando o reconhecimento de placa
    string placa = recognize_license_plate();

    // Procura a placa no arquivo JSON
    for (const auto& obj : data) {
        if (obj["placa"] == placa) {
            string chat_id = obj["id_telegram"];
            string message_text = "Olá! Esta é uma mensagem enviada pelo bot do Telegram.";
            send_message(chat_id, message_text);
            break;
        }
    }

    cout << "Placa não encontrada no arquivo JSON." << endl;

    return 0;
}
