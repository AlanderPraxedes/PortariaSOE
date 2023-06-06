#include <opencv2/opencv.hpp>

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
    }

    // Salvar a imagem com o retângulo
    cv::imwrite("imagem1.jpg", image);

    return 0;
}
