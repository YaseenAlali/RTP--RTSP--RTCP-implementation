//IO
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>
//Networking
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
//system
#include <thread>
#include <time.h>
#include <syscall.h>

#include "CImg.h"
using namespace std;
using namespace cimg_library;

//client and server set IPs and ports
char const *SERVER_IP = "127.0.10.1";
char const *CLIENT_IP = "127.0.11.1";

char const *PORT_CLIENT = "10000";
char const *PORT_CLIENT_RTCP = "10001";
char const *PORT_SERVER = "20000";


//used to display informative runtime information, can be toggled off
bool debug = true; 





/*RTP packet
    version : the version of RTP
    sequence number: the sequence number of the packet
    TS: the time stamp of which the packet was sent it
    SSRC: the synching sources of the packet
    CCRC: contributing sources of the packet
    payload: the data on the packet
*/
struct RTP_packet
{
    int version = 2;
    short sequence_number;
    int TS;
    int SSRC = 0;
    int CSRC = 0;
    char payload[10000];
};

/* RTCP Packet
    version: RTCP's version
    report_count: the total number of reports sent
    packet_type: the type of the packet
    length: the length of the packet
    SSRC: synching source ID(originator)
    NTP_TS: NTP time stamp
    RTP_TS: RTP time stamp
    packet_count: the total number of packets sent
    octet_count: the total number of octets sent
    SSRC_n:synching source ID
    fraction: The fraction of RTP data packets from source SSRC_n
    lost_packets: the total number of packets lost
    longest_sequence:the highest sequence number received in an RTP data packet from source SSRC_n
    jitter: An estimate of the statistical variance of the RTP data packet
    last_SR_TS: the last sent report's time stamp
    delay_last_SR: the delay of the last sent report
*/
struct RTCP_packet
{
    int version = 2;
    int report_count;
    int packet_type;
    int length;
    int SSRC;
    int NTP_TS;
    int RTP_TS;
    int packet_count;
    int octet_count;
    int SSRC_n;
    int fraction;
    int lost_packets;
    int longest_sequence;
    int jitter;
    int last_SR_TS;
    int delay_last_SR;
};


/*server globals
used by RTCP to keep track of the last packet
*/
struct server_globals{
    int packets = 0;
    int octets = 0;
    int sequence = 0;
    int last_SR = 0;
    int TS = 0;
    int port = 0;
};



/*RTSP request packet
    method: the request's method, i.e. play, pause, etc
    port: the port used by the server
    url: the URL of the request
    version: RTSP's version
    cseq: sequence number for RTSP request/response pair
    session: the ID of the session
*/
struct RTSP_client_packet
{
    char method[50];
    int port;
    char url[50] = "rtsp://multi_ass";
    int version = 2;
    unsigned int cseq = 1;
    unsigned int session;
};


/* RTSP response packet
    version: RTSP's version
    cseq: sequence number for RTSP request/response pair
    session: the ID of the session
*/
struct RTSP_server_packet
{
    int version = 2;
    unsigned int status_code;
    unsigned int cseq;
    unsigned int session;
};
