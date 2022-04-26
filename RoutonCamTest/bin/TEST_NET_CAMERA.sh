#!bin/sh
export LD_LIBRARY_PATH=../lib/hikvision:../lib/opencv:$LD_LIBRARY_PATH

./RoutonCamTest 1 172.16.55.31 admin 123456jl 8000
