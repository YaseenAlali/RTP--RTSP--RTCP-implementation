Low level C++ implementation of popular real time streaming protocols, Runs on linux 

1 - Compilation and running:

    make sure that the three scripts: compile.sh, clean.sh and run_both are given execute permissions
    if one of them doesn't work use ./X.sh instead of bash X.sh  

    Compile:
        use ./compile.sh

        will create binaries server and client that can be run with ./server ./client

    Remove binaries:
        use ./clean.sh

        will remove server and client binaries if existing

    Compile and run the server and client:
        use ./run_both 

        calls clean.sh to remove old binaries and compile.sh to create new binaries

        opens a new terminal window that runs the server and another that runs the client 
        
        ignore the deprecated option warning



2 - Runtime:

    2.1 - Controls:
            1 - play the video, on by default

            2 - pause the video

            3 - restart the video

            4 - end the video and stream

    2.2 - Behavior:
        the video plays by default once the client connects to the server
        
        pressing restart also pauses the video, so you will need to press restart(3) then play(1) 

        RTCP will write its data on RTCP_client_log.txt every one second



3 - Extras:

        header.cpp :: debug can be turned off to stop the client and server from displaying messages on the standard output

        the server uses the IP 127.0.10.1 and ports 20000/20001
        
        the client uses the IP 127.0.11.1 and ports 10000/10001
