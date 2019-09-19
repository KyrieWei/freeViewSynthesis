#include "loaddata.h"

loadData::loadData()
{
    camera_num = 8;
    view_num = (camera_num-1)*4 + 1;

    basePath = "F:\\wei\\freeViewProject\\test_pic_video\\ballet-dancer\\MSR3DVideo-Ballet\\";

    cameraID = new int[camera_num];
    viewID = new int[view_num];

    path_arr = new string[camera_num];
    frame_imgArr = new cv::Mat[2];
    frame_depArr = new cv::Mat[2];
    m_CalibParams = new CalibStruct[camera_num];
    allView_CalibParams = new CalibStruct[view_num + camera_num];
    curView_CalibParams = new CalibStruct();
    pts = new double[3];

    height = 768;
    width = 1024;
    MaxZ = 130.0;
    MinZ = 42.0;

    for(int i = 0; i < camera_num; i ++)
    {
        cameraID[i] = i;
        path_arr[i] = basePath + "cam" + char('0'+i) + "\\";
    }

    for(int i = 0; i < view_num; i ++)
        viewID[i] = i;

}

void loadData::get_frame_imgArr(int frame_num, int left_cam, int right_cam)
{
    char tmp[20];
    left_cam --;
    right_cam --;
    sprintf(tmp, "color-cam%d-f%03d.jpg", left_cam, frame_num);
    string frame_name = path_arr[left_cam] + tmp;
    cout << frame_name << endl;
    frame_imgArr[0] = cv::imread(frame_name);
    memset(tmp, '\0', sizeof(tmp));

    sprintf(tmp, "color-cam%d-f%03d.jpg", right_cam, frame_num);
    frame_name = path_arr[right_cam] + tmp;
    //cout << frame_name << endl;
    frame_imgArr[1] = cv::imread(frame_name);
    memset(tmp, '\0', sizeof(tmp));

}

void loadData::get_frame_depArr(int frame_num, int left_cam, int right_cam)
{
    char tmp[20];
    left_cam --;
    right_cam --;
    sprintf(tmp, "depth-cam%d-f%03d.png", left_cam, frame_num);
    string frame_name= path_arr[left_cam] + tmp;
    cout << frame_name << endl;
    frame_depArr[0] = cv::imread(frame_name);
    memset(tmp, '\0', sizeof(tmp));

    sprintf(tmp, "depth-cam%d-f%03d.png", right_cam, frame_num);
    frame_name = path_arr[right_cam] + tmp;
    frame_depArr[1] = cv::imread(frame_name);
    memset(tmp, '\0', sizeof(tmp));
}

void loadData::get_view_setting(char* path)
{
    int i, j, k;
    FILE *pIn;
    int camIdx;
    double dIn;
    float s1, s2, s3;
    if(pIn = fopen(path, "r"))
    {

        for(k = 0; k < camera_num; k ++)
        {
            fscanf(pIn, "%d", &camIdx);

            for(i = 0; i < 3; i ++)
            {
                fscanf(pIn, "%lf\t%lf\t%lf", &(m_CalibParams[camIdx].m_K[i][0]), &(m_CalibParams[camIdx].m_K[i][1]), &(m_CalibParams[camIdx].m_K[i][2]));
                //std::cout << (m_CalibParams[camIdx].m_K[i][0])<<"\t"<<(m_CalibParams[camIdx].m_K[i][1]) <<"\t"<< (m_CalibParams[camIdx].m_K[i][2]) << std::endl;
            }


            fscanf(pIn, "%lf", &dIn);
            fscanf(pIn, "%lf", &dIn);

            for(i = 0; i < 3; i ++)
            {
                for(j = 0; j < 3; j ++)
                {
                    fscanf(pIn, "%lf", &dIn);
                    m_CalibParams[camIdx].m_Rotmatrix[i][j] = dIn;
                }

                fscanf(pIn, "%lf", &dIn);
                m_CalibParams[camIdx].m_Trans[i] = dIn;
            }

        }
        fclose(pIn);
    }
}

