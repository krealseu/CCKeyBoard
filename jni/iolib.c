//
// Created by lthee on 2016/3/2.
//

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


ssize_t my_read(int fd, void *buffer, size_t length){
    ssize_t done = length;
    while(done>0){
        done = read(fd , buffer, length);
        if(done <=0 )
            if(errno==EINTR)
                done =length;
            else{
                return -1;
            }
        else
            break;
    }
    return done;
};
