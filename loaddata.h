#ifndef LOADDATA_H
#define LOADDATA_H

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
using namespace std;

typedef struct
{
    double m_K[3][3];
    double m_Rotmatrix[3][3];
    double m_Trans[3];

    double m_ProjMatrix[4][4];
} CalibStruct;


class loadData
{
public:
    loadData();

    int camera_num;                     //camera number
    int view_num;                       //view number
    int camera_left;                    //left camera to synthesis view
    int camera_right;                   //right camera to synthesis view

    int width;                          //frame width
    int height;                         //frame height

    double Z_near_L;
    double Z_far_L;

    double Z_near_R;
    double Z_far_R;

    double MinZ;
    double MaxZ;
    double *pts;                       //

    FILE *view_setting;                 //camera parameters txt files

    CalibStruct* m_CalibParams;         //in, ex param
    CalibStruct* allView_CalibParams;
    CalibStruct* curView_CalibParams;   //current view in,ex params

    string basePath;                    //image sequence path

    int* cameraID;
    int* viewID;

    string* path_arr;                   //image arr path
    cv::Mat* frame_imgArr;              //reference color image
    cv::Mat* frame_depArr;              //reference depth image



    void get_frame_imgArr(int frame_num, int left_cam, int right_cam);
    void get_frame_depArr(int frame_num, int left_cam, int right_cam);
    void get_view_setting(char *path);
    void get_ref_camera(int cur_view, int cam_ref[]);
    void get_ref_cameraID_in_all_views(int cur_view, int cam_ref[]);
    void get_allView_calibParams();
};
#endif // LOADDATA_H
