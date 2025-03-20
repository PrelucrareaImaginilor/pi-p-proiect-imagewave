#include <opencv2/opencv.hpp>
#include <iostream>
#include <numeric>
#include <cmath>

using namespace std;
using namespace cv;

class KalmanFilter2D {
public:
    cv::KalmanFilter kf;
    cv::Mat state;
    cv::Mat meas;

    KalmanFilter2D() {
        // Initializare Kalman Filter cu 4 stari (pozitie si viteza) si 2 masuratori (pozitie x si y)
        kf = cv::KalmanFilter(4, 2, 0);
        state = cv::Mat::zeros(4, 1, CV_32F);
        meas = cv::Mat::zeros(2, 1, CV_32F);

        // Matricea de tranzitie (model de miscare liniara simplu)
        kf.transitionMatrix = (cv::Mat_<float>(4, 4) <<
            1, 0, 1, 0,
            0, 1, 0, 1,
            0, 0, 1, 0,
            0, 0, 0, 1);

        // Setam matricile de identificare pentru masuratori si covarianta zgomotului
        setIdentity(kf.measurementMatrix);
        setIdentity(kf.processNoiseCov, cv::Scalar::all(1e-3)); // Zgomot mic pentru proces
        setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-2)); // Zgomot masuratori
        setIdentity(kf.errorCovPost, cv::Scalar::all(1)); // Covarianta initiala a erorii

        // Initializare stare cu zgomot
        randn(kf.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));
    }

    // Functie pentru predictia pozitiei
    cv::Point2f predict() {
        cv::Mat pred = kf.predict();
        return cv::Point2f(pred.at<float>(0), pred.at<float>(1));
    }

    // Functie pentru actualizarea masuratorilor
    void update(float x, float y) {
        meas.at<float>(0) = x;
        meas.at<float>(1) = y;
        kf.correct(meas);
    }
};

int main() {
    // Deschidem camera web pentru capturare video
    cv::VideoCapture cap(0); // 0 indică camera implicită
    if (!cap.isOpened()) {
        std::cerr << "Eroare la deschiderea camerei!" << std::endl;
        return -1;
    }

    KalmanFilter2D kalman;
    cv::Mat frame, grayFrame, prevGrayFrame, diffFrame, binaryMask;
    cv::Mat background;

    // Citim primul cadru pentru initializarea background-ului
    cap >> frame;
    if (frame.empty()) {
        std::cerr << "Eroare: Flux video gol!" << std::endl;
        return -1;
    }

    cv::cvtColor(frame, background, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(background, background, cv::Size(5, 5), 0);

    cv::Mat flow, flowParts[2];
    cv::Mat magnitude, angle;

    int frameCounter = 0;

    while (true) {
        cap >> frame; // Citim urmatorul cadru
        if (frame.empty()) break; // Terminam daca nu mai sunt cadre

        // Transformam cadrul curent in grayscale
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(grayFrame, grayFrame, cv::Size(5, 5), 0);

        // Actualizam background-ul la fiecare 20 de cadre pentru a tine cont de schimbarile lente
        frameCounter++;
        if (frameCounter % 20 == 0) {
            background = grayFrame.clone();
        }

        // Calculam diferenta dintre background si cadrul curent
        cv::absdiff(background, grayFrame, diffFrame);

        // Aplicam pragul pentru a obtine o masca binara
        cv::threshold(diffFrame, binaryMask, 30, 255, cv::THRESH_BINARY);

        // Eliminam zgomotul folosind operatii morfologice
        cv::erode(binaryMask, binaryMask, cv::Mat());
        cv::dilate(binaryMask, binaryMask, cv::Mat());

        // Ne asiguram ca binaryMask este de tip CV_8U pentru calcOpticalFlow
        if (binaryMask.type() != CV_8U) {
            binaryMask.convertTo(binaryMask, CV_8U);
        }

        // Detectam contururile regiunilor de interes
        vector<vector<Point>> contours;
        findContours(binaryMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // Combinam toate contururile intr-un dreptunghi unificat
        Rect unifiedRoi;
        for (const auto& contour : contours) {
            if (contourArea(contour) > 1000) { // Ignoram regiuni foarte mici
                Rect roi = boundingRect(contour);
                unifiedRoi = unifiedRoi | roi; // Unificam dreptunghiurile
            }
        }

        if (unifiedRoi.area() > 0) {
            cv::rectangle(frame, unifiedRoi, cv::Scalar(0, 255, 0), 2); // Afisam dreptunghiul unificat
        }

        // Calculam optical flow daca avem un cadru anterior
        if (!prevGrayFrame.empty()) {
            cv::calcOpticalFlowFarneback(prevGrayFrame, grayFrame, flow, 0.5, 5, 20, 3, 7, 1.5, 0);
            cv::split(flow, flowParts);

            // Calculam componentele medii ale fluxului optic
            cv::Scalar avgFlowX = cv::mean(flowParts[0], binaryMask);
            cv::Scalar avgFlowY = cv::mean(flowParts[1], binaryMask);

            double avgX = avgFlowX[0];
            double avgY = avgFlowY[0];

            // Filtrare folosind Kalman
            kalman.update(avgX, avgY);
            cv::Point2f filteredPoint = kalman.predict();

            // Determinam directia miscarii
            string direction;
            double angle = atan2(filteredPoint.y, filteredPoint.x) * 180.0 / CV_PI;
            if (angle < 0) angle += 360.0;

            if (sqrt(filteredPoint.x * filteredPoint.x + filteredPoint.y * filteredPoint.y) > 1.5) {
                if (angle >= 45 && angle < 135) {
                    direction = "Jos";
                }
                else if (angle >= 135 && angle < 225) {
                    direction = "Stanga";
                }
                else if (angle >= 225 && angle < 315) {
                    direction = "Sus";
                }
                else {
                    direction = "Dreapta";
                }
            }
            else {
                direction = "Stationar";
            }

            // Afisam directia pe consola si pe ecran
            cout << "Directia: " << direction << " (X: " << filteredPoint.x << ", Y: " << filteredPoint.y << ")" << endl;
            putText(frame, "Directia: " + direction, cv::Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
        }

        // Salvam cadrul curent ca prevGrayFrame
        grayFrame.copyTo(prevGrayFrame);

        // Afisam rezultatele
        cv::imshow("Video", frame);
        cv::imshow("Binary Mask", binaryMask);

        // Asteptam 1ms intre cadre pentru procesare in timp real
        if (cv::waitKey(1) >= 0) break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
