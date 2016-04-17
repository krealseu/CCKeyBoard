//
// Created by lthee on 2016/2/26.
//

#include "ccKeyBoard.h"

int ccKeyBoard::setup_uinput_device(int type) {
    int i=0;
    // Open the input device
    uinp_fd = open("/dev/uinput", O_WRONLY | O_NDELAY);
    if (uinp_fd <=0)
    {
        perror("Unable to open /dev/uinput/n");
        return -1;
    }
    struct uinput_user_dev uinp;
    memset(&uinp,0,sizeof(uinp)); // Intialize the uInput device to NULL
    strncpy(uinp.name, "CC KeyBoard", UINPUT_MAX_NAME_SIZE);
    uinp.id.version = 4;
    uinp.id.bustype = BUS_USB;

    // keyboard
    if(type == 0){
        ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);
        ioctl(uinp_fd, UI_SET_EVBIT, EV_SYN);
        for (i=0; i < 127; i++) {
            ioctl(uinp_fd, UI_SET_KEYBIT, i);
        }
    }
    //mouse
    ioctl(uinp_fd, UI_SET_EVBIT, EV_REL);
    ioctl(uinp_fd, UI_SET_RELBIT, REL_X);
    ioctl(uinp_fd, UI_SET_RELBIT, REL_Y);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MOUSE);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_TOUCH);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MOUSE);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MIDDLE);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_FORWARD);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_BACK);
    /* Create input device into input sub-system */
    write(uinp_fd, &uinp, sizeof(uinp));
    if (ioctl(uinp_fd, UI_DEV_CREATE))
    {
        perror("Unable to create UINPUT device.");
        return -1;
    }
    return 1;
}

int ccKeyBoard::close_uinput_device() {
    /* Destroy the input device */
    ioctl(uinp_fd, UI_DEV_DESTROY);
    /* Close the UINPUT device */
    close(uinp_fd);
    return 1;
}

int ccKeyBoard::send_unit(uint16_t type, uint16_t keycode, int32_t value) {
    struct input_event ev;
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = type;
    ev.code = keycode;
    ev.value = value;
    if (write(uinp_fd, &ev, sizeof(struct input_event))<0) {
        perror("report key error!\n");
        return -1;
    }
    return 0;
}

int ccKeyBoard::send(uint16_t type, int32_t keycode, int32_t value) {
    int result = 0;
    if(type==EV_REL){
        result += send_unit(EV_REL,REL_X,keycode);
        result += send_unit(EV_REL,REL_Y,value);
        result += send_unit(EV_SYN,SYN_REPORT,0);
    }else if(type==EV_KEY){
        result += send_unit(EV_KEY,keycode,value);
        result += send_unit(EV_SYN,SYN_REPORT,0);
    } else return -1;
    return result==0? 0:-1;
}