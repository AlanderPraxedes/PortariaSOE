#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace cv;
using namespace std;
using json = nlohmann::json;

const std::string TOKEN = "5802864819:AAHfljVgO6D1GFvr7KjXBG769UDqzq-LkXM";

// Função de callback para escrita dos dados recebidos pela requisição cURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* buffer) {
    size_t totalSize = size * nmemb;
    buffer->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// Função para enviar mensagem pelo Telegram
void enviarMensagem(const std::string& chat_id, const std::string& texto) {
    std::string url = "https://api.telegram.org/bot" + TOKEN + "/sendMessage";
    std::string postData = "chat_id=" + chat_id + "&text=" + texto;

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        std::string resposta;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resposta);

        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            std::cout << "Mensagem enviada com sucesso!" << std::endl;
        } else {
            std::cout << "Falha ao enviar a mensagem." << std::endl;
        }

        curl_easy_cleanup(curl);
    }
}

// Função para capturar uma imagem
void capturarImagem(const string& caminhoArquivo) {
    VideoCapture camera(CAP_V4L2);

    Mat frame;
    if (camera.read(frame)) {
        imwrite(caminhoArquivo, frame);
    }

    camera.release();
}

int main() {
    while(1){
    string caminhoImagem = "imagem.jpg";
    capturarImagem(caminhoImagem);


    // Carregar a imagem
    cv::Mat imagem = cv::imread("imagem.jpg");

    // Verificar se a imagem foi carregada corretamente
    if (imagem.empty()) {
        std::cout << "Falha ao carregar a imagem." << std::endl;
        return -1;
    }

    // Carregar o arquivo XML contendo os dados do classificador de placa de carro
    cv::CascadeClassifier classificadorPlaca;
    if (!classificadorPlaca.load("a.xml")) {
        std::cout << "Falha ao carregar o classificador." << std::endl;
        return -1;
    }

    // Converter a imagem para escala de cinza
    cv::Mat imagemCinza;
    cv::cvtColor(imagem, imagemCinza, cv::COLOR_BGR2GRAY);

    // Realizar a detecção da placa de carro
    std::vector<cv::Rect> placas;
    classificadorPlaca.detectMultiScale(imagemCinza, placas, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

    // Variáveis para armazenar a maior região encontrada
    cv::Rect maiorPlaca;
    int maiorTamanhoPlaca = 0;

    // Desenhar retângulo no contorno da placa e verificar a maior região
    for (const auto& placa : placas) {
        cv::rectangle(imagem, placa, cv::Scalar(0, 255, 0), 2);

        // Verificar o tamanho da região
        int tamanhoPlaca = placa.width * placa.height;
        if (tamanhoPlaca > maiorTamanhoPlaca) {
            maiorPlaca = placa;
            maiorTamanhoPlaca = tamanhoPlaca;
        }
    }

    // Verificar se uma região foi encontrada
    if (maiorTamanhoPlaca > 0) {
        // Criar uma nova imagem contendo apenas a maior região selecionada
        cv::Mat imagemPlaca = imagem(maiorPlaca).clone();

        // Salvar a nova imagem
        cv::imwrite("placa.jpg", imagemPlaca);
    }

    // Salvar a imagem com o retângulo
    cv::imwrite("imagem1.jpg", imagem);

    // Carregar a imagem da placa
    cv::Mat imagemPlaca = cv::imread("placa.jpg");

    // Verificar se a imagem foi carregada corretamente
    if (imagemPlaca.empty()) {
        std::cout << "Falha ao carregar a imagem da placa." << std::endl;
        return -1;
    }

    // Converter a imagem para escala de cinza
    cv::Mat imagemCinzaPlaca;
    cv::cvtColor(imagemPlaca, imagemCinzaPlaca, cv::COLOR_BGR2GRAY);

    // Inicializar o objeto Tesseract OCR
    tesseract::TessBaseAPI tess;
    if (tess.Init(nullptr, "por2")) {
        std::cout << "Falha ao inicializar o Tesseract OCR." << std::endl;
        return -1;
    }

    // Configurar o modo de segmentação de caracteres
    tess.SetPageSegMode(tesseract::PageSegMode::PSM_SINGLE_BLOCK);

    // Definir a imagem do OCR como a imagem em escala de cinza da placa
    tess.SetImage(imagemCinzaPlaca.data, imagemCinzaPlaca.cols, imagemCinzaPlaca.rows, 1, imagemCinzaPlaca.cols);

    // Realizar o reconhecimento de caracteres
    tess.Recognize(nullptr);

    // Obter os resultados do OCR
    tesseract::ResultIterator* iteradorResultados = tess.GetIterator();
    tesseract::PageIteratorLevel nivel = tesseract::RIL_SYMBOL;

    std::string textoDetectado;

    if (iteradorResultados != nullptr) {
        do {
            const char* simbolo = iteradorResultados->GetUTF8Text(nivel);
            float confianca = iteradorResultados->Confidence(nivel);
            if (simbolo != nullptr && confianca > 80.00) {
                // Verificar se o símbolo é uma letra maiúscula ou um número
                if ((*simbolo >= 'A' && *simbolo <= 'Z') || (*simbolo >= '0' && *simbolo <= '9')) {
                    std::cout << "Letra: " << simbolo << " | Confiança: " << confianca << std::endl;
                    textoDetectado += simbolo;
                }
                delete[] simbolo;
            }
        } while (iteradorResultados->Next(nivel));
    }

    // Imprimir o texto completo detectado
    std::cout << "Texto completo: " << textoDetectado << std::endl;

    // Enviar mensagem

    std::ifstream arquivoJson("registro_placas.json");
    if (!arquivoJson.is_open()) {
        std::cout << "Falha ao abrir o arquivo JSON." << std::endl;
        return 1;
    }

    json dados;
    arquivoJson >> dados;
    arquivoJson.close();

    std::string placa = textoDetectado;
    bool placaEncontrada = false;

    for (const auto& obj : dados) {
        if (obj["placa"] == placa) {
            std::string chat_id = obj["id_telegram"];
            std::string texto_mensagem = "Olá! Esta é uma mensagem enviada pelo bot do Telegram.";
            enviarMensagem(chat_id, texto_mensagem);
            placaEncontrada = true;
            break;
        }
    }

    if (!placaEncontrada) {
        std::cout << "Placa não encontrada no arquivo JSON." << std::endl;
    }
    //Excluir imagem da placa recortada
    const char* nomeArquivo = "placa.jpg";

    // Verifica se o arquivo existe antes de excluir
    if (std::remove(nomeArquivo) == 0) {
        std::cout << "O arquivo '" << nomeArquivo << "' foi excluído com sucesso." << std::endl;
    } else {
        std::cout << "Falha ao excluir o arquivo '" << nomeArquivo << "'." << std::endl;
    }
    }
    return 0;
}
