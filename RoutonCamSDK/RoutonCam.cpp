#include "RoutonCam.h"
#include <RoutonCamCommon.h>
#include <vector>
#include <math.h>


int _nPort = -1;   // camera decode port
gDataCallBack _gDataCallBack = nullptr;
BYTE _pBuf[13500000] = {0}; // (3000 + 3000 / 2) * 3000 = 13500000

void CALLBACK HC_RealDataCallBack(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser);
void CALLBACK DecCBFun(int nPort, char *pBuf, int nSize, FRAME_INFO *pFrameInfo, void *nUser, int nReserved2);

RoutonCam::RoutonCam(int type,int log)
{
    _iType = type;
    _iLog = log;
    _lUserID = -1;
    _lRealPlayHandle = -1;
    //_nPort = 0;
    //_gDataCallBack = nullptr;
}

// camera init
// 1:success 0:fail
int RoutonCam::Init(CAMERA sCam)
{
    if (_iType < 0 || _iType > 4)
    {
        if (_iLog)
        {
            char time_str[256];
            Routon_GetSysTime(time_str);
            printf("%s %s >> camera type invaild. \n", __func__, time_str);
        }
        return 0;
    }
    switch (_iType)
    {
    case 1:
        NET_DVR_Init();

        NET_DVR_SetConnectTime(2000, 1);
        NET_DVR_SetReconnect(10000, true);

        //login
        NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
        NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};
        struLoginInfo.bUseAsynLogin = false;

        struLoginInfo.wPort = sCam.iHost;
        memcpy(struLoginInfo.sDeviceAddress, sCam.sIP, NET_DVR_DEV_ADDRESS_MAX_LEN);
        memcpy(struLoginInfo.sUserName, sCam.sUsername, NAME_LEN);
        memcpy(struLoginInfo.sPassword, sCam.sPwd, NAME_LEN);

        _lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);

        if (_lUserID < 0)
        {
            if (_iLog)
            {
                char time_str[256];
                Routon_GetSysTime(time_str);
                printf("%s %s >> login error, %d\n", __func__, time_str, NET_DVR_GetLastError());
            }
            NET_DVR_Logout(_lUserID);
            NET_DVR_Cleanup();
            return 0;
        }
        break;
/*
    case 2:
        break;

    case 3:
        break;

    case 4:
        break;
*/
    }

    return 1;
}

// open the preview stream, ui
int RoutonCam::PlayOpen(HWND hWnd, gDataCallBack datacallback)
{
    if (_lUserID == -1)
    {
        if (_iType == 1 && _iLog)
        {
            char time_str[256];
            Routon_GetSysTime(time_str);
            printf("%s %s >> no login net camera.\n", __func__, time_str);
        }
        return 0;
    }
    _gDataCallBack = datacallback;

/*
    NET_DVR_PREVIEWINFO struPlayInfo = {0};
    struPlayInfo.hPlayWnd = hWnd;
    struPlayInfo.lChannel = 1;
    struPlayInfo.dwStreamType = 0;       //0-主码流，1-子码流，2-码流3，3-码流4，以此类推
    struPlayInfo.dwLinkMode   = 0;       //0- TCP方式，1- UDP方式，2- 多播方式，3- RTP方式，4-RTP/RTSP，5-RSTP/HTTP
    struPlayInfo.bBlocked     = 1;       //0- 非阻塞取流，1- 阻塞取流

    _lRealPlayHandle = NET_DVR_RealPlay_V40(_lUserID, &struPlayInfo, HC_RealDataCallBack, NULL);
    */
    NET_DVR_CLIENTINFO clientInfo = { 0 };
    clientInfo.hPlayWnd = hWnd;         //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
    clientInfo.lChannel = 1;			//预览通道号
    clientInfo.lLinkMode = 0;			//最高位(31)为0表示主码流，为1表示子码流0～30位表示连接方式：0－TCP方式；1－UDP方式；2－多播方式；3－RTP方式;
    clientInfo.sMultiCastIP = NULL;     //多播地址，需要多播预览时配置

    BOOL bPreviewBlock = TRUE;       //请求码流过程是否阻塞，0：否，1：是
    _lRealPlayHandle = NET_DVR_RealPlay_V30(_lUserID, &clientInfo, HC_RealDataCallBack, nullptr, bPreviewBlock); // 开启实时预览
    if (_lRealPlayHandle < 0)
    {
        if (_iLog)
        {
            char time_str[256];
            Routon_GetSysTime(time_str);
            printf("%s %s >> NET_DVR_RealPlay_V40 callback failed, error code: %d.\n", __func__, time_str, NET_DVR_GetLastError());
        }
        return 0;
    }

    return 1;
}

