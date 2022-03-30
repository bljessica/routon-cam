#ifndef ROUTONCAM_H
#define ROUTONCAM_H
#include <RoutonCamSDK.h>
#include <RoutonCamType.h>
#include <vector>

class RoutonCam
{
public:
    RoutonCam(int type = 1, int log = 0);
    int Init(CAMERA sCam); // init camera
    int Release();  // release

    int PlayOpen(HWND hWnd, gDataCallBack datacallback);  // get Net camera stream, format type: h264 / h265, open the preview stream
    int PlayClose();  // close the preview stream

    int PTZCtl_Support();   // check camera PTZ control
    int PTZCtl(int ctl, int time); // PTZ control

    // std::vector<int> GetDVRConfigHex();
    std::vector<int> GetDVRConfig();
    // void SetDVRConfig(int target_x, int target_y, int frame_rows, int frame_cols);
    void SetDVRConfig(int wPanPos, int wTiltPos, int wZoomPos);
    // void SetDVRConfigHex(int wPanPos, int wTiltPos, int wZoomPos);

private:
    //static void CALLBACK HC_RealDataCallBack(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser);
    //static void CALLBACK DecCBFun(long nPort, char *pBuf, long nSize, FRAME_INFO * pFrameInfo, long nUser, long nReserved2);

    void HiksionSDK_Version();

    int _iType; // camera type
    int _iLog;  // log signal,1:open 0:close

    LONG _lUserID;  // hikvison sdk login interfence
    LONG _lRealPlayHandle; // play stream (UI)
    /*
    static LONG _nPort;   // camera decode port
    static gDataCallBack _gDataCallBack;
    static BYTE _pBuf[13500000]; // (3000 + 3000 / 2) * 3000 = 13500000
    */
};

#endif // ROUTONCAM_H
