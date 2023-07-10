#include "header.cpp"
//contains packet headers, libs, global variables and namespaces

// video player control
bool pause_video = false;
bool RTP_send = false;
int method = 2;


// opens the image located in client_video and displays them 
void display_video()
{
    if (debug)
        printf("display_video start\n");

    // Create CImage object
    CImgDisplay ImageDisplay;

    // display menu
    printf("Menu\n"
           "press 1 to play/resume the video\n"
           "press 2 to pause the video\n"
           "press 3 to restart the video\n"
           "press 4 to exit the video");

    while (pause_video)
    {
        usleep(2500000);
        //code might block
        if (method == 4)
            break;
    }
    // start playing the video,images are named imageX where X is a 3 digit number from 1-500
    int index = 1;
    char image_name[40];

    // 500 images
    while (index <= 500)
    {
        // if the video is paused, Wait until user resumes
        while (pause_video)
            usleep(2500000);
        // if user entered the exit command
        if (method == 4)
            break;
        // if user entered the restart command
        if (method == 3)
        {
            // go back to the first image
            index = 1;
            continue;
        }
        // change the name of the image to display depending on the index
        sprintf(image_name, "client_video/image%03d.jpg", index);

        // display image
        CImg<unsigned char> Image(image_name);
        ImageDisplay = Image;
        usleep(30000);

        // if the final frame is reached, await user command of restarting or exiting
        while (index == 500)
        {
            printf("Video ended.. you can replay or exit\n");
            if (method == 4)
                break;
            if (method == 3)
            {
                index = 1;
                continue;
            }
            usleep(5000000);
        }
        index++;
    }
    if (debug)
        printf("display_video end\n");
}




void RTP_Client()
{
    if (debug)
        printf("\nRTP started\n");


    // create UDP packet
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in cliaddr, servaddr;
    cliaddr.sin_family = AF_INET;
    inet_pton(AF_INET, CLIENT_IP,&cliaddr.sin_addr);
    cliaddr.sin_port = htons(atoi(PORT_CLIENT));

    // bind the socket and listen
    if (bind(sockfd, (sockaddr *)&cliaddr, sizeof(cliaddr)) < 0)
    {
        printf("Binding of RTP failed. \n");
        return;
    }

    // prepare to receieve images
    char image_name[40];
    ofstream image;
    int size = sizeof(servaddr), index = 1, bytes = 0;
    RTP_packet rtp_packet;
    thread video_player(display_video);

    // images from 1 to 500
    while (index <= 500)
    {
        // if the user existed the video
        if (method == 4)
            break;
        // format the image's name and open a pointer to the image
        sprintf(image_name, "client_video/image%03d.jpg", index);
        image.open(image_name, ios::binary);

        //receieve the image's data
        bzero(rtp_packet.payload, sizeof(rtp_packet.payload));
        bytes = recvfrom(sockfd, &rtp_packet, sizeof(rtp_packet),
                         MSG_WAITALL, (struct sockaddr *)&servaddr, (socklen_t *)&size);


        // build the image
        image.write(rtp_packet.payload, sizeof(rtp_packet.payload));
        image.close();
        index++;
    }

    //wait for video player to finish
    video_player.join();


    //cleaning up       
    method = 4;
    sendto(sockfd, &rtp_packet, sizeof(rtp_packet), 0,
           (struct sockaddr *)&servaddr, size);

    close(sockfd);
    if (debug)
        printf("RTP finished\n");
}





void RTCP_Client()
{
    if (debug)
        printf("\nRTCP started, will write on RTCP_client_log.txt\n");

    // create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    inet_pton(AF_INET, CLIENT_IP,&cliaddr.sin_addr);
    cliaddr.sin_port = htons(atoi(PORT_CLIENT_RTCP));
    // bind a socket to listen on
    if (bind(sockfd, (sockaddr *)&cliaddr, sizeof(cliaddr)) < 0)
    {
        printf("Binding of RTCP failed. \n");
        return;
    }

    // prepar RTCP packet to receive from server
    RTCP_packet rtcp_packet;

    // save the data on RTCP_client_log.txt
    ofstream RTCP_log;
    RTCP_log.open("RTCP_client_log.txt", ios::out);
    char RTCP_message[1024];

    while (method != 4)
    {
        // receive from stream if not in stop state
        recvfrom(sockfd, &rtcp_packet, sizeof(rtcp_packet), MSG_WAITALL, 0, 0);

        // format the RTCP message
        sprintf(RTCP_message,
                "report count:%d\npacket type:%d\n"
                "packet length:%d\nSSRC:%d\nNTP time stamp:%d\n"
                "RTP time stamp:%d\ntotal packet count:%d\n"
                "octet count:%d\nsource ID:%d\nfraction lost:%d\n"
                "total packets lost:%d\njitter:%d\n"
                "extended highest sequence number received:%d\n"
                "last SR timestamp:%d\ndelay since last SR:%d\n\n\n",
                rtcp_packet.report_count, rtcp_packet.packet_type, rtcp_packet.length,
                rtcp_packet.SSRC, rtcp_packet.NTP_TS, rtcp_packet.RTP_TS, rtcp_packet.packet_count,
                rtcp_packet.octet_count, rtcp_packet.SSRC_n, rtcp_packet.fraction,
                rtcp_packet.lost_packets, rtcp_packet.jitter, rtcp_packet.longest_sequence,
                rtcp_packet.last_SR_TS, rtcp_packet.delay_last_SR);


        // write on RTCP_log every one second
        RTCP_log << RTCP_message;
        usleep(1000000);
    }
    //cleaning up
    close(sockfd);
    if (debug)
        printf("RTCP ended\n");
}





void RTSP_Client()
{
    if (debug)
        printf("\nRTSP started\n");


    // craete UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP,&servaddr.sin_addr);;
    servaddr.sin_port = htons(atoi(PORT_SERVER));
    int size = sizeof(servaddr), bytes;


    //prepare RTSP packets
    RTSP_client_packet rtsp_client_packet;
    RTSP_server_packet rtsp_server_packet;

    // setup the connection between the server and client
    rtsp_client_packet.port = atoi(PORT_CLIENT);
    strcpy(rtsp_client_packet.method, "setup");
    sendto(sockfd, &rtsp_client_packet, sizeof(rtsp_client_packet), 0,
           (struct sockaddr *)&servaddr, size);

    // receive response
    bytes = recvfrom(sockfd, &rtsp_server_packet, sizeof(rtsp_server_packet),
                     MSG_WAITALL, (struct sockaddr *)&servaddr, (socklen_t *)&size);

    RTP_send = true;
    while (method != 4)
    {
        // take user input and send RTSP packets accordingly
        scanf("%d", &method);
        switch (method)
        {
        case 1:
            // play 
            pause_video = false;
            strcpy(rtsp_client_packet.method, "play");
            break;
        case 2:
            // pause 
            pause_video = true;
            strcpy(rtsp_client_packet.method, "pause");
            break;
        case 3:
            // replay 
            pause_video = false;
            strcpy(rtsp_client_packet.method, "replay");
            break;
        case 4:
            // stop 
            pause_video = false;
            strcpy(rtsp_client_packet.method, "stop");
            break;
        default:
            // only accespt the correct input from 1-4
            printf("invalid input.\n");
        }
        // send RTSP packets
        sendto(sockfd, &rtsp_client_packet, sizeof(rtsp_client_packet), 0,
               (struct sockaddr *)&servaddr, size);
    }
    //cleaning up
    close(sockfd);
    if (debug)
        printf("RTSP ended\n");
}
