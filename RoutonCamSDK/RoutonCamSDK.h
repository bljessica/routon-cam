#pragma once
#include <vector>
#include <Windows.h>

#ifndef ROUTONCAMSDK_H
#define ROUTONCAMSDK_H

#if (defined(_WIN32)) //windows
#define ROUTONCAM_API  extern "C" __declspec(dllimport)
#else
#define ROUTONCAM_API extern "C"
#endif

#ifndef HWND
#if defined(__linux__)
typedef unsigned int HWND;
#else
#include <windows.h>
#endif
#endif

#define TEST

typedef struct tagCAMERA
{
    int iType;  // 1��Net Camera 2:rtsp or video 3: USB Camera

    // net camera
    char sIP[129];  // ip
    char sUsername[32]; // username
    char sPwd[32]; // password
    int iHost;  // host

    // rtsp or video
    char sRtsp[129];    // rtsp url or local video path

    // USB Camera
    int iIndex; // usb index

}CAMERA, * LPCAMERA;

/*
 *  |5   1   6|
 *  |3   *   4|
 *  |7   2   8|
 */
enum PTZ_CONTROL {
    ROUTON_ZOOM_IN = 11,    // the focal length bigger - ������
    ROUTON_ZOOM_OUT = 12,   // the focal length smaller - �����С
    ROUTON_FOCUS_NEAR = 13, // before the focus adjustment - ����ǰ��
    ROUTON_FOCUS_FAR = 14,  // after the focus adjustment - ������
    ROUTON_IRIS_OPEN = 15,  // aperture expanding - ��Ȧ����
    ROUTON_IRIS_CLOSE = 16, // aperture narrowing - ��Ȧ��С
    ROUTON_TILT_UP = 21,    // pitch up - ��̨����
    ROUTON_TILT_DOWN = 22,  // pitch under - ��̨�¸�
    ROUTON_PAN_LEFT = 23,   // left-handed ratation - ��̨��ת
    ROUTON_PAN_RIGHT = 24,  // right-handed ratation - ��̨��ת
    ROUTON_UP_LEFT = 25,    // pitch up and left-handed rotation - ��̨��������ת
    ROUTON_UP_RIGHT = 26,   // pitch up and right-handed rotation - ��̨��������ת
    ROUTON_DOWN_LEFT = 27,  // pitch under and left-handed rotation - ��̨�¸�����ת
    ROUTON_DOWN_RIGHT = 28, // pitch under and right-handed rotation�� - ��̨�¸�����ת
    ROUTON_PAN_AUTO = 29    // left-handed or right-handed auto-rotation - ��̨�����Զ�ɨ��
};

// play status
enum PLAY_STATUS {
    PLAY_OPEN = 1,  // open the preview stream
    PLAY_CLOSE = 0  // close the preview stream
};

// data callback
//typedef void (*gGetDataCallback)(int width, int height, unsigned char* buffer);
typedef void (*gGetDataCallback)(int width, int height, unsigned char* buffer);

#ifndef TEST

ROUTONCAM_API int __stdcall CamInit(int type, int log);
ROUTONCAM_API int __stdcall CamLogin(CAMERA cam);
ROUTONCAM_API int __stdcall CamPlay(int status, HWND hwnd, gGetDataCallback callback);
ROUTONCAM_API int __stdcall CamPTZCtl(int ctl, int time);
ROUTONCAM_API int __stdcall CamRelease();
ROUTONCAM_API void __stdcall CamVerison();

#else

extern "C" _declspec(dllexport) int CamInit(int type, int log);
extern "C" _declspec(dllexport) int CamLogin(CAMERA cam);
extern "C" _declspec(dllexport) int CamPlay(int status, HWND hwnd, gGetDataCallback callback);
extern "C" _declspec(dllexport) int CamPTZCtl(int ctl, int time);
extern "C" _declspec(dllexport) void CamGetDVRConfig(int* tmpPos);
extern "C" _declspec(dllexport) void CamSetDVRConfig(int wPanPos, int wTiltPos, int wZoomPos);
// void CamSetDVRConfig(int target_x, int target_y, int frame_rows, int frame_cols);
//extern "C" _declspec(dllexport) void CamScanAround(bool* is_scan_finished = nullptr, int time_interval = 0);
extern "C" _declspec(dllexport) void CamScanAround(bool* is_scan_finished, int time_interval, int targets[][3], int targets_num, int rounds);
extern "C" _declspec(dllexport) void CamSetZoomIn(int x, int y, int width, int height, int frame_width, int frame_height);
extern "C" _declspec(dllexport) void CamSetPtzAndFocus(int p, int t, int z, int x, int y, int width, int height, int frame_width, int frame_height);
extern "C" _declspec(dllexport) int CamRelease();
extern "C" _declspec(dllexport) void CamVersion();
extern "C" _declspec(dllexport) void CamStartToRecordVideo();
extern "C" _declspec(dllexport) void CamEndToRecordVideo();

#endif

#endif // ROUTONCAMSDK_H
