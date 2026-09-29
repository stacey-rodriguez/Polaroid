#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>

std::string timestamp() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "IMG_%Y%m%d_%H%M%S.polaroid.jpg", ltm);
    return std::string(buf);
}

int main() {
    cv::VideoCapture cam(1);
    if (!cam.isOpened()) {
        std::cerr << "Camera failed\n";
        return -1;
    }

    cv::Mat frame;
    std::cout << "Press SPACE to shoot. ESC to quit.\n";

    while (true) {
        cam >> frame;
        if (frame.empty()) break;

        cv::imshow("Viewfinder", frame);
        int key = cv::waitKey(1);

        if (key == 27) break;               // ESC
        if (key == 32) {                   // SPACE
            std::string name = timestamp();
            cv::imwrite(name, frame);
            std::cout << "Saved: " << name << "\n";
            break; // no retakes 😈
        }
    }

    cam.release();
    cv::destroyAllWindows();
    return 0;
}
cv::Mat developed = frame.clone();

polaroidColor(developed);
addGrain(developed);
imperfections(developed);
developed = addFrame(developed);

cv::Mat fade = cv::Mat::zeros(developed.size(), developed.type());

for (int i = 0; i <= 100; i++) {
    double alpha = i / 100.0;
    cv::addWeighted(developed, alpha, fade, 1 - alpha, 0, fade);
    cv::imshow("Developing...", fade);
    cv::waitKey(30);
}
