#include "RoutonCam.h"
#include <RoutonCamCommon.h>
#include <vector>
#include <math.h>


LONG _nPort = -1;   // camera decode port
gDataCallBack _gDataCallBack = nullptr;
BYTE _pBuf[13500000] = { 0 }; // (3000 + 3000 / 2) * 3000 = 13500000

void CALLBACK HC_RealDataCallBack(LONG lPlayHandle, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize, void* pUser);
void CALLBACK DecCBFun(long nPort, char* pBuf, long nSize, FRAME_INFO* pFrameInfo, long nUser, long nReserved2);

RoutonCam::RoutonCam(int type, int log)
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
        NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
        NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };
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
        struPlayInfo.dwStreamType = 0;       //0-��������1-��������2-����3��3-����4���Դ�����
        struPlayInfo.dwLinkMode   = 0;       //0- TCP��ʽ��1- UDP��ʽ��2- �ಥ��ʽ��3- RTP��ʽ��4-RTP/RTSP��5-RSTP/HTTP
        struPlayInfo.bBlocked     = 1;       //0- ������ȡ����1- ����ȡ��

        _lRealPlayHandle = NET_DVR_RealPlay_V40(_lUserID, &struPlayInfo, HC_RealDataCallBack, NULL);
        */
    NET_DVR_CLIENTINFO clientInfo = { 0 };
    clientInfo.hPlayWnd = hWnd;         //��ҪSDK����ʱ�����Ϊ��Чֵ����ȡ��������ʱ����Ϊ��
    clientInfo.lChannel = 1;			//Ԥ��ͨ����
    clientInfo.lLinkMode = 0;			//���λ(31)Ϊ0��ʾ��������Ϊ1��ʾ������0��30λ��ʾ���ӷ�ʽ��0��TCP��ʽ��1��UDP��ʽ��2���ಥ��ʽ��3��RTP��ʽ;
    clientInfo.sMultiCastIP = NULL;     //�ಥ��ַ����Ҫ�ಥԤ��ʱ����

    BOOL bPreviewBlock = TRUE;       //�������������Ƿ�������0����1����
    _lRealPlayHandle = NET_DVR_RealPlay_V30(_lUserID, &clientInfo, HC_RealDataCallBack, nullptr, bPreviewBlock); // ����ʵʱԤ��
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

// ����ʵʱ���ݻص�  
void CALLBACK HC_RealDataCallBack(LONG lPlayHandle, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize, void* pUser)
{

    //int i = (int)pUser;
    //int i = (int)pUser;

    BOOL inData = FALSE;

    switch (dwDataType)
    {
    case NET_DVR_SYSHEAD:      //ϵͳͷ
    {
        if (_nPort >= 0)
        {
            break; //ͬһ·��������Ҫ��ε��ÿ����ӿ�
        }

        if (!PlayM4_GetPort(&_nPort)) //��ȡ���ſ�δʹ�õ�ͨ����
        {
            if (PlayM4_GetLastError(_nPort) != 0)
            {
            }
            break;
        }

        //PlayM4_CheckDiscontinuousFrameNum(nPort[i],TRUE);  //  ֡��������������һI֡

        if (dwBufSize > 0)
        {
            if (!PlayM4_OpenStream(_nPort, pBuffer, dwBufSize, 1024 * 1024 * 3))
            {
                break;
            }
            if (!PlayM4_SetDecCBStream(_nPort, 1))   // 1�� ��ʾ��Ƶ��
            {
            }

            //PlayM4_PlaySkipErrorData()
            //���ý���ص����� ֻ���벻��ʾ
            if (!PlayM4_SetDecCallBackMend(_nPort, DecCBFun, (long)pUser))
            {
                break;
            }

            //����Ƶ����
            if (!PlayM4_Play(_nPort, 0))
            {
                break;
            }
        }
        break;

    }
    case NET_DVR_STREAMDATA:   // ��������
    {
        if (dwBufSize > 0 && _nPort != -1)
        {
            inData = PlayM4_InputData(_nPort, pBuffer, dwBufSize);
            while (!inData)
            {
                Sleep(10000);
                inData = PlayM4_InputData(_nPort, pBuffer, dwBufSize);
            }
            break;
        }
    }

    default:
        inData = PlayM4_InputData(_nPort, pBuffer, dwBufSize);
        while (!inData)
        {
            Sleep(10000);
            inData = PlayM4_InputData(_nPort, pBuffer, dwBufSize);
        }
        break;
    }
}

void CALLBACK DecCBFun(long nPort, char* pBuf, long nSize, FRAME_INFO* pFrameInfo, long nUser, long nReserved2)
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

            memcpy(gFrameData[i].cData, frame.data, fw * fh * 3 * sizeof(unsigned char));  // ����֡����
            */
            memcpy(_pBuf, pBuf, (fh + fh / 2) * fw * sizeof(BYTE));
            //(*getFrameCallBack[i])(i, gFrmaeData[i]); // �ص����
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
    if (NET_DVR_StopRealPlay(_lRealPlayHandle))
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

