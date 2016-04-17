CC=arm-linux-androideabi-gcc
All :
	$(CC) -fpie -pie main.cpp ccKeyBoard.cpp -llog -o keyboard

install:
	adb push ../libs/armeabi-v7a/keyboard /data/CC
