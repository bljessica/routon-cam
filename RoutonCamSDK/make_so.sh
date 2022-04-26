cmake .
make
cp ./build/lib/libRoutonCamSDK.so ../RoutonCamTest/lib
cp ./build/lib/libRoutonCamSDK.so ../../person_detect_project/lib/CamSDK/
cp ./build/lib/libRoutonCamSDK.so ../../cam-control-for-rk/lib/CamSDK/
cp ./RoutonCamSDK.h ../RoutonCamTest/include
cp ./RoutonCamSDK.h ../../person_detect_project/include/CamSDK/
cp ./RoutonCamSDK.h ../../cam-control-for-rk/include/CamSDK/
echo 'Copied'