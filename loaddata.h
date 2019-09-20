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

    int camera_num;                     //camera number 8
    int vir_num;                        //virtual view between two cameras 3
    int view_num;                       //view number   (8-1)*4+1

    int frame_num;                      //current frame
    int cur_view;                       //current view
    int cur_camera;                     //current camera

    int width;                          //frame width
    int height;                         //frame height

    double MinZ;
    double MaxZ;

    int refIdx_in_cam_arr[2];           //ref index in cameras array (0-7)
    int refIdx_in_view_arr[2];          //ref index in views array (0-28)

    double *pts;                        //

    FILE *view_setting;                 //camera parameters txt files
    char* calibParams_path;            //path to camera parameters file

    CalibStruct* m_CalibParams;         //in, ex param
    CalibStruct* allView_CalibParams;

    string basePath;                    //image sequence path

    string* path_arr;                   //image arr path
    cv::Mat* frame_imgArr;              //reference color image
    cv::Mat* frame_depArr;              //reference depth image

    void get_frame_imgArr(int frame_num, int left_cam, int right_cam);          //get left and right color images
    void get_frame_depArr(int frame_num, int left_cam, int right_cam);          //get left and right depth images
    void get_view_setting(char *path);                                          //read camera calibParams
    void get_ref_camera(int cur_view);                                          //get camera indexs in camera array
    void get_ref_cameraID_in_all_views(int cur_view);                           //get camera indexs in view array
    void get_allView_calibParams();                                             //compute view (camera + virtual) point calibParams
};
#endif // LOADDATA_H
