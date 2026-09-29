#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>

using namespace cv;
using namespace std;

// green tint
Mat applyGreenReclaimed(Mat img) {
    Mat out = img.clone();
    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            Vec3b &p = out.at<Vec3b>(y, x);
            p[0] = saturate_cast<uchar>(p[0] * 0.85);
            p[1] = saturate_cast<uchar>(p[1] * 1.28);
            p[2] = saturate_cast<uchar>(p[2] * 0.90);
        }
    }
    out.convertTo(out, -1, 0.96, 6);
    return out;
}

//expired film look
Mat applyExpiredFilm(Mat img) {
    Mat out = img.clone();
    RNG rng(getTickCount());

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            Vec3b &p = out.at<Vec3b>(y, x);
            int decay = rng.uniform(-12, 12);
            p[1] = saturate_cast<uchar>(p[1] + decay);
            p[0] = saturate_cast<uchar>(p[0] - decay / 2);
            p[2] = saturate_cast<uchar>(p[2] * 0.9);
        }
    }

    Mat fog(out.size(), out.type(), Scalar(10,18,10));
    addWeighted(out, 0.92, fog, 0.08, 0, out);
    return out;
}

// chemical edge
Mat applyChemicalEdges(Mat img) {
    Mat out = img.clone();
    RNG rng(getTickCount());

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            int edge = min(min(x, img.cols-x), min(y, img.rows-y));
            if (edge < 45) {
                float s = (45-edge)/45.0f * rng.uniform(0.5f,1.1f);
                Vec3b &p = out.at<Vec3b>(y,x);
                p[1] = saturate_cast<uchar>(p[1] + 35*s);
                p[2] = saturate_cast<uchar>(p[2] + 15*s);
                p[0] = saturate_cast<uchar>(p[0] * (1.0-0.25*s));
            }
        }
    }
    return out;
}

//chemical blotches 
Mat applyChemicalBlotches(Mat img) {
    Mat out = img.clone();
    RNG rng(getTickCount());

    int count = rng.uniform(6,12);
    for (int i=0;i<count;i++) {
        Point c(rng.uniform(0,img.cols), rng.uniform(0,img.rows));
        int R = rng.uniform(40,120);
        for (int r=R;r>0;r--) {
            float a = pow((float)r/R,2.5f)*0.15f;
            Scalar col(rng.uniform(5,15), rng.uniform(40,75), rng.uniform(25,45));
            circle(out,c,r,col,-1,LINE_AA);
            addWeighted(out,a,img,1-a,0,out);
        }
    }
    return out;
}

//black lift
Mat liftBlacks(Mat img) {
    Mat fog(img.size(), img.type(), Scalar(18,28,18));
    addWeighted(img,0.90,fog,0.10,0,img);
    return img;
}

//film grain effect
Mat applyFilmGrain(Mat img) {
    Mat noise(img.size(), CV_8SC3);
    randn(noise,0,12);
    Mat out;
    img.convertTo(out,CV_16SC3);
    out += noise;
    out.convertTo(out,CV_8UC3);
    return out;
}

//vignette around 
Mat applyVignette(Mat img) {
    Mat out = img.clone();
    for (int y=0;y<img.rows;y++) {
        for (int x=0;x<img.cols;x++) {
            float dx=(x-img.cols/2.0f)/(img.cols/2.0f);
            float dy=(y-img.rows/2.0f)/(img.rows/2.0f);
            float d=sqrt(dx*dx+dy*dy);
            float v=max(1.0f-d*0.35f,0.6f);
            out.at<Vec3b>(y,x) *= v;
        }
    }
    return out;
}

//roller streaks
Mat applyRollerStreaks(Mat img) {
    Mat out = img.clone();
    RNG rng(getTickCount());
    for (int x=0;x<img.cols;x+=rng.uniform(30,80)) {
        float s=rng.uniform(0.92f,0.98f);
        for (int y=0;y<img.rows;y++)
            out.at<Vec3b>(y,x) *= s;
    }
    return out;
}