/*
 * caculate camera indexs in camera array
 */
void loadData::get_ref_camera(int cur_view, int cam_ref[])
{
    if(cur_view % 4 == 1)
    {
        cam_ref[0] = cur_view / 4 + 1;
        cam_ref[1] = cur_view / 4 + 1;
    }
    else if(cur_view % 4 == 0)
    {
        cam_ref[0] = cur_view / 4;
        cam_ref[1] = cur_view / 4 + 1;
    }
    else
    {
        cam_ref[0] = cur_view / 4 + 1;
        cam_ref[1] = cur_view / 4 + 2;
    }
}

/*
 * caculate camera indexs in all views array
 */
void loadData::get_ref_cameraID_in_all_views(int cur_view, int cam_ref[])
{
    cur_view --;
    cam_ref[0] = cur_view - (cur_view % 4);
    cam_ref[1] = cam_ref[0] + 4;
}

/*
 * caculate projectionMatrix of all the views' calibParams (camera + virtual view)
 */
void loadData::get_allView_calibParams()
{
    int viewIdx, cam_ref[2], k;
    double (*inMat)[3];
    double exMat[3][4];
    for(viewIdx = 0; viewIdx < view_num; viewIdx ++)
    {
        k = viewIdx % 4;
        cam_ref[0] = viewIdx / 4;
        cam_ref[1] = viewIdx / 4 + 1;
        if(cam_ref[1] == camera_num) // arr beyond boundary
            cam_ref[1] = cam_ref[0];

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                allView_CalibParams[viewIdx].m_K[i][j] = m_CalibParams[cam_ref[0]].m_K[i][j] + 1.0 * (m_CalibParams[cam_ref[1]].m_K[i][j] - m_CalibParams[cam_ref[0]].m_K[i][j]) / 4 * k;
                std::cout << allView_CalibParams[viewIdx].m_K[i][j] << "\t";
            }
            std::cout << std::endl;
        }

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                allView_CalibParams[viewIdx].m_Rotmatrix[i][j] = m_CalibParams[cam_ref[0]].m_Rotmatrix[i][j] + 1.0 * (m_CalibParams[cam_ref[1]].m_Rotmatrix[i][j] - m_CalibParams[cam_ref[0]].m_Rotmatrix[i][j]) / 4 * k;
                std::cout << allView_CalibParams[viewIdx].m_Rotmatrix[i][j] << "\t";
            }
            allView_CalibParams[viewIdx].m_Trans[i] = m_CalibParams[cam_ref[0]].m_Trans[i] + 1.0 * (m_CalibParams[cam_ref[1]].m_Trans[i] - m_CalibParams[cam_ref[0]].m_Trans[i]) / 4 * k;
            std::cout << allView_CalibParams[viewIdx].m_Trans[i] << std::endl;
        }
        std::cout << std::endl;

        //caculate projectionMatrix
        inMat = allView_CalibParams[viewIdx].m_K;
        for(int i = 0; i < 3; i ++)
        {
            for(int j = 0; j < 3; j ++)
                exMat[i][j] = allView_CalibParams[viewIdx].m_Rotmatrix[i][j];
        }

        for(int i = 0; i < 3; i ++)
            exMat[i][3] = allView_CalibParams[viewIdx].m_Trans[i];

        for(int i = 0; i < 3; i ++)
            for(int j = 0; j < 4; j ++)
            {
                allView_CalibParams[viewIdx].m_ProjMatrix[i][j] = 0.0;
                for(int m = 0; m < 3; m ++)
                    allView_CalibParams[viewIdx].m_ProjMatrix[i][j] += inMat[i][m] * exMat[m][j];
            }

        allView_CalibParams[viewIdx].m_ProjMatrix[3][0] = 0.0;
        allView_CalibParams[viewIdx].m_ProjMatrix[3][1] = 0.0;
        allView_CalibParams[viewIdx].m_ProjMatrix[3][2] = 0.0;
        allView_CalibParams[viewIdx].m_ProjMatrix[3][3] = 1.0;
    }
}

