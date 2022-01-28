/*
CPSC 441: Assignment 2 Kathryn Lepine 

This client program is the intraction based between the user and the 3 microservices provided. Client connects to indirection.c
with a TCP conncetion. There is 3 choices for mircoservices of translation, voting, and currency. 
To compile gcc -o client client.c 
To run ./client 

Code strongly references TA Amirhossien Sefati tutorial examples of exampleClient.c and exampleServer.C
Code references C How to Program (Seventh Edition) - Paul Deitel and Havey Deitel 
*/
//import esstential libariries 
#include <stdio.h>	
#include <string.h>	
#include <sys/socket.h>	
#include <arpa/inet.h>	
#include <unistd.h>
#include <stdlib.h>

//define IPs and ports
#define SERVER_IP "136.159.5.27"  /* csx3.cpsc.ucalgary.ca */
#define SERVER_PORT 8638

int main(int argc , char *argv[]){
    //initialization for socket
	int sock;
    //server socket address inistialization 
	struct sockaddr_in server;
	//declaring message variables
	char msgout[500], msgin[500];
    //initailize int for user to choose microservice
    int choice;

	//Create socket with TCP
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1){
		printf("could not create socket");
	}
	puts("socket created");
	
    //giving address of the indirection server 
	server.sin_addr.s_addr = inet_addr(SERVER_IP);
	//IPv4 protocol 
	server.sin_family = AF_INET;
	//port number
	server.sin_port = htons(SERVER_PORT);

	//Connect to server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
		printf("connect failed. Error");
		return 1;
	}
	puts("connected\n");
	
	//main while loop to continue interaction with indirection 
    //and choose which microservice to choose 
	while(1){
    //goto place for when voting UCP connection is disconnected     
    jump:
        //print intro to client 
        printf("\nHello\nWhich microserver would you like to use?\nTranslator: 1\nCurrency: 2\nVoting: 3\nExit Service: 0\nQuit: 4\n");
        //get choice of mircoserver from client 
        scanf("%d", &choice);
        //initializing buffer
        char buff[20];
        //converting a int to string
        sprintf(buff, "%d", choice);
        //if client chooses currency or voting microservice 
        if((choice == 1) || (choice == 2)){
            //send choice to indirections
            if(send(sock , buff, strlen(buff), 0) < 0){
                puts("Send failed");
                return 1;
		    }
            printf("sent: %s\n", buff);
            //initialize message
            char message[500];
            //clearing message 
            memset(msgin, 0, sizeof(msgin));  
            //get a message from indirection 
            if((recv(sock, msgin, sizeof(msgin), 0)) < 0){
                printf("Message received failed");
            }
            printf("reply from server: %s\n", msgin);
            //while loop to continue using miroservice without selecting from main menu again
            while(1){
                //clearing all message chars before use
                memset(msgout, '\0', sizeof(msgout));
                char message[500];
                memset(message, '\0', sizeof(message));
                //getting input from client 
                //using fgets so we get blank spaces as well 
                fgets(message, 30, stdin);        
                //send clients message to indirection
                if(send(sock , message, strlen(message), 0) < 0){
                    puts("Send failed");
                    return 1;
                }
                printf("Sent to server: %s\n", message);
                //clear message before use
                memset(msgin, 0, sizeof(msgin)); 
                //if the message is 0, then return to main menu
                if(strncmp(message, "0", 1) == 0){
                    (recv(sock, msgin, sizeof(msgin), 0));
                    printf("Reply from server: %s\n", msgin);
                    puts("Going back to home page\n");
                    break;
                }
                //clear message before use
                memset(msgin, 0, sizeof(msgin));  
                //receive message from indirection - this will be also message from microservice
                if((recv(sock, msgin, sizeof(msgin), 0)) < 0){
                    printf("Message received failed");
                }
                printf("Reply from sever: %s\n", msgin);
            }
        }
        //client choose voting microservice 
        else if(choice == 3){
            puts("You choose voting");
            //send choice of voting mircoservice to indirection
            if(send(sock , buff, strlen(buff), 0) < 0){
                puts("Send failed");
                return 1;
		    }
            printf("sent: %s\n", buff);
            //initializing and clearing 
            char message[500];
            memset(msgin, 0, sizeof(msgin));
            //getting message from indirection server  
            if((recv(sock, msgin, sizeof(msgin), 0)) < 0){
                printf("Message received failed");
            }
            printf("reply from server: %s\n", msgin);
            //main loop to interact with indirection without re-choosing microservice 
            while(1){
                //clearing variables before use 
                memset(msgout, '\0', sizeof(msgout));
                char message[500];
                //get input from user 
                scanf("%s", message);   
                //send input from user to indirection 
                if(send(sock , message, strlen(message), 0) < 0){
                    puts("Send failed");
                    return 1;
                }
                printf("Sent to server: %s\n", message);
                //clearing varaible before use
                memset(msgin, 0, sizeof(msgin));
                //if the user input is 0 return to the main menu 
                if(strncmp(message, "0", 1) == 0){
                    (recv(sock, msgin, sizeof(msgin), 0));
                    printf("Reply from server: %s\n", msgin);
                    puts("Going back to home page\n");
                    break;
                }
                //if user input is B, then the user would like to vote
                if(strncmp(message, "B", 1) == 0){
                    //get message from indirection, contain candidates and key to vote
                    if((recv(sock, msgin, sizeof(msgin), 0)) < 0){
                        printf("Message received failed");
                    }
                    printf("Reply from sever: %s\n", msgin);
                    //if Sorry is in message, then indirection lost connectino with voting 
                    //taken back to main microservice menu
                    if(strncmp(msgin, "Sorry", 5) == 0){
                        goto jump;
                    }
                    //get the string length of the message from indirection
                    int tem = strlen(msgin);
                    //get the last char in the message (this is where the key is stored)
                    char k = msgin[tem - 1];
                    //tell user their key for clarity of system performance 
                    printf("key is: %c\n", k);
                    //remove the key from message before printing the main message to client
                    msgin[tem-1]='\0';
                    printf("Reply from sever: %s\n", msgin);
                    printf("key is: %c\n", k);
                    //turn single char key into an int
                    int voteint = k - '0';
                    //initialize variable to vote
                    int id;
                    //get voting choice from client
                    scanf("%i", &id);
                    //ecrypt the clients vote with the key
                    int cryp = voteint * id;
                    //print encrypt vote to client for performance clarity 
                    printf("cryp: %i\n", cryp);
                    //clear message before use
                    memset(message, 0, sizeof(message));
                    //convert encrypted vote into string char
                    sprintf(message, "%i", cryp);
                    //send vote to indirection
                    if(send(sock , message, strlen(message), 0) < 0){
                        puts("Send failed");
                        return 1;
                    }
                    //clear msgin in before use
                    memset(msgin, 0, sizeof(msgin)); 
                    //get confirmation of voting from indirection
                    if((recv(sock, msgin, sizeof(msgin), 0)) < 0){
                        printf("Message received failed");
                    }
                    printf("Reply from sever: %s\n", msgin);
                    //check again if the UDP microservice failed to response
                    //if so goto the main menu
                    if(strncmp(msgin, "Sorry", 5) == 0){
                        goto jump;
                    }
                }else{
                    //if client does not vote 
                    //simply get the message from indirection
                    //clear msgin before use
                    memset(msgin, 0, sizeof(msgin)); 
                    //get message from indirection
                    if((recv(sock, msgin, sizeof(msgin), 0)) < 0){
                        printf("Message received failed");
                    }
                    printf("Reply from sever: %s\n", msgin);
                    //check again if the UDP microservice failed to response
                    //if so goto the main menu
                    if(strncmp(msgin, "Sorry", 5) == 0){
                        goto jump;
                    }
                }
            }
        }else if(choice == 4){
            //if the client choice is 4 and break the main while loop
            send(sock , "x", 1, 0);
            break; 
        }else{
            puts("invalid section, try again");
        }
	}
    //close the socket
	close(sock);
	return 0;
}