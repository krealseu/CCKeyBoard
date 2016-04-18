#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <string.h>
#include <fcntl.h>
#include <android/log.h>
//#include "iolib.h"
#include "ccKeyBoard.h"


#define iflog true
#define TAG "keyboard"
#define LOGV(...) if(iflog) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)

#define FILENAME "CCKEYBOARD"

int sfd, cfd;

void socket_make_sockaddr_un(const char *name, struct sockaddr_un *p_addr, socklen_t *alen) {
    memset(p_addr, 0, sizeof(*p_addr));
    size_t namelen;

    namelen = strlen(name);
    if ((namelen + 1) > sizeof(p_addr->sun_path)) {
        exit(-1);
    }
    p_addr->sun_path[0] = 0;
    memcpy(p_addr->sun_path + 1, name, namelen);
    p_addr->sun_family = AF_LOCAL;
    *alen = namelen + offsetof(struct sockaddr_un, sun_path) + 1;
}

void do_key(int client){
    write(client,"OK", sizeof("OK"));
    ccKeyBoard board;
    board.setup_uinput_device(0);
    char str[1024]={0};
    int s=0;
    int jixin = 1;
    int num[3];
    char *pos;
    int lastbyte;
    int len;
    while (1){
        if((lastbyte = read(client,str,1024))<=0){
            LOGV("disconnect");
            board.close_uinput_device();
            break;
        }
        str[lastbyte] = '\0';
        len = strlen(str);
        pos=str;
        for(int i=0;i<len;i++,pos++){
            if( *pos == '(') {
                s = 0;
                jixin = 1;
                bzero(num,3* sizeof(int));
            }
            else if(*pos == ')'){
                if(s!=2)
                    continue;
                num[s] = num[s] * jixin;
                s=0;
                //printf("%d  %d  %d\r\n",num[0],num[1],num[2]);
                board.send(num[0],num[1],num[2]);
            }
            else if(*pos==','){
                num[s] = num[s] * jixin;
                jixin = 1;
                s = ++s%3;
            }
            else if(*pos>=48 && *pos<=57){
                num[s]=num[s]*10 + *pos-48;
            }
            else if(*pos == '-'){
                jixin = -1;
            }
            else {
                continue;
            }
        }
    }
};
void signal_hander(int signo){
    close(cfd);
}
int main() {
    daemon(0,0);
    signal(SIGCHLD, SIG_IGN);
    if(signal(SIGUSR1,signal_hander) == SIG_ERR){
        perror("sigal fail");
        exit(1);
    }
 /*
    struct sockaddr_un addr;
    int addrLength;
    socket_make_sockaddr_un(FILENAME, &addr, &addrLength);
    unlink(FILENAME);
    if ((sfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0){
        LOGV("fail");
        return -1;
    }
 */
    unsigned short portnum=8088;
    struct sockaddr_in addr;
    int addrLength;
    bzero(&addr,sizeof(struct sockaddr_in));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(portnum);
    addrLength = sizeof(struct sockaddr);


    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        LOGV("fail");
        return -1;
    }
    if (bind(sfd, (struct sockaddr *) &addr,addrLength)) {
        //perror("Bind Fail");
        LOGV("Bind Fail");
        return -1;
    }
    if (listen(sfd, 5)) {
        LOGV("Listen Fail");
        return -1;
    }
    while(1){
        LOGV("wait a connect");
        cfd = accept(sfd, (struct sockaddr *) &addr, &addrLength);
        if(cfd == -1)
            continue;
        pid_t fpid;
        if((fpid=fork())<0)
            perror("fork erro");
        else if(fpid==0){
            close(sfd);
            char cmd[1024]={0};
            int len=read(cfd,cmd,1024);
            cmd[len]='\0';
            if(strstr(cmd,"KEY")==cmd)
                do_key(cfd);
            else if(strstr(cmd,"MOUSE")==cmd)
                do_key(cfd);
            else if(strstr(cmd,"EXIT")==cmd)
                kill(0,SIGUSR1);
            close(cfd);
            return 1;
        } else {
            close(cfd);
        }
    }
    return 0;
}



