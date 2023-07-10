#include "backend.cpp"
/*
Contains:
RTSP server
RTP server 
RTCP server
*/


int main(){
    if (debug)
        printf("server started\n");

    // set up RTSP coonection
    thread RTSP_runner(RTSP_Server);
    
    //wait for client connection
    while (!connected)
        usleep(1000000);
    
    if (debug)
        printf("client connected\n");

    //start sending RTP packets
    thread RTP_runner(RTP_Server);
    //run RTCP 
    usleep(100000);
    thread RTCP_runner(RTCP_Server);


    //clean up
    RTSP_runner.join();
    RTP_runner.join();
    RTCP_runner.join();

    
    if (debug)
        printf("server ended");
}