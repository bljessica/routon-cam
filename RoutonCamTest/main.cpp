#include <RoutonCamSDK.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>
#include <math.h>
#include <Windows.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// 上下左右键keycode
# define KEY_CODE_DIR_UP 82
# define KEY_CODE_DIR_DOWN 84
# define KEY_CODE_DIR_LEFT 81
# define KEY_CODE_DIR_RIGHT 83
# define KEY_CODE_FOCUS_NEAR 119
# define KEY_CODE_FOCUS_FAR 115
# define KEY_CODE_ZOOM_IN 69
# define KEY_CODE_ZOOM_OUT 68

void NetCameraDataCallback(int width, int height, unsigned char* buffer);
void SetCamSVRConfig(int keyCode);
void FocusCamByPosStr(std::vector<char> pos_char_arr);

cv::Mat gFrame;
std::vector<char> input_pos_char_arr;

int main(int argc, char* argv[])
{
	if (argc < 6)
	{
		printf("Input parameter invailed:\n");
		printf("Please format: ./RoutonCamTest [TYPE] [IP] [USERNAME] [PASSWORD] [HOST]\n");
		printf("eg. ./RoutonCamTest 1 172.16.55.31 admin 123456jl 8000\n");
		return 0;
	}

	CamInit(1, 1);
	CAMERA mCamera;
	mCamera.iType = atoi(argv[1]);
	sprintf(mCamera.sIP, "%s", argv[2]);
	sprintf(mCamera.sUsername, "%s", argv[3]);
	sprintf(mCamera.sPwd, "%s", argv[4]);
	mCamera.iHost = atoi(argv[5]);

	CamLogin(mCamera);

	//CamPTZCtl(ROUTON_PAN_LEFT, 10);
	//CamPTZCtl(ROUTON_TILT_UP, 10);

	CamPlay(PLAY_OPEN, 0, NetCameraDataCallback);


	printf("Enter Esc to exit.\n");
	Sleep(1000);

	//CamStartToRecordVideo();
	while (1) {

	 }
	//while (true) {
		//char keyCode = cv::waitKey(1);
		// cv::Rect rect(1000, 800, 200, 200);
		// cv::rectangle(gFrame, rect, cv::Scalar(0,0,255), 2); // 绘制人像矩形
		//if (keyCode == 'q') {
			//CamEndToRecordVideo();
			//break;
		//}
		//else if (keyCode == 255) { // 空格
		//    int* camDVRConfig = CamGetDVRConfig();
		//    printf("Cur p: %d, t: %d, z: %d\n", camDVRConfig[0], camDVRConfig[1], camDVRConfig[2]);
		//}
		//else  {
		//else if (keyCode != -1) {
			//printf("keycode: %c\n", keyCode);
			// CamSetZoomIn(1043, 320, 235, 397, 1280, 720);
			//CamSetPtzAndFocus(130, 45, 1, 1043, 320, 235, 397, 1280, 720);
			// CamScanAround();
			// // printf("Cur keycode:%d\n", keyCode);
			// if (keyCode >= 48 && keyCode <= 57 || keyCode == 32) { // 数字键或空格
			//     input_pos_char_arr.push_back(char(keyCode));
			//     // printf("Pushed %c\n", keyCode);
			// } else if (keyCode == 13) { // 回车
			//     FocusCamByPosStr(input_pos_char_arr);
			//     input_pos_char_arr.clear();
			// }
			// // ControlCamPTZ(keyCode);
		//}
	//}

	CamRelease();

	CamVersion();

	printf("test finish.\n");

	return 1;
}


// 控制摄像头精准移动聚焦
void SetCamSVRConfig(int keyCode) {
	int frame_rows = 1280;
	int frame_cols = 1920;
	int target_x = 300, target_y = 100; // 左上
	if (keyCode == 50) {
		target_x = 1500;
		target_y = 900;
		printf("右下  target_x: %d, target_y: %d, frame_cols: %d, frame_rows: %d\n", target_x, target_y, frame_cols, frame_rows);
	}
	else if (keyCode == 49) {
		printf("左上 target_x: %d, target_y: %d, frame_cols: %d, frame_rows: %d\n", target_x, target_y, frame_cols, frame_rows);
	}
	// CamSetSVRConfig(target_x, target_y, frame_rows, frame_cols);
}


