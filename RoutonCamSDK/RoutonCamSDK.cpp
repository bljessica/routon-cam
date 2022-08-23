#include <RoutonCamSDK.h>
#include <RoutonCam.h>
#include <cmath>


RoutonCam *gCAM = nullptr;
gGetDataCallback gDataCall = nullptr;
LONG _lPort = -1;

int CamInit(int type, int log)
{
    try
    {
        gCAM = new RoutonCam(type, log);
    }
    catch (...)
    {
        printf("%s >> throw exception, init failed.\n", __func__);
        return 0;
    }

    return 1;
}

int CamLogin(CAMERA cam)
{
    if (gCAM == nullptr)
    {
        printf("%s >> login failed, no init.\n", __func__);
        return 0;
    }

    return gCAM->Init(cam);
}

/*
// data callback
void GetDataCallback(LONG lRealHandle, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize,void* dwUser)
{
    switch (dwDataType)
    {
        case NET_DVR_SYSHEAD: //系统头
            if (_lPort >= 0)
            {
                break;  //该通道取流之前已经获取到句柄，后续接口不需要再调用
            }

            if (!PlayM4_GetPort(&_lPort))  //获取播放库未使用的通道号
            {
                break;
            }
            if (dwBufSize > 0)
            {
                if (!PlayM4_SetStreamOpenMode(_lPort, STREAME_REALTIME))  //设置实时流播放模式
                {
                    break;
                }

                if (!PlayM4_OpenStream(_lPort, pBuffer, dwBufSize, 1024*1024 * 3)) //打开流接口
                {
                    break;
                }
                PlayM4_SetDecCBStream(_lPort, 1);   // 1: video stream

                if (!PlayM4_SetDecCallBack(_lPort, nullptr));
                {
                    break;
                }
#ifdef __WIN32
                if (!PlayM4_Play(_lPort, nullptr)) //播放开始
#else
                if (!PlayM4_Play(_lPort, 0)) //播放开始
#endif
                {
                    break;
                }
            }
            break;

        case NET_DVR_STREAMDATA:   //码流数据
            if (dwBufSize > 0 && _lPort != -1)
            {
                if (!PlayM4_InputData(_lPort, pBuffer, dwBufSize))
                {
                    break;
                }
                gDataCall(100,100, pBuffer);
            }
            break;
        default: //其他数据
            if (dwBufSize > 0 && _lPort != -1)
            {
                if (!PlayM4_InputData(_lPort, pBuffer, dwBufSize))
                {
                    break;
                }
            }
            break;
    }
}
*/

int CamPlay(int status, HWND hwnd, gGetDataCallback callback)
{
    if (status != PLAY_OPEN && status != PLAY_CLOSE)
    {
        printf("%s >> status paramer invaild.\n", __func__);
        return 0;
    }

    if (gCAM == nullptr)
    {
        printf("%s >> play failed, no init.\n", __func__);
        return 0;
    }

    if (status == PLAY_OPEN)
    {
#ifdef _WIN32
        if (callback == nullptr && hwnd == nullptr)
#else
        if (callback == nullptr && hwnd == 0)
#endif
        {
            printf("%s >> status = PLAY_OPEN, hwnd or callback paramer invaild.\n", __func__);
            return 0;
        }
        if (gDataCall == nullptr)
        {
            gDataCall = callback;
        }
        gCAM->PlayOpen(hwnd, gDataCall);
    }
    else
    {
        gCAM->PlayClose();
    }
    return 1;
}

int CamPTZCtl(int ctl, int time)
{
    if (gCAM == nullptr)
    {
        printf("%s >> PTZ control failed, no init.\n", __func__);
        return 0;
    }

    return gCAM->PTZCtl(ctl, time);
}


std::vector<int> CamGetDVRConfig() {
    return gCAM->GetDVRConfig();
}

void CamSetDVRConfig(int wPanPos, int wTiltPos, int wZoomPos) {
    return gCAM->SetDVRConfig(wPanPos, wTiltPos, wZoomPos);
}


// void CamSetDVRConfig(int target_x, int target_y, int frame_rows, int frame_cols) {
//     return gCAM->SetDVRConfig(target_x, target_y, frame_rows, frame_cols);
// }



// 摄像头扫视四周
void CamScanAround(bool *is_scan_finished) {
    printf("Camera start to scan.\n");
    const int target_num = 5;
    if (is_scan_finished != nullptr) {
        *is_scan_finished = false;
        int targets[target_num][3] = {
            // 0, 0, 1,
            // 180, 0, 1,
            // 350, 0, 1,

            0, 90, 1,
            0, 45, 1,

            180, 45, 1,

            350, 45, 1,
            350, 90, 1,

        };
        std::vector<int> curPos = CamGetDVRConfig();
        if (abs(curPos[0] - 0) > abs(curPos[0] - 350)) { // 翻转数组
            for (int i = 0; i < target_num / 2; ++i) {
                int p = targets[target_num - i - 1][0], t = targets[target_num - i - 1][1];
                targets[target_num - i - 1][0] = targets[i][0];
                targets[target_num - i - 1][1] = targets[i][1];
                targets[i][0] = p;
                targets[i][1] = t;
            }
        }

        // bool finished_init;
        for (int i = 0; i < target_num; i++) {
            // finished_init = false;
            gCAM->SetDVRConfig(targets[i][0], targets[i][1], targets[i][2]);  
            // while (!finished_init) {
            //     sleep(2);
            //     std::vector<int> cur_config = gCAM->GetDVRConfig();
            //     if (cur_config[0] == targets[i][0] && cur_config[1] == targets[i][1] && cur_config[2] == targets[i][2]) {
            //         finished_init = true;
            //     }
            // } 
        }
        *is_scan_finished = true;
    }
    printf("Camera finished scanning.\n");
}


void CamSetZoomIn(int x, int y, int width, int height, int frame_width, int frame_height) {
    gCAM->PTZSetZoomIn(x, y, width, height, frame_width, frame_height);
}


void CamSetPtzAndFocus(int p, int t, int z, int x, int y, int width, int height, int frame_width, int frame_height) {
    bool finished_init = false;
    gCAM->SetDVRConfig(p, t, z);  
    while (!finished_init) {
        sleep(2);
        std::vector<int> cur_config = gCAM->GetDVRConfig();
        if (cur_config[0] == p && cur_config[1] == t && cur_config[2] == z) {
            finished_init = true;
        }
    } 
    gCAM->PTZSetZoomIn(x, y, width, height, frame_width, frame_height);
}



int CamRelease()
{
    if (gCAM == nullptr)
    {
        printf("%s >> release failed, no init.\n", __func__);
        return 0;
    }

    return gCAM->Release();
}

// SDK verison
void CamVersion()
{
    int version_major = 1;
    int version_minor = 0;
    int version_patch = 0;
    int version_date = 20211207;
    printf("RoutonCamSDK verison-v%d.%d.%d.%d\n", version_major, version_minor, version_patch, version_date);
}


void CamStartToRecordVideo() {
    return gCAM->StartToRecordVideo();
}


void CamEndToRecordVideo() {
    return gCAM->EndToRecordVideo();
}

