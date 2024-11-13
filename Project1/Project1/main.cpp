#include "function.h"

using namespace module1;


int main() 
{
    string videoPath = "Video/video.mp4";

    vector<Mat> frames;
    PrintFrames(videoPath, 12);

    return 0;
}
