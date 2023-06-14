#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void captureImage(const string& filePath) {
    VideoCapture camera(0); // 0 representa a webcam padrão

    Mat frame;
    if (camera.read(frame)) {
        imwrite(filePath, frame);
    }

    camera.release();
}

int main() {
    string imagePath = "imagem.jpg";
    captureImage(imagePath);

    return 0;
}
