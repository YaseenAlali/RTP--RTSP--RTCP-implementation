#include "frontend.cpp"
/*
Contains:
RTSP client
RTP client
RTCP client
*/


int main() {
    if (debug)
        printf("client started\n");

    //remove client_video directory if it exists, create a new one
    system("rm -f -r client_video");
    system("mkdir client_video");

    //set up RTSP connection
    thread RTSP_runner(RTSP_Client);

    while (!RTP_send) 
        usleep(1000000);

    //start receving RTP packets from server
    thread RTP_runner(RTP_Client);
    usleep(100000);
    //run RTCP client after 0.1 second to ensure the menu displays properly, write data on RTCP_log.txt
    thread RTCP_runner(RTCP_Client);

    //clean up
    RTSP_runner.join();
    RTP_runner.join();
    RTCP_runner.join();
    system("rm -f -r client_video");


    if (debug)
        printf("client ended");
}