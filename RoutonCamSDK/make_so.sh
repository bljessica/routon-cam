cmake .
make
cp ./build/lib/libRoutonCamSDK.so ../RoutonCamTest/lib
cp ./build/lib/libRoutonCamSDK.so ../../person_detect_project/lib/CamSDK/libRoutonCamSDK.so
cp ./RoutonCamSDK.h ../RoutonCamTest/include
cp ./RoutonCamSDK.h ../../person_detect_project/include/CamSDK/RoutonCamSDK.h
echo 'Copied'