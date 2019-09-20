#include "loaddata.h"

loadData::loadData()
{
    camera_num = 8;
    vir_num = 3;
    view_num = (camera_num-1)*(vir_num+1) + 1;

    cur_camera = 3;
    cur_view = cur_camera*(vir_num + 1);
    frame_num = 0;

    basePath = "F:\\wei\\freeViewProject\\test_pic_video\\ballet-dancer\\MSR3DVideo-Ballet\\";
    calibParams_path = "F:\\wei\\freeViewProject\\test_pic_video\\ballet-dancer\\MSR3DVideo-Ballet\\calibParams-ballet.txt";

    path_arr = new string[camera_num];
    frame_imgArr = new cv::Mat[2];
    frame_depArr = new cv::Mat[2];
    m_CalibParams = new CalibStruct[camera_num];
    allView_CalibParams = new CalibStruct[view_num + camera_num];

    pts = new double[3];

    height = 768;
    width = 1024;
    MaxZ = 130.0;
    MinZ = 42.0;

    for(int i = 0; i < camera_num; i ++)
        path_arr[i] = basePath + "cam" + char('0'+i) + "\\";
}

void loadData::get_frame_imgArr(int frame_num, int left_cam, int right_cam)
{
    char tmp[20];
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
void loadData::get_ref_camera(int cur_view)
{
    if(cur_view % (vir_num + 1) == 0)           //cur_view is at the camera position
    {
        refIdx_in_cam_arr[0] = cur_view / (vir_num + 1);
        refIdx_in_cam_arr[1] = cur_view / (vir_num + 1);
    }
    else                                        //cur_view is between two cameras
    {
        refIdx_in_cam_arr[0] = cur_view / (vir_num + 1);
        refIdx_in_cam_arr[1] = cur_view / (vir_num + 1) + 1;
    }
}

/*
 * caculate camera indexs in all views array
 */
void loadData::get_ref_cameraID_in_all_views(int cur_view)      //cur_view is at the camera position
{
    if(cur_view % (vir_num + 1) == 0)
    {
        refIdx_in_view_arr[0] = cur_view;
        refIdx_in_view_arr[1] = cur_view;
    }
    else                                                        //cur_view is between two cameras
    {
        refIdx_in_view_arr[0] = cur_view - cur_view % (vir_num + 1);
        refIdx_in_view_arr[1] = refIdx_in_view_arr[0] + vir_num + 1;
    }
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
        cout << viewIdx << endl;
        k = viewIdx % (vir_num + 1);
        cam_ref[0] = viewIdx / (vir_num + 1);
        cam_ref[1] = viewIdx / (vir_num + 1) + 1;
        if(cam_ref[1] == camera_num)            //beyond camera index boundary
            cam_ref[1] = cam_ref[0];

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                allView_CalibParams[viewIdx].m_K[i][j] = m_CalibParams[cam_ref[0]].m_K[i][j] + 1.0 * (m_CalibParams[cam_ref[1]].m_K[i][j] - m_CalibParams[cam_ref[0]].m_K[i][j]) / (vir_num + 1) * k;
                //std::cout << allView_CalibParams[viewIdx].m_K[i][j] << "\t";
            }
            //std::cout << std::endl;
        }

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                allView_CalibParams[viewIdx].m_Rotmatrix[i][j] = m_CalibParams[cam_ref[0]].m_Rotmatrix[i][j] + 1.0 * (m_CalibParams[cam_ref[1]].m_Rotmatrix[i][j] - m_CalibParams[cam_ref[0]].m_Rotmatrix[i][j]) / (vir_num + 1) * k;
                //std::cout << allView_CalibParams[viewIdx].m_Rotmatrix[i][j] << "\t";
            }
            allView_CalibParams[viewIdx].m_Trans[i] = m_CalibParams[cam_ref[0]].m_Trans[i] + 1.0 * (m_CalibParams[cam_ref[1]].m_Trans[i] - m_CalibParams[cam_ref[0]].m_Trans[i]) / (vir_num + 1) * k;
            //std::cout << allView_CalibParams[viewIdx].m_Trans[i] << std::endl;
        }
        //std::cout << std::endl;

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

        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++)
                std::cout << allView_CalibParams[viewIdx].m_K[j][k] << "\t";
            std::cout << std::endl;
        }

        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++)
                std::cout << allView_CalibParams[viewIdx].m_Rotmatrix[j][k] << "\t";
            std::cout << std::endl;
        }
        for (int k = 0; k < 3; k++)
            std::cout << allView_CalibParams[viewIdx].m_Trans[k] << "\t";
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
    }
}

