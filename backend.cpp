#include "header.cpp"

//RTCP logging variables
server_globals server_global;

//video player controls
bool pause_video = false;
int method = 2;
bool connected = false;



void RTP_Server(){
    if (debug)
        printf("\nRTP started\n");


    // create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    inet_pton(AF_INET, CLIENT_IP,&cliaddr.sin_addr);
    cliaddr.sin_port = htons(server_global.port);


    //prepare to send images
    ifstream image;
    //ofstream image;
    RTP_packet rtp_packet;
    int index = 1 , bytes;
    char image_name[40];

    //choose a random number to start the sequence with
    rtp_packet.sequence_number = rand();


    // send images 1-500
    while (index <= 500){
        //increment the sequence number, save info of last packet
        rtp_packet.sequence_number++;
        server_global.sequence = rtp_packet.sequence_number;

        //save current time in the TS field and save info of last packet
        rtp_packet.TS = time(0);
        server_global.TS = rtp_packet.TS;

        //choose image depending on index
        sprintf(image_name,"server_video/image%03d.jpg",index);

        //open image and save it on the packet's payload
        bzero(rtp_packet.payload,sizeof(rtp_packet.payload));
        image.open(image_name, ios::binary);
        image.read(rtp_packet.payload,sizeof(rtp_packet.payload));

        //wait if the user paused the video
        while (pause_video) 
            usleep(2500000);

        //end the transmission if the method is 4
        if(method == 4) {
            sendto(sockfd, &rtp_packet, sizeof(rtp_packet), 0, 
            (struct sockaddr *) &cliaddr,sizeof(cliaddr));
            break;
        }

        //send to the client, save the number of bytes sent
        bytes =  sendto(sockfd, &rtp_packet, sizeof(rtp_packet), 0,
         (struct sockaddr *) &cliaddr,sizeof(cliaddr));

        //increment the total number of packets and sent bytes
        server_global.packets++;
        server_global.octets+=bytes;
        usleep(25000);
        image.close();
        index++;
    }


    //clean up
    recvfrom(sockfd,&rtp_packet,sizeof(rtp_packet),0,0,0);
    method = 4;
    close(sockfd);
    if (debug)
        printf("RTP ended\n");
}



void RTCP_Server(){
    if (debug)
        printf("\nRTCP started\n");


    // create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in cliaddr;
    int RTCP_port = server_global.port + 1;
    cliaddr.sin_family = AF_INET;
    inet_pton(AF_INET, CLIENT_IP,&cliaddr.sin_addr);
    cliaddr.sin_port = htons(RTCP_port);


    // choose a random SRC
    int SRC = rand();

    while (method != 4){
        // receive from stream if not in stop state
        RTCP_packet rtcp_packet;
        //fill packet info
        rtcp_packet.packet_type = 200;
        rtcp_packet.SSRC = SRC;
        rtcp_packet.NTP_TS = time(0);
        rtcp_packet.RTP_TS = server_global.TS;
        rtcp_packet.fraction = 0;
        rtcp_packet.packet_count = server_global.packets;
        rtcp_packet.octet_count = server_global.octets;
        rtcp_packet.longest_sequence = server_global.sequence;
        rtcp_packet.last_SR_TS = server_global.last_SR;
        rtcp_packet.delay_last_SR = time(0) - server_global.last_SR;
        usleep(1000000);

        //send packet
        sendto(sockfd, &rtcp_packet, sizeof(rtcp_packet), 
        0, (struct sockaddr *) &cliaddr,sizeof(cliaddr));
    }
    //clean up
    close(sockfd);
    if (debug)
        printf("RTCP ended\n");
}





void RTSP_Server(){
    if (debug)
        printf("\nRTSP started\n");


    // create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in cliaddr,servaddr;
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP,&servaddr.sin_addr);
    servaddr.sin_port = htons(atoi(PORT_SERVER));
    int size = sizeof(cliaddr);
    //bind socket to listen on 
    if (bind(sockfd, (sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("Binding of RTSP failed. \n");
        return;
    }

    //prepare RTSP packets
    RTSP_server_packet rtsp_server_packet;
    RTSP_client_packet rtsp_client_packet;
    char url[] = "rtsp://multi_ass";

    //wait for the user's connection
    recvfrom(sockfd, &rtsp_client_packet, sizeof(rtsp_client_packet),
    MSG_WAITALL,(struct sockaddr *) &cliaddr,(socklen_t *)&size);

    //compare the client's data with the expected results, if it passes toggle connected flag and save the client's port
    if(strcmp(url,rtsp_client_packet.url)) 
        rtsp_server_packet.status_code = 200;
        connected = true;
        server_global.port = rtsp_client_packet.port;


    sendto(sockfd, &rtsp_server_packet, sizeof(rtsp_server_packet),
     0, (struct sockaddr *) &cliaddr,size);


    while (method != 4) {
        //change the method depending on the reveived user input
        pause_video = false;
        if (strcmp("play" , rtsp_client_packet.method) == 0) 
            method = 1;
        
        if (strcmp("pause" , rtsp_client_packet.method) == 0) {
            method = 2;
            pause_video = true;
        }
        if (strcmp("replay" , rtsp_client_packet.method) == 0) 
            method = 3;
        
        if (strcmp("stop" , rtsp_client_packet.method) == 0) 
            method = 4;
        sendto(sockfd, &rtsp_server_packet, sizeof(rtsp_server_packet), 
        0, (struct sockaddr *) &cliaddr,size);
    }

    //clean up
    close(sockfd);
    if (debug)
        printf("RTSP ended\n");

}
