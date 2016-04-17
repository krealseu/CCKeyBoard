//
// Created by lthee on 2016/2/26.
//

#ifndef NDKTEST_CCKEYBOARD_H
#define NDKTEST_CCKEYBOARD_H
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

class ccKeyBoard {
public:
    //ccKeyBoard();
    //~ccKeyBoard();
    int setup_uinput_device(int type);
    int close_uinput_device();
    int send(uint16_t type, int32_t keycode, int32_t value);
    int getState();
private:
    int mtpye;
    int uinp_fd;
    int send_unit(uint16_t type, uint16_t keycode, int32_t value);

};


#endif //NDKTEST_CCKEYBOARD_H
