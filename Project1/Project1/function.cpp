#include "function.h"


void module1::showCanvasRegion(const Mat& canvas, int x, int y) {
    // Asigurăm că x și y sunt în limitele canvasului
    x = max(0, min(x, canvas.cols - WINDOW_WIDTH));
    y = max(0, min(y, canvas.rows - WINDOW_HEIGHT));

    // Extragem regiunea corespunzătoare pentru a o afișa
    Rect roi(x, y, WINDOW_WIDTH, WINDOW_HEIGHT);
    Mat view = canvas(roi);

    // Afișăm regiunea selectată
    imshow("Scrollable Canvas", view);
}

Mat module1::filterFrame(Mat frame)
{
    Mat grayFrame;
    cvtColor(frame, grayFrame, COLOR_BGR2GRAY);

    Mat blurredFrame;
    GaussianBlur(grayFrame, blurredFrame, Size(5, 5), 1.5); // Kernel 5x5, sigma=1.5

    // 2. Ajustarea intensității prin CLAHE
    Mat equalizedFrame;

    Ptr<CLAHE> clahe = createCLAHE(6.0, Size(8, 8)); // Clip limit=2.0, grid size=8x8
    clahe->apply(blurredFrame, equalizedFrame);

    cvtColor(equalizedFrame, equalizedFrame, COLOR_GRAY2BGR); // Conversie în BGR

    return equalizedFrame;
}

void module1::PrintFrames(string videoPath, int numFramesToDisplay)
{
    Size s(WIDTH, HEIGHT);
    int rows = HEIGHT * numFramesToDisplay;
    int coloms = WIDTH * 2;

    Mat canvas(rows, coloms, CV_8UC3, Scalar(0, 0, 0));

    VideoCapture capture(videoPath);
    if (!capture.isOpened())
    {
        cout << "NU S-A DESCHIS\n";
        exit(0);
    }

    int totalFrames = (int)capture.get(CAP_PROP_FRAME_COUNT);
    int interval = totalFrames / numFramesToDisplay;

    for (int i = 0; i < numFramesToDisplay; i++)
    {
        Mat frame;

        int frameNumber = i * interval;
        capture.set(CAP_PROP_POS_FRAMES, frameNumber);

        capture >> frame;
        if (frame.empty())
        {
            cerr << "Cadrul nu a putut fi citit la poziția: " << frameNumber << endl;
            break;
        }
        
        addFrame(canvas, frame, i);
    }

    PrintWindow(canvas, numFramesToDisplay);
}

void module1::PrintWindow(Mat canvas, int numFramesToDisplay)
{
    int scrollX = 0;
    int scrollY = 0;

    namedWindow("Scrollable Canvas", WINDOW_NORMAL);
    resizeWindow("Scrollable Canvas", WINDOW_WIDTH, WINDOW_HEIGHT);

    // Afișăm partea inițială a canvasului
    showCanvasRegion(canvas, scrollX, scrollY);

    while (true) {
        int key = waitKey(0);

        // Actualizăm poziția de scroll pe baza tastei apăsate
        if (key == 27) { // ESC pentru a ieși
            break;
        }
        else if (key == 'w' || key == 82) { // Săgeată sus sau 'w'
            scrollY -= 50;

            if (scrollY < 0)
                scrollY = 0;
        }
        else if (key == 's' || key == 84) { // Săgeată jos sau 's'
            scrollY += 50;

            if (scrollY > HEIGHT * (numFramesToDisplay - 2))
                scrollY = HEIGHT * (numFramesToDisplay - 2);

            cout << scrollY << '\n';
        }

        // Afișăm partea actualizată a canvasului
        showCanvasRegion(canvas, scrollX, scrollY);
    }
}

void module1::addFrame(Mat canvas, Mat frame, int i)
{
    Mat fframe = filterFrame(frame);
    frame.copyTo(canvas(Rect(0, i * HEIGHT, frame.cols, frame.rows)));
    fframe.copyTo(canvas(Rect(WIDTH, i * HEIGHT, fframe.cols, fframe.rows)));
}


