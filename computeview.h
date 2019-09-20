#ifndef COMPUTEVIEW_H
#define COMPUTEVIEW_H

#include "loaddata.h"
#include <opencv2/opencv.hpp>

class computeView
{
public:
    computeView();

    loadData* ld;

    void set_ld(loadData* ld);
    double DepthLevelToZ(unsigned char d);
    unsigned char ZToDepthLevel(double z);
    void projUVZtoXY(double projMatrix[4][4], double u, double v, double z, double *x, double *y);
    double projXYZtoUV(double projMatrix[4][4], double x, double y, double z, double *u, double *v);
    void pointProject_from_ref_to_otherView(int ref_id, int ref, int u, int v, unsigned char d);
    void warpingImage(int ref, int ref_id, int proj, cv::Mat &imageColorOut, cv::Mat &imageDepthOut);
    cv::Mat viewSynthesis(int cur_view);
};

#endif // COMPUTEVIEW_H