// 控制云台
int ControlCamPTZ(int keyCode) {
	int ptzCmdCode = -1;
	if (keyCode == KEY_CODE_DIR_UP) {
		ptzCmdCode = ROUTON_TILT_UP;
		printf("UP\n");
	}
	else if (keyCode == KEY_CODE_DIR_DOWN) {
		ptzCmdCode = ROUTON_TILT_DOWN;
		printf("DOWN\n");
	}
	else if (keyCode == KEY_CODE_DIR_LEFT) { // 左右与摄像头旋转方向相反
		ptzCmdCode = ROUTON_PAN_RIGHT;
		printf("LEFT\n");
	}
	else if (keyCode == KEY_CODE_DIR_RIGHT) {
		ptzCmdCode = ROUTON_PAN_LEFT;
		printf("RIGHT\n");
	}
	else if (keyCode == KEY_CODE_FOCUS_NEAR) {
		ptzCmdCode = ROUTON_FOCUS_NEAR;
		printf("NEAR\n");
	}
	else if (keyCode == KEY_CODE_FOCUS_FAR) {
		ptzCmdCode = ROUTON_FOCUS_FAR;
		printf("FAR\n");
	}
	else if (keyCode == KEY_CODE_ZOOM_IN) {
		ptzCmdCode = ROUTON_ZOOM_IN;
		printf("ZOOM IN\n");
	}
	else if (keyCode == KEY_CODE_ZOOM_OUT) {
		ptzCmdCode = ROUTON_ZOOM_OUT;
		printf("ZOOM OUT\n");
	}
	if (ptzCmdCode > 0) {
		if (CamPTZCtl(ptzCmdCode, 1) == 1) {
			// printf("Control PTZ sucessfully.\n");
		}
	}
	return 1;
}


void NetCameraDataCallback(int width, int height, unsigned char* buffer)
{
	cv::Mat frame(height + height / 2, width, CV_8UC1);
	memcpy(frame.data, buffer, (height + height / 2) * width * sizeof(uchar));
	//cv::cvtColor(frame, frame, CV_YUV2BGR_YV12);
	cv::cvtColor(frame, frame, cv::COLOR_YUV2RGB_YV12);

	gFrame = cv::Mat(height, width, CV_8UC3);
	memcpy(gFrame.data, frame.data, height * width * 3 * sizeof(uchar));
	//cv::imwrite("test.jpg", gFrame);
	cv::imshow("TEST", gFrame);
	int keyCode = cv::waitKey(1);
	if (keyCode == 27) {
		int camDVRConfig[3];
		CamGetDVRConfig(camDVRConfig);
		printf("Cur p: %d, t: %d, z: %d\n", camDVRConfig[0], camDVRConfig[1], camDVRConfig[2]); 
			exit(1);
	}
	else if (keyCode != 255) {
		//printf("#############code: %d\n", keyCode);
		if (keyCode >= 48 && keyCode <= 57 || keyCode == 32) { // 数字键或空格
			input_pos_char_arr.push_back(char(keyCode));
			 printf("Pushed %c\n", keyCode);
		}
		else if (keyCode == 13) { // 回车
			FocusCamByPosStr(input_pos_char_arr);
			input_pos_char_arr.clear();
		}
	}
}


// 处理输入的坐标，并控制摄像头聚焦
void FocusCamByPosStr(std::vector<char> pos_char_arr) {
	char* pos_arr_ptr = &pos_char_arr.at(0);
	const char* split_char = " ";
	char* split_ptr = strtok(pos_arr_ptr, split_char);
	std::vector<int> pos_num_arr;
	while (split_ptr) {
		int tmp_num = std::atoi(split_ptr);
		pos_num_arr.push_back(tmp_num);
		split_ptr = strtok(NULL, split_char);
	}
	if (pos_num_arr.size() != 3) {
		printf("Pos input invalid!\n");
		return;
	}
	int p = pos_num_arr[0];
	int t = pos_num_arr[1];
	int z = pos_num_arr[2];
	printf("Target p t z: %d %d %d\n", p, t, z);
	CamSetDVRConfig(p, t, z);

	Sleep(5000);
	int camDVRConfig[3];
	CamGetDVRConfig(camDVRConfig);
	printf("Cur p: %d, t: %d, z: %d\n", camDVRConfig[0], camDVRConfig[1], camDVRConfig[2]);
	//printf("Cur %%%%% p: %d, t: %d, z: %d\n", camDVRConfig[0] % 1048576, camDVRConfig[1] % 1048591, camDVRConfig[2]);
	//int x = pos_num_arr[0];
	//int y = pos_num_arr[1];
	//printf("Target x y: %d %d\n", x, y);
	//// CamSetSVRConfig(x, y, gFrame.rows, gFrame.cols);
}