// 海康实时数据回调  
void CALLBACK HC_RealDataCallBack(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser)
{

	//int i = (int)pUser;
	//int i = (int)pUser;

	BOOL inData = FALSE;
   
	switch (dwDataType)
	{
	case NET_DVR_SYSHEAD:      //系统头
	{
		if (_nPort >= 0)
		{
			break; //同一路码流不需要多次调用开流接口
		}

		if (!PlayM4_GetPort(&_nPort)) //获取播放库未使用的通道号
		{
			if (PlayM4_GetLastError(_nPort) != 0)
			{
			}
			break;
		}

		//PlayM4_CheckDiscontinuousFrameNum(nPort[i],TRUE);  //  帧不连续，跳到下一I帧

		if (dwBufSize > 0)
		{
			if (!PlayM4_OpenStream(_nPort, pBuffer, dwBufSize, 1024 * 1024 * 3))
			{
				break;
			}
			if (!PlayM4_SetDecCBStream(_nPort, 1))   // 1： 表示视频流
			{
			}

			//PlayM4_PlaySkipErrorData()
			//设置解码回调函数 只解码不显示
			if (!PlayM4_SetDecCallBackMend(_nPort, DecCBFun, pUser))
			{
				break;
			}

			//打开视频解码
			if (!PlayM4_Play(_nPort, 0))
			{
				break;
			}
		}
		break;

	}
	case NET_DVR_STREAMDATA:   // 码流数据
	{
		if (dwBufSize > 0 && _nPort != -1)
		{
			inData = PlayM4_InputData(_nPort, pBuffer, dwBufSize);
			while (!inData)
			{
				sleep(10);
				inData = PlayM4_InputData(_nPort, pBuffer, dwBufSize);
			}
			break;
		}
	}

	default:
		inData = PlayM4_InputData(_nPort, pBuffer, dwBufSize);
		while (!inData)
		{
			sleep(10);
			inData = PlayM4_InputData(_nPort, pBuffer, dwBufSize);
		}
		break;
	}
}

void CALLBACK DecCBFun(int nPort, char *pBuf, int nSize, FRAME_INFO * pFrameInfo, void* nUser, int nReserved2)
{
	//int i = (int)nUser;

	long lFrameType = pFrameInfo->nType;
	int fw = pFrameInfo->nWidth;
	int fh = pFrameInfo->nHeight;
 
	if (lFrameType == T_YV12)
	{
		if (pBuf != nullptr && (fw * fh < 3000 * 3000))
		{
            /*
			cv::Mat frame(fh + fh / 2, fw, CV_8UC1);
			memcpy(frame.data, pBuf, (fh + fh / 2) * fw * sizeof(uchar));
			cv::cvtColor(frame, frame, CV_YUV2BGR_YV12);
			gFrameData[i].iW = fw;
			gFrameData[i].iH = fh;
            
			memcpy(gFrameData[i].cData, frame.data, fw * fh * 3 * sizeof(unsigned char));  // 复制帧数据
            */
            memcpy(_pBuf, pBuf, (fh + fh / 2) * fw * sizeof(BYTE));
			//(*getFrameCallBack[i])(i, gFrmaeData[i]); // 回调结果
			//_gDataCallBack(i, gFrameData[i]);
            _gDataCallBack(fw, fh, _pBuf);

		
		}
		
	}
}


int RoutonCam::PlayClose()
{
    if (_lRealPlayHandle == -1)
    {
        if (_iLog)
        {
            char time_str[256];
            Routon_GetSysTime(time_str);
            printf("%s %s >> play no open.\n", __func__, time_str);
        }
        return 0;
    }

    //close the preview stream
    if(NET_DVR_StopRealPlay(_lRealPlayHandle))
    {
        if (_iLog)
        {
            char time_str[256];
            Routon_GetSysTime(time_str);
            printf("%s %s >> NET_DVR_StopRealPlay failed, error code: %d.\n", __func__, time_str, NET_DVR_GetLastError());
        }
        return 0;
    }
    return 1;
}

int RoutonCam::Release()
{
    switch (_iType)
    {
    case 1:
        //logout
        NET_DVR_Logout(_lUserID);
        NET_DVR_Cleanup();
        break;
    }
    return 1;
}

// check camera ptz is or not supported
int RoutonCam::PTZCtl_Support()
{
    return 1;
}

