#include "effects.hpp"
#include <random>

void polaroidColor(cv::Mat& img) {
    img.convertTo(img, -1, 1.1, 10); // contrast + brightness
    std::vector<cv::Mat> channels;
    cv::split(img, channels);

    channels[0] *= 0.95; // blue down
    channels[1] *= 1.05; // green up
    channels[2] *= 1.15; // red up

    cv::merge(channels, img);
}

void addGrain(cv::Mat& img) {
    cv::Mat noise(img.size(), img.type());
    cv::randn(noise, 0, 12);
    img += noise;
}
void imperfections(cv::Mat& img) {
    if (rand() % 2 == 0)
        cv::GaussianBlur(img, img, {3,3}, 0);

    if (rand() % 2 == 0) {
        cv::circle(
            img,
            {rand() % img.cols, rand() % img.rows},
            rand() % 3 + 1,
            {255,255,255},
            -1
        );
    }
}
cv::Mat addFrame(const cv::Mat& img) {
    int bottom = img.rows * 0.25;
    cv::Mat framed;
    cv::copyMakeBorder(
        img, framed,
        20, bottom, 20, 20,
        cv::BORDER_CONSTANT,
        cv::Scalar(255,255,255)
    );

    cv::putText(
        framed,
        "POLAROID",
        {30, framed.rows - 30},
        cv::FONT_HERSHEY_SIMPLEX,
        0.7,
        cv::Scalar(50,50,50),
        2
    );

    return framed;
}