//polaroud frame
Mat addPolaroidFrame(Mat img) {
    int size = min(img.cols,img.rows);
    Mat square = img(Rect(
        (img.cols-size)/2,
        (img.rows-size)/2,
        size,size
    ));

    int side=size/12;
    int top=size/14;
    int bottom=size/3;

    Mat framed(size+top+bottom, size+side*2, CV_8UC3, Scalar(245,245,240));
    Rect photoRect(side, top, size, size);
    square.copyTo(framed(photoRect));

    RNG rng(getTickCount());

    //yellowed paper
    Mat decay(framed.size(), framed.type(),
              Scalar(rng.uniform(20,35),rng.uniform(40,65),rng.uniform(60,95)));
    addWeighted(framed,1.0,decay,0.12,0,framed);

    //cracks and straches
    int cracks = rng.uniform(10,18);
    for (int i=0;i<cracks;i++) {
        Point p1(rng.uniform(0,framed.cols), rng.uniform(0,framed.rows));
        Point p2(p1.x+rng.uniform(-150,150), p1.y+rng.uniform(-150,150));

        if (photoRect.contains(p1) || photoRect.contains(p2)) continue;
        line(framed,p1,p2,Scalar(230,230,230),1,LINE_AA);
    }

    // edge creep
    for (int y=0;y<framed.rows;y++) {
        for (int x=0;x<framed.cols;x++) {
            if (photoRect.contains(Point(x,y))) continue;

            int edge=min(min(x,framed.cols-x),min(y,framed.rows-y));
            if (edge<40) {
                float c=(40-edge)/40.0f*rng.uniform(0.3f,1.0f);
                Vec3b &p=framed.at<Vec3b>(y,x);
                p[1]=saturate_cast<uchar>(p[1]-20*c);
                p[2]=saturate_cast<uchar>(p[2]-14*c);
            }
        }
    }

    // bottom stained
    for (int y=photoRect.y+photoRect.height;y<framed.rows;y++) {
        float b=pow((float)y/framed.rows,4.0f);
        for (int x=0;x<framed.cols;x++) {
            Vec3b &p=framed.at<Vec3b>(y,x);
            p[1]=saturate_cast<uchar>(p[1]-rng.uniform(6,14)*b);
            p[2]=saturate_cast<uchar>(p[2]-rng.uniform(8,18)*b);
        }
    }

    return framed;
}

//filename
string filename() {
    time_t now=time(0);
    tm *ltm=localtime(&now);
    char buf[64];
    strftime(buf,sizeof(buf),"IMG_%Y%m%d_%H%M%S.polaroid.jpg",ltm);
    return string(buf);
}

//main
int main() {
    VideoCapture cap(1, CAP_AVFOUNDATION);
    if (!cap.isOpened()) return -1;

    Mat frame;
    namedWindow("Polaroid", WINDOW_AUTOSIZE);
    cout<<"SPACE = shoot | ESC = quit\n";

    while (true) {
        cap>>frame;
        if (frame.empty()) break;

        flip(frame,frame,1);
        imshow("Polaroid",frame);

        char key=waitKey(30);
        if (key==27) break;

        if (key==32) {
            srand(time(0));

            Mat flash(frame.size(),frame.type(),Scalar(255,255,255));
            imshow("Polaroid",flash);
            waitKey(120);

            Mat shot=frame.clone();
            shot=applyGreenReclaimed(shot);
            shot=applyExpiredFilm(shot);
            shot=applyChemicalBlotches(shot);
            shot=applyChemicalEdges(shot);
            shot=liftBlacks(shot);
            shot=applyFilmGrain(shot);
            shot=applyRollerStreaks(shot);
            shot=applyVignette(shot);
            shot=addPolaroidFrame(shot);
            GaussianBlur(shot,shot,Size(3,3),0);

            Mat black=Mat::zeros(shot.size(),shot.type());
            for (int i=0;i<=20;i++) {
                double a=i/20.0;
                Mat blend;
                addWeighted(black,1-a,shot,a,0,blend);
                imshow("Polaroid",blend);
                waitKey(60);
            }

            imwrite(filename(),shot);
            cout<<"Saved\n";
        }
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