// 云台控制（1 成功 0 失败）
int RoutonCam::PTZCtl(int ctl, int time)
{
    if (_iType != 1)
    {
        if (_iLog)
        {
            char time_str[256];
            Routon_GetSysTime(time_str);
            printf("%s %s >> camera type = %d, no support PTZ control.\n", __func__, time_str, _iType);
        }
        return 0;
    }
   
    if(!NET_DVR_PTZControl_Other(_lUserID,1,ctl,0)) // 开始控制
    {
        if (_iLog)
        {
            char time_str[256];
            Routon_GetSysTime(time_str);
            printf("%s %s >> PTZ control_Other failed, error code: %d\n", __func__, time_str, NET_DVR_GetLastError());
        }
        return 0;
    }

    sleep(time);
    
    if(!NET_DVR_PTZControl_Other(_lUserID,1,ctl,1)) // 停止控制
    {
        if (_iLog)
        {
            char time_str[256];
            Routon_GetSysTime(time_str);
            printf("%s %s >> PTZ control_Other failed, error code: %d\n", __func__, time_str, NET_DVR_GetLastError());
        }
        return 0;
    }
    return 1;
}


//相机的角度参数是十六进制,需要和十进制来回切换
int  HexToDecMa(int wHex)//十六进制转十进制
{
    return (wHex / 4096) * 1000 + ((wHex % 4096) / 256) * 100 + ((wHex % 256) / 16) * 10 + (wHex % 16);
}


int DEC2HEX_doc(int x)//十进制转十六进制
{
    return (x / 1000) * 4096 + ((x % 1000) / 100) * 256 + ((x % 100) / 10) * 16 + x % 10;
}


// 读取摄像头参数
std::vector<int> RoutonCam::GetDVRConfig() {
    NET_DVR_PTZPOS m_ptzPos;
    DWORD dwTmp;
    std::vector<int> tmpPos(3);
    if (!NET_DVR_GetDVRConfig(_lUserID, NET_DVR_GET_PTZPOS, 0, &m_ptzPos, sizeof(NET_DVR_PTZPOS), &dwTmp)) {
        printf("Get DVR Config failed.\n");
    } else {
        // 转换相机信息到十进制
        int m_iPara1 = HexToDecMa(m_ptzPos.wPanPos);
        int m_iPara2 = HexToDecMa(m_ptzPos.wTiltPos);
        int m_iPara3 = HexToDecMa(m_ptzPos.wZoomPos);
        tmpPos[0] = m_iPara1 / 10; // P水平方向 
        tmpPos[1] = m_iPara2 / 10; // T仰角
        tmpPos[2] = m_iPara3 / 10; // Z焦距
    }
    return tmpPos;
}


// 读取十六进制摄像头参数
// std::vector<int> RoutonCam::GetDVRConfigHex() {
//     NET_DVR_PTZPOS m_ptzPos;
//     DWORD dwTmp;
//     std::vector<int> tmpPos(3);
//     if (!NET_DVR_GetDVRConfig(_lUserID, NET_DVR_GET_PTZPOS, 0, &m_ptzPos, sizeof(NET_DVR_PTZPOS), &dwTmp)) {
//         printf("Get DVR Config failed.\n");
//     } 
//     return tmpPos;
// }


// 设置摄像头参数
// void RoutonCam::SetDVRConfigHex(int wPanPos, int wTiltPos, int wZoomPos) {
//     NET_DVR_PTZPOS m_ptzPos;
//     m_ptzPos.wAction = 1;
//     //进制转换
//     m_ptzPos.wPanPos = wPanPos;
//     m_ptzPos.wTiltPos = wTiltPos;
//     m_ptzPos.wZoomPos = wZoomPos;
//     if (!NET_DVR_SetDVRConfig(_lUserID, NET_DVR_SET_PTZPOS, 0, &m_ptzPos, sizeof(NET_DVR_PTZPOS))) {
//         printf("Set DVR Config failed.\n");
//     }
// }  