// ��̨���ƣ�1 �ɹ� 0 ʧ�ܣ�
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

    if (!NET_DVR_PTZControl_Other(_lUserID, 1, ctl, 0)) // ��ʼ����
    {
        if (_iLog)
        {
            char time_str[256];
            Routon_GetSysTime(time_str);
            printf("%s %s >> PTZ control_Other failed, error code: %d\n", __func__, time_str, NET_DVR_GetLastError());
        }
        return 0;
    }

    Sleep(time * 1000);

    if (!NET_DVR_PTZControl_Other(_lUserID, 1, ctl, 1)) // ֹͣ����
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


//����ĽǶȲ�����ʮ������,��Ҫ��ʮ���������л�
int  HexToDecMa(int wHex)//ʮ������תʮ����
{
    return (wHex / 4096) * 1000 + ((wHex % 4096) / 256) * 100 + ((wHex % 256) / 16) * 10 + (wHex % 16);
}


int DEC2HEX_doc(int x)//ʮ����תʮ������
{
    return (x / 1000) * 4096 + ((x % 1000) / 100) * 256 + ((x % 100) / 10) * 16 + x % 10;
}


// ��ȡ����ͷ����
void RoutonCam::GetDVRConfig(int* tmpPos) {
    NET_DVR_PTZPOS m_ptzPos;
    DWORD dwTmp;
    if (!NET_DVR_GetDVRConfig(_lUserID, NET_DVR_GET_PTZPOS, 0, &m_ptzPos, sizeof(NET_DVR_PTZPOS), &dwTmp)) {
        printf("Get DVR Config failed.\n");
    }
    else {
        // ת�������Ϣ��ʮ����
        int m_iPara1 = HexToDecMa(m_ptzPos.wPanPos);
        int m_iPara2 = HexToDecMa(m_ptzPos.wTiltPos);
        int m_iPara3 = HexToDecMa(m_ptzPos.wZoomPos);
        tmpPos[0] = m_iPara1 / 10; // Pˮƽ���� 
        tmpPos[1] = m_iPara2 / 10; // T����
        tmpPos[2] = m_iPara3 / 10; // Z����
    }
}


// ��������ͷ����
void RoutonCam::SetDVRConfig(int wPanPos, int wTiltPos, int wZoomPos) {
    // printf("set config: %d %d %d\n", wPanPos, wTiltPos, wZoomPos);
    NET_DVR_PTZPOS m_ptzPos;
    m_ptzPos.wAction = 1;
    //����ת��
    m_ptzPos.wPanPos = DEC2HEX_doc(wPanPos * 10);
    m_ptzPos.wTiltPos = DEC2HEX_doc(wTiltPos * 10);
    m_ptzPos.wZoomPos = DEC2HEX_doc(wZoomPos * 10);
    if (!NET_DVR_SetDVRConfig(_lUserID, NET_DVR_SET_PTZPOS, 0, &m_ptzPos, sizeof(NET_DVR_PTZPOS))) {
        printf("Set DVR Config failed.\n");
    }
    // �ȴ�����ͷִ����
    bool finished_init = false;
    while (!finished_init) {
        Sleep(1000);
        int cur_config[3];
        GetDVRConfig(cur_config);
        // printf("cur: %d %d %d\n", cur_config[0], cur_config[1], cur_config[2]);
        if (cur_config[0] == wPanPos && cur_config[1] == wTiltPos && cur_config[2] == wZoomPos) {
            finished_init = true;
        }
    }
}


// ��������ͷ�۽�ĳ����
void RoutonCam::PTZSetZoomIn(int x, int y, int width, int height, int frame_width, int frame_height) {
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


// get hiksion sdk version
void RoutonCam::HiksionSDK_Version()
{
    unsigned int uiVersion = NET_DVR_GetSDKBuildVersion();
    char strTemp[1024] = { 0 };
    sprintf(strTemp, "HCNetSDK V%d.%d.%d.%d\n", \
        (0xff000000 & uiVersion) >> 24, \
        (0x00ff0000 & uiVersion) >> 16, \
        (0x0000ff00 & uiVersion) >> 8, \
        (0x000000ff & uiVersion));
    printf("%s", strTemp);
}


void RoutonCam::StartToRecordVideo() {
    char file_name[20] = "test.mp4";
    if (!NET_DVR_SaveRealData(_lRealPlayHandle, file_name)) {
        perror("Start to save video error.\n");
    }
}


void RoutonCam::EndToRecordVideo() {
    if (!NET_DVR_StopSaveRealData(_lRealPlayHandle)) {
        perror("End to save video error.\n");
    }
}

