#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <iostream>

int main() {
    // Carregar a imagem
    cv::Mat image = cv::imread("imagem.jpg");

    // Verificar se a imagem foi carregada corretamente
    if (image.empty()) {
        std::cout << "Falha ao carregar a imagem." << std::endl;
        return -1;
    }

    // Carregar o arquivo XML contendo os dados do classificador de placa de carro
    cv::CascadeClassifier plateCascade;
    if (!plateCascade.load("a.xml")) {
        std::cout << "Falha ao carregar o classificador." << std::endl;
        return -1;
    }

    // Converter a imagem para escala de cinza
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    // Realizar a detecção da placa de carro
    std::vector<cv::Rect> plates;
    plateCascade.detectMultiScale(grayImage, plates, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

    // Desenhar retângulo no contorno da placa
    for (const auto& plate : plates) {
        cv::rectangle(image, plate, cv::Scalar(0, 255, 0), 2);

        // Criar uma nova imagem contendo apenas a região selecionada
        cv::Mat plateImage = image(plate).clone();

        // Salvar a nova imagem
        cv::imwrite("placa.jpg", plateImage);
    }

    // Salvar a imagem com o retângulo
    cv::imwrite("imagem1.jpg", image);


// Carregar a imagem da placa
    cv::Mat plateImage = cv::imread("placa.jpg");

    // Verificar se a imagem foi carregada corretamente
    if (plateImage.empty()) {
        std::cout << "Falha ao carregar a imagem da placa." << std::endl;
        return -1;
    }

    // Converter a imagem para escala de cinza
    cv::Mat grayImage2;
    cv::cvtColor(plateImage, grayImage2, cv::COLOR_BGR2GRAY);

    // Inicializar o objeto Tesseract OCR
    tesseract::TessBaseAPI tess;
    if (tess.Init(nullptr, "eng")) {
        std::cout << "Falha ao inicializar o Tesseract OCR." << std::endl;
        return -1;
    }

    // Configurar o modo de segmentação de caracteres
    tess.SetPageSegMode(tesseract::PageSegMode::PSM_SINGLE_BLOCK);

    // Definir a imagem do OCR como a imagem em escala de cinza da placa
    tess.SetImage(grayImage2.data, grayImage2.cols, grayImage2.rows, 1, grayImage2.cols);

    // Realizar o reconhecimento de caracteres
    tess.Recognize(nullptr);

    // Obter os resultados do OCR
    tesseract::ResultIterator* resultIterator = tess.GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL;

    std::string detectedText;

   if (resultIterator != nullptr ) {
        do {
            const char* symbol = resultIterator->GetUTF8Text(level);
            float confidence = resultIterator->Confidence(level);
            if (symbol != nullptr /*&& confidence > 90.00*/ ) {
                // Verificar se o símbolo é uma letra maiúscula ou um número
                if ((*symbol >= 'A' && *symbol <= 'Z') || (*symbol >= '0' && *symbol <= '9')) {
                    std::cout << "Letra: " << symbol << " | Confiança: " << confidence << std::endl;
                    detectedText += symbol;
                }
                delete[] symbol;
            }
        } while (resultIterator->Next(level));
    }

    // Imprimir o texto completo detectado
    std::cout << "Texto completo: " << detectedText << std::endl;


    return 0;
}
