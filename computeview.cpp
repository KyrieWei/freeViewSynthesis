#include "computeview.h"

computeView::computeView()
{

}

void computeView::set_ld(loadData *ld)
{
    this->ld = ld;
}

void computeView::computeProjectionMatrices()
{

}

double computeView::DepthLevelToZ(unsigned char d)
{
    double z;
    z = 1.0 / ((d / 255.0)*(1.0 / ld->MinZ - 1.0 / ld->MaxZ) + 1.0 / ld->MaxZ);
    return z;
}

unsigned char computeView::ZToDepthLevel(double z)
{
    unsigned char d;
    d = (unsigned char)(255.0*(1 / (double)z - 1 / ld->MaxZ) / (1 / ld->MinZ - 1 / ld->MaxZ));
    return d;
}

void computeView::projUVZtoXY(double projMatrix[4][4], double u, double v, double z, double *x, double *y)
{
    double c0, c1, c2;

    v = (double)ld->height - v - 1.0;

    c0 = z * projMatrix[0][2] + projMatrix[0][3];
    c1 = z * projMatrix[1][2] + projMatrix[1][3];
    c2 = z * projMatrix[2][2] + projMatrix[2][3];

    *y = u * (c1*projMatrix[2][0] - projMatrix[1][0] * c2) +
        v * (c2*projMatrix[0][0] - projMatrix[2][0] * c0) +
        projMatrix[1][0] * c0 - c1 * projMatrix[0][0];

    *y /= v * (projMatrix[2][0] * projMatrix[0][1] - projMatrix[2][1] * projMatrix[0][0]) +
        u * (projMatrix[1][0] * projMatrix[2][1] - projMatrix[1][1] * projMatrix[2][0]) +
        projMatrix[0][0] * projMatrix[1][1] - projMatrix[1][0] * projMatrix[0][1];

    *x = (*y)*(projMatrix[0][1] - projMatrix[2][1] * u) + c0 - c2 * u;
    *x /= projMatrix[2][0] * u - projMatrix[0][0];
}

double computeView::projXYZtoUV(double projMatrix[4][4], double x, double y, double z, double *u, double *v)
{
    double w;

    *u = projMatrix[0][0] * x +
        projMatrix[0][1] * y +
        projMatrix[0][2] * z +
        projMatrix[0][3];

    *v = projMatrix[1][0] * x +
        projMatrix[1][1] * y +
        projMatrix[1][2] * z +
        projMatrix[1][3];

    w = projMatrix[2][0] * x +
        projMatrix[2][1] * y +
        projMatrix[2][2] * z +
        projMatrix[2][3];

    *u /= w;
    *v /= w;

    // image (0,0) is bottom lefthand corner
    *v = (double)ld->height - *v - 1.0;

    return w;
}

void computeView::pointProject_from_ref_to_otherView(int ref_id, int u, int v, unsigned char d)
{
    double x, y, z = DepthLevelToZ(d);

    projUVZtoXY(ld->allView_CalibParams[ref_id].m_ProjMatrix, (double)u, (double)v, z, &x, &y);

    double *pt = ld->pts;
    pt[2] = projXYZtoUV(ld->allView_CalibParams[ref_id].m_ProjMatrix, x, y, z, &pt[0], &pt[1]);
    pt[2] = ZToDepthLevel(pt[2]);

}

void computeView::warpingImage(int ref, int ref_id, int proj, cv::Mat &imageColorOut, cv::Mat &imageDepthOut)
{
    for (int v = 0; v < ld->frame_imgArr[ref].rows; v++)
        for (int u = 0; u < ld->frame_imgArr[ref].cols; u++)
        {
            double d = ld->frame_depArr[ref].at<cv::Vec3b>(v, u)[0];

            pointProject_from_ref_to_otherView(ref_id, u, v, d);
            int u1 = (int)ld->pts[0];
            int v1 = (int)ld->pts[1];
            int k1 = (int)ld->pts[2];
            if (u1 < 0 || u1 >= ld->frame_imgArr[ref].cols - 1 || v1 < 0 || v1 >= ld->frame_imgArr[ref].rows - 1)
                continue;
            if (k1 < imageDepthOut.at<cv::Vec3b>(v1, u1)[0])
                continue;
            imageColorOut.at<cv::Vec3b>(v1, u1) = ld->frame_imgArr[ref].at<cv::Vec3b>(v, u);
            imageDepthOut.at<cv::Vec3b>(v1, u1)[0] = k1;
            imageDepthOut.at<cv::Vec3b>(v1, u1)[1] = k1;
            imageDepthOut.at<cv::Vec3b>(v1, u1)[2] = k1;
        }
}

cv::Mat computeView::viewSynthesis(int cur_view)
{

    cv::Mat imageColorOut;
    cv::Mat imageColorOut2;
    cv::Mat imageDepthOut;

    imageColorOut.create(ld->frame_imgArr[0].rows, ld->frame_imgArr[0].cols, ld->frame_imgArr[0].type());
    imageColorOut2.create(ld->frame_imgArr[0].rows, ld->frame_imgArr[0].cols, ld->frame_imgArr[0].type());
    imageDepthOut.create(ld->frame_imgArr[0].rows, ld->frame_imgArr[0].cols, ld->frame_imgArr[0].type());

    for(int v = 0; v < ld->frame_imgArr[0].rows; v ++)
    {
        for(int u = 0; u < ld->frame_imgArr[0].cols; u ++)
        {
            imageColorOut.at<cv::Vec3b>(v, u)[0] = 0;
            imageColorOut.at<cv::Vec3b>(v, u)[1] = 0;
            imageColorOut.at<cv::Vec3b>(v, u)[2] = 0;
            imageColorOut2.at<cv::Vec3b>(v, u)[0] = 0;
            imageColorOut2.at<cv::Vec3b>(v, u)[1] = 0;
            imageColorOut2.at<cv::Vec3b>(v, u)[2] = 0;
            imageDepthOut.at<cv::Vec3b>(v, u)[0] = 0;
            imageDepthOut.at<cv::Vec3b>(v, u)[1] = 0;
            imageDepthOut.at<cv::Vec3b>(v, u)[2] = 0;
        }
    }
    int cam_ref[2];
    ld->get_ref_cameraID_in_all_views(cur_view, cam_ref);
    cout << cam_ref[0] << " " << cam_ref[1] << endl;
    warpingImage(0, cam_ref[0], cur_view, imageColorOut, imageDepthOut);
    warpingImage(1, cam_ref[1], cur_view, imageColorOut, imageDepthOut);

    cv::medianBlur(imageColorOut, imageColorOut, 3);
    cv::medianBlur(imageDepthOut, imageDepthOut, 3);

    return imageColorOut;
}
