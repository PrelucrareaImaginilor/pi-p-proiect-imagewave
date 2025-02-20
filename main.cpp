﻿#include <opencv2/opencv.hpp>
#include <iostream>
#include <numeric>
#include <cmath>

class KalmanFilter2D {
public:
    cv::KalmanFilter kf;
    cv::Mat state;
    cv::Mat meas;

    KalmanFilter2D() 
    {
        kf = cv::KalmanFilter(4, 2, 0); // filtru Kalman cu 4 stari(pozitie si viteza) si 2 masuratori (pozitie x,pozitie y)
        state = cv::Mat::zeros(4, 1, CV_32F);
        meas = cv::Mat::zeros(2, 1, CV_32F);

        // matrice tranzitie
        kf.transitionMatrix = (cv::Mat_<float>(4, 4) << 1, 0, 1, 0,
            0, 1, 0, 1,
            0, 0, 1, 0,
            0, 0, 0, 1);

        // matrici de identificare pentru masuratori si covarianta zgomotului
        setIdentity(kf.measurementMatrix);
        setIdentity(kf.processNoiseCov, cv::Scalar::all(1e-3)); // zgomot mic pentru proces
        setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-2)); // zgomot masuratori
        setIdentity(kf.errorCovPost, cv::Scalar::all(1)); // covarianta initiala a erorii

        randn(kf.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1)); // init stare cu zgomot
    }

    //  predictia pozitiei
    cv::Point2f predict() 
    {
        cv::Mat pred = kf.predict();
        return cv::Point2f(pred.at<float>(0), pred.at<float>(1));
    }

    // actualizarea masuratorilor
    void update(float x, float y)
    {
        meas.at<float>(0) = x;
        meas.at<float>(1) = y;
        kf.correct(meas);
    }
};

int main() {

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) 
    {
        std::cerr << "Eroare la deschiderea camerei!" << std::endl;
        return -1;
    }

    KalmanFilter2D kalman;
    cv::Mat frame, grayFrame, prevGrayFrame, diffFrame, binaryMask;
    cv::Mat background;

    cap >> frame;
    if (frame.empty())
    {
        std::cerr << "Eroare: Flux video gol!" << std::endl;
        return -1;
    }

    cv::cvtColor(frame, background, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(background, background, cv::Size(5, 5), 0);

    cv::Mat flow, flowParts[2];
    cv::Mat magnitude, angle;

    static int frameCounter = 0;

    while (true)
    {
        cap >> frame; 
        if (frame.empty()) break; 

        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(grayFrame, grayFrame, cv::Size(5, 5), 0);

        frameCounter++;
        if (frameCounter % 30 == 0)
        {
            background = grayFrame.clone(); // actualizare background o data la 30sec pt a tine cont de miscarile lente
        }

        cv::absdiff(background, grayFrame, diffFrame); // diferenta dintre background si cadrul curent

        cv::threshold(diffFrame, binaryMask, 30, 255, cv::THRESH_BINARY); // masca binara

        cv::erode(binaryMask, binaryMask, cv::Mat()); // eliminare zgomot
        cv::dilate(binaryMask, binaryMask, cv::Mat());

        if (binaryMask.type() != CV_8U) // verific daca masca binara are formatul corect pt optical flow
        {
            binaryMask.convertTo(binaryMask, CV_8U);
        }

        // detectie contur regiuni de interes
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(binaryMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        cv::Rect unifiedRoi; // unim toate contururile intr-un dreptunghi unificat
        for (const auto& contour : contours)
        {
            if (cv::contourArea(contour) > 1000) 
            { // ignoram regiuni foarte mici
                cv::Rect roi = cv::boundingRect(contour);
                unifiedRoi = unifiedRoi | roi; // unim dreptunghiurile
            }
        }

        if (unifiedRoi.area() > 0)
        {
            cv::rectangle(frame, unifiedRoi, cv::Scalar(0, 255, 0), 2); // afisare dreptunghi unificat
        }


        // calcul optical flow daca exista cadru anterior
        if (!prevGrayFrame.empty())
        {
            cv::calcOpticalFlowFarneback(prevGrayFrame, grayFrame, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
            cv::split(flow, flowParts);

            // calcul componente medii optical flow
            cv::Scalar avgFlowX = cv::mean(flowParts[0], binaryMask);
            cv::Scalar avgFlowY = cv::mean(flowParts[1], binaryMask);

            double avgX = avgFlowX[0];
            double avgY = avgFlowY[0];

           // filtrare metoda Kalman
            kalman.update(avgX, avgY);
            cv::Point2f filteredPoint = kalman.predict();

            // det directia miscarii
            std::string direction;
            double angle = std::atan2(filteredPoint.y, filteredPoint.x) * 180.0 / CV_PI;
            if (angle < 0) angle += 360.0;

            if (std::sqrt(filteredPoint.x * filteredPoint.x + filteredPoint.y * filteredPoint.y) > 1.0) 
            {
                if (angle >= 45 && angle < 135)
                {
                    direction = "Jos";
                }
                else if (angle >= 135 && angle < 225)
                {
                    direction = "Stanga";
                }
                else if (angle >= 225 && angle < 315) 
                {
                    direction = "Sus";
                }
                else
                {
                    direction = "Dreapta";
                }
            }
            else
            {
                direction = "Stationar";
            }

            std::cout << "Directia: " << direction << " (X: " << filteredPoint.x << ", Y: " << filteredPoint.y << ")" << std::endl;
            cv::putText(frame, "Directia: " + direction, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
        }

        grayFrame.copyTo(prevGrayFrame);

        cv::imshow("Video", frame);
        cv::imshow("Binary Mask", binaryMask);

        if (cv::waitKey(1) >= 0) break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
