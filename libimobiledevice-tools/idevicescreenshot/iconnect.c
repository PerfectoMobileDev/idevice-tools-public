#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>


#include "iconnect.h"


int sendingData(char* buffer, uint64_t size,SOCKET msgsock)
{
	int retval = 0;

    retval = send(msgsock, buffer, size, 0);
	 
    if (retval == SOCKET_ERROR)
    {
        fprintf(stderr,"Server: send() failed: error %d\n", WSAGetLastError());
    }
    else
        printf("Server: send() is OK.\n");

	return retval;
}

int MyFuckenServer(int Port,screenshotr_client_t shotr)
{
	
    char *ip_address;
    unsigned short port;
    int socket_type;
	
	int retval;
    int fromlen;
    struct sockaddr_in local;
	struct sockaddr_in from;
    //WSADATA wsaData;
    SOCKET listen_socket;
	SOCKET msgsock;


	char Buffer[128] = {0};

	char *imgdata = NULL;
	char* byteBuffer = NULL;

	uint64_t imgsize = 0;

    ip_address = NULL;
    port = Port;
    socket_type = SOCK_STREAM;

	byteBuffer =  (char*)malloc(15*1000*1024); //15 MB

 
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = (!ip_address) ? INADDR_ANY:inet_addr(ip_address);
 

    local.sin_port = htons(port);

    listen_socket = socket(AF_INET, socket_type,0);
 
    if (listen_socket == INVALID_SOCKET){
        fprintf(stderr,"Server: socket() failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    else
       printf("Server: socket() is OK.\n");
 

    if (bind(listen_socket, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
       {
        fprintf(stderr,"Server: bind() failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    else
              printf("Server: bind() is OK.\n");
 

    if (socket_type != SOCK_DGRAM)
       {
        if (listen(listen_socket,5) == SOCKET_ERROR)
              {
            fprintf(stderr,"Server: listen() failed with error %d\n", WSAGetLastError());
            WSACleanup();
            return -1;
        }
       else
              printf("Server: listen() is OK.\n");
    }

 
	       fromlen =sizeof(from);

        if (socket_type != SOCK_DGRAM)
              {
            msgsock = accept(listen_socket, (struct sockaddr*)&from, &fromlen);
            if (msgsock == INVALID_SOCKET)
           {
                fprintf(stderr,"Server: accept() error %d\n", WSAGetLastError());
                WSACleanup();
                return -1;
            }
           else
              printf("Server: accept() is OK.\n");
              printf("Server: accepted connection from %s, port %d\n", inet_ntoa(from.sin_addr), htons(from.sin_port)) ;
           
        }
        else
            msgsock = listen_socket;


       while(1)
       {
 
        if (socket_type != SOCK_DGRAM)
            retval = recv(msgsock, Buffer, sizeof(Buffer), 0);
 
       else
       {
            retval = recvfrom(msgsock,Buffer, sizeof(Buffer), 0, (struct sockaddr *)&from, &fromlen);
            printf("Server: Received datagram from %s\n", inet_ntoa(from.sin_addr));
        }
           
        if (retval == SOCKET_ERROR)
        {
			memset(Buffer,0,sizeof(Buffer));
            //fprintf(stderr,"Server: recv() failed: error %d\n", WSAGetLastError());
            //closesocket(msgsock);
            continue;
        }
       else
            printf("Server: recv() is OK.\n");
 
        if (retval == 0)
              {
            printf("Server: Client closed connection.\n");
            closesocket(msgsock);
            continue;
        }

        printf("Server: Received %d bytes, data \"%s\" from client\n", retval, Buffer);
		if (strcmp(Buffer,"SCREENSHOT:") == 0)
		{
			uint32_t netValue = 0;
			uint32_t netDataValue = 0;
			

			
			netValue = htonl((uint32_t)0);
		
			 printf("Server: SCREENSHOTING !!!\n");
			 get_screenshot_data(shotr,&imgdata,&imgsize);

			 netDataValue = htonl((uint32_t)imgsize);

			 /*
			 if (byteBuffer == NULL)
				byteBuffer =  (char*)malloc(imgsize + 2*sizeof(uint32_t));
				*/

			 memcpy(&byteBuffer[0],&netValue,sizeof(uint32_t));
			 memcpy(&byteBuffer[4],&netDataValue,sizeof(uint32_t));
			 memcpy(&byteBuffer[8],imgdata,imgsize);

			 sendingData(byteBuffer,imgsize + 2*sizeof(uint32_t),msgsock);

			 free(imgdata);
			 //free(byteBuffer);
			 
		}
		else
		{
			 printf("Server: Echoing the same data back to client...\n");
			 sendingData(Buffer,retval,msgsock);
		}

		
 
		

 
       if (socket_type != SOCK_DGRAM)
       {
            printf("Server: I'm waiting more connection, try running the client\n");
            printf("Server: program from the same computer or other computer...\n");
            //closesocket(msgsock);
        }
        else
            printf("Server: UDP server looping back for more requests\n");
        continue;
    }
      
   free(byteBuffer);

	return 0;
}