// 设置摄像头参数
void RoutonCam::SetDVRConfig(int wPanPos, int wTiltPos, int wZoomPos) {
    // printf("set config: %d %d %d\n", wPanPos, wTiltPos, wZoomPos);
    NET_DVR_PTZPOS m_ptzPos;
    m_ptzPos.wAction = 1;
    //进制转换
    m_ptzPos.wPanPos = DEC2HEX_doc(wPanPos * 10);
    m_ptzPos.wTiltPos = DEC2HEX_doc(wTiltPos * 10);
    m_ptzPos.wZoomPos = DEC2HEX_doc(wZoomPos * 10);
    if (!NET_DVR_SetDVRConfig(_lUserID, NET_DVR_SET_PTZPOS, 0, &m_ptzPos, sizeof(NET_DVR_PTZPOS))) {
        printf("Set DVR Config failed.\n");
    }
    // 等待摄像头执行完
    bool finished_init = false;
    while (!finished_init) {
        sleep(1);
        std::vector<int> cur_config = GetDVRConfig();
        // printf("cur: %d %d %d\n", cur_config[0], cur_config[1], cur_config[2]);
        if (cur_config[0] == wPanPos && cur_config[1] == wTiltPos && cur_config[2] == wZoomPos) {
            finished_init = true;
        }
    } 
}  


// 控制摄像头聚焦某区域
void RoutonCam::PTZSetZoomIn(int x, int y, int width, int height, int frame_width, int frame_height) {
    // printf("set zoom in: x: %d, y: %d, w: %d, h: %d, f_w: %d, f_h: %d\n", x, y, width, height, frame_width, frame_height);
    NET_DVR_POINT_FRAME pos_data;
    pos_data.xTop = (int)(x * 255 / frame_width);
    pos_data.xBottom = (int)((x + width) * 255 / frame_width);
    pos_data.yTop = (int)(y * 255 / frame_height);
    pos_data.yBottom = (int)((y + height) * 255 / frame_height);
    pos_data.bCounter = 1;
    if (!NET_DVR_PTZSelZoomIn(_lRealPlayHandle, &pos_data)) {
        printf("Set PTZ zoom in failed, error code: %d.\n", NET_DVR_GetLastError());
    }
}


// 设置摄像头参数
// void RoutonCam::SetDVRConfig(int target_x, int target_y, int frame_rows, int frame_cols) {
//     std::vector<int> curPtzPos = GetDVRConfig();
//     int focus_adjust = 10; // 焦距

//     int y_degree = (target_y - frame_rows / 2.0) > 0 ? -atan(-(frame_rows / 2.0 - target_y) / (frame_rows / 2) * tan(21.8 / 180 * M_PI)) * 180 / M_PI : atan(-(target_y - frame_rows / 2.0) / (frame_rows / 2) * tan(21.8 / 180 * M_PI)) * 180 / M_PI;
//     int z_degree = ((target_x - frame_cols / 2.0) > 0) ? atan((target_x - frame_cols / 2.0) / (frame_cols / 2) * tan(29.15 / 180 * M_PI)) * 180 / M_PI : (360 - atan((frame_cols / 2.0 - target_x) / (frame_cols / 2) * tan(29.15 / 180 * M_PI)) * 180 / M_PI);
//     z_degree = ((z_degree < 100) ? -z_degree : 360 - z_degree);

//     // 补上球机初始矫正角度
//     z_degree = -z_degree + curPtzPos[0]; 
//     y_degree = -y_degree + curPtzPos[1];

//     NET_DVR_PTZPOS m_ptzPos;
//     m_ptzPos.wAction = 1;
//     int z_idegree = z_degree * 10;
//     int y_idegree = y_degree * 10;
//     //进制转换
//     m_ptzPos.wPanPos = DEC2HEX_doc(z_idegree);
//     m_ptzPos.wTiltPos = DEC2HEX_doc(y_idegree);
//     m_ptzPos.wZoomPos = DEC2HEX_doc(focus_adjust);

//     if (!NET_DVR_SetDVRConfig(_lUserID, NET_DVR_SET_PTZPOS, 0, &m_ptzPos, sizeof(NET_DVR_PTZPOS))) {
//         printf("Set DVR Config failed.\n");
//     }
// }


// get hiksion sdk version
void RoutonCam::HiksionSDK_Version()
{
    unsigned int uiVersion = NET_DVR_GetSDKBuildVersion();
    char strTemp[1024] = {0};
    sprintf(strTemp, "HCNetSDK V%d.%d.%d.%d\n", \
            (0xff000000 & uiVersion)>>24, \
            (0x00ff0000 & uiVersion)>>16, \
            (0x0000ff00 & uiVersion)>>8, \
            (0x000000ff & uiVersion));
    printf("%s",strTemp);
}


void RoutonCam::StartToRecordVideo() {
    if (!NET_DVR_SaveRealData(_lRealPlayHandle, "test.mp4")) {
        perror("Start to save video error.\n");
    }
}


void RoutonCam::EndToRecordVideo() {
    if (!NET_DVR_StopSaveRealData(_lRealPlayHandle)) {
        perror("End to save video error.\n");
    }
}

