//CPSC 441: Assignment 1
//Kathryn Lepine 30044629
//This program is a simple HTTP proxy using sockets. Run program with dynamic.c and 
//the program allows you to dynamically block and unblock specific keywords.

//References 
//Code heavily references tutorial material of TA Amirhossein Sefati
//Code references Carey Williamson's sample client and server Assignment 0 code
//Code references C How to Program (Seventh Edition) - Paul Deitel and Havey Deitel 


//importing libararies 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

//initializing variables for sockets 
int server_socket, client_socket, accept_socket;

//will close sockets when program ends if needed 
void catcher(int sig){
    close(server_socket);
    close(client_socket);
    close(accept_socket);
    exit(0);
}

int main(int argc, char *argv[]){
    //initializing variables 
    //0 means word is not in URL, 1 means in URL 
    int flop = 0;
    int spon = 0;
    int curl = 0; 
    //0 means word is not in message body, 1 means in message body
    int msgflop = 0;
    int msgspon = 0;
    int msgcurl = 0; 
    //0 means word is unblocked, 1 means word is blocked (IN URL)
    int floppyb = 0; 
    int curlingb = 0;
    int spongeb = 0;
    //0 means word is unblocked, 1 means word is blocked (IN MESSAGE BODY)
    int msgfloppyb = 0; 
    int msgcurlingb = 0;
    int msgspongeb = 0;
    //1 means jpg, 0 means html
    //assuming html unless it is a jpg
    int jpg = 0; 

    ////MAKING THE SERVER SOCKET////
    //server socket address inistialization 
    struct sockaddr_in address_server;
    //initializing address to zero
    memset(&address_server, 0, sizeof(address_server));
    //IPv4 protocol 
    address_server.sin_family = AF_INET;
    //port number 8638
    address_server.sin_port = htons(8638);
    //IP address from local machine 
    address_server.sin_addr.s_addr = INADDR_ANY;
    
    //create server socket 
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            printf("Could not create server socket\n");
            exit(1); 
    }
    
    //bind server socket 
    if(bind(server_socket, (struct sockaddr *)&address_server, sizeof(address_server)) < 0){
        printf("Server socket bind failed\n");
        exit(1);
    }
    printf("Server socket has been created and binded\n");

    ////MAIN WHILE LOOP THAT LISTENS, ACCEPTS CONNECTIONS, CREATES//// 
    ////CLIENT SOCKET, CENSORS DATA, AND RETURNS APPROPRIATE DATA////
    while(1){
        //server socket is listening 
        listen(server_socket, 5);
        printf("At the top of the loop, waiting for connections\n");
        //server socket accepts a connection 
        if((accept_socket = accept(server_socket, NULL, NULL)) < 0){
            printf("Accepting client server failed\n");
        }
        printf("Connection has been accepted\n");

        //initialize variables to stores and modify strings with 
        char sendmsg[1000000]; //used
        char revmsg[100000]; //used 
        char server_reply[100000];
        char msg[100000]; //used 
        char msg1[100000]; //used
        char newMsg[10000]; //used
    
        //server socket accepts a connection and stores mesage 
        if(recv(accept_socket, revmsg, sizeof(revmsg), 0) < 0){
            printf("Receving message failed\n");
        }

        //print message recieved 
        printf("HELP: %s\n", revmsg);

        ////DYNAMICALLY CHANGING BLOCK AND UNBLOCK STATUS////
        //if the string length of the message is less than 16 characters good probabaility it is from dynamic socket
        //parse the message to see what dynamic changes are desired to change 
        if(strlen(revmsg) < 20){
            //compare messages with all key blocking and unblocked words 
            //if strings match updated the variables appropriately 
            //0 means unblocked, 1 means blocked 
            char a1[] = "blockfloppy";
            if(strcmp(revmsg, a1) == 0){
                printf("floppy in URL been blocked");
                floppyb = 1; 
            }
            char a2[] = "blockspongebob";
            if(strcmp(revmsg, a2) == 0){
                printf("spongebob in URL been blocked");
                spongeb = 1;
            }
            char a3[] = "blockcurling";
            if(strcmp(revmsg, a3) == 0){
                printf("curling in URL been blocked");
                curlingb = 1;
            }
            char a4[] = "blockall";
            if(strcmp(revmsg, a4) == 0){
                printf("everything in URL been blocked");
                floppyb = 1; 
                curlingb = 1;
                spongeb = 1;
            }
            char a5[] = "freefloppy";
            if(strcmp(revmsg, a5) == 0){
                printf("floppy in URL been freed");
                floppyb = 0; 
            }
            char a6[] = "freespongebob";
            if(strcmp(revmsg, a6) == 0){
                printf("spongebob in URL been freed");
                spongeb = 0;
            }
            char a7[] = "freecurling";
            if(strcmp(revmsg, a7) == 0){
                printf("floppy in URL been freed");
                curlingb = 0;
            }
            char a8[] = "freeall";
            if(strcmp(revmsg, a8) == 0){
                printf("everything in URL been freed");
                floppyb = 0; 
                curlingb = 0;
                spongeb = 0;
            }
            char a9[] = "blockmsgall";
            if(strcmp(revmsg, a9) == 0){
                printf("all key words message been blocked");
                msgfloppyb = 1; 
                msgcurlingb = 1;
                msgspongeb = 1;
            }
            char a10[] = "freemsgall";
            if(strcmp(revmsg, a10) == 0){
                printf("all key words message been unblocked");
                msgfloppyb = 0; 
                msgcurlingb = 0;
                msgspongeb = 0;
            }
            else{
                printf("sorry, error in message");
            }

        ////MAKE CLIENT SOCKET, GET AND RETURN APPROPRIATE CONTENT TO ACCEPT SOCKET////
        }else{
            printf("this where you wanna be for website");
            //copy message to check URL and get host name 
            strcpy(msg, revmsg);
            strcpy(msg1, revmsg);
            
            //take everything before "//" away
            char *token1 = strtok(msg, "//");
            //take everything after "/" away leaving the host name 
            char *token2 = strtok(NULL, "/");
            printf("host name: %s\n", token2);

            //isolate just the first GET request line of message 
            //\r moves cursior to left side, \n moves to new line 
            char *url1 = strtok(msg1, "\r\n");
           
            printf("The get request with URL: %s\n", url1);

            ////CHECKING URL FOR KEY WORDS TO BE BLOCKED////
            //if key word is in URL change respective variable to 1
            ////check for Floppy in URL
            char floppy[] = "Floppy";
            if(strstr(url1, floppy) != NULL){
                printf("Floppy is in URL\n");
                flop = 1; 
            }else{
                printf("Floppy is not in URL\n");
            }
            //check for curling in URL
            char curling[] = "curling";
            if(strstr(url1, curling) != NULL){
                printf("curling is in URL\n");
                curl = 1; 
            }else{
                printf("curling is not in URL\n");
            }
            //check for SpongeBob in URL
            char sponge[] = "SpongeBob";
            if(strstr(url1, sponge) != NULL){
                printf("SpongeBob is in URL\n");
                spon = 1; 
            }else{
                printf("SpongeBob is not in URL\n");
            } 

            ////CHECK IF JPG OR HTML
            char jp[] = "jpg";
            if(strstr(url1, jp) != NULL){
                printf("This is a jpg\n");
                jpg = 1; 
            }else{
                printf("This is a html\n");
            } 

            ////MAKING THE CLIENT SOCKET 
            //client socket address initialization 
            struct sockaddr_in address_client;
            struct hostent *hosts;
            //using host name to get IP address 
            hosts = gethostbyname(token2);
            //clearing the address
            memset(&address_client, 0, sizeof(address_client));
            //IPv4 protocol 
            address_client.sin_family = AF_INET;
            //port 80
            address_client.sin_port = htons(80);
            bcopy((char *) hosts->h_addr, (char *)&address_client.sin_addr.s_addr, hosts->h_length);
            
            //creating the client socket 
            client_socket = socket(AF_INET, SOCK_STREAM, 0);
            if(client_socket == -1){
                printf("Could not create client socket\n");
            }
            
            //connecting the client socket 
            if(connect(client_socket, (struct sockaddr *)&address_client, sizeof(address_client)) < 0){
                printf("connection of client failed\n");
            }
            printf("Client socket created and connected\n");
           
            ////IF THE WEBPAGE SHOULD BE BLOCKED BASED ON URL////
            //if any key word is in the URL and should be blocked return the error page 
            if(((flop==1) && (floppyb==1)) || ((curl==1) && (curlingb==1)) || ((spon==1) && (spongeb==1))){
                printf("Blocking webpage based on URL\n");

                /*printf("flop: %d\n", flop);
                printf("floppyb: %d\n", floppyb);
                printf("curl: %d\n", curl);
                printf("curlingb: %d\n", curlingb);
                printf("spon: %d\n", spon);
                printf("spongeb: %d\n", spongeb);*/
               
                //hard code the error page to send back 
                //creating a valid GET request to the error page 
                char s1[] = "GET http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/error.html HTTP/1.1\r\n";
                char s2[] = "Host: pages.cpsc.ucalgary.ca\r\n\r\n";
                strcat(newMsg, s1); 
                strcat(newMsg, s2);
                printf("Sending GET request:\n%s\n", newMsg);

                //sending GET request of error page to client socket 
                if(send(client_socket, newMsg, strlen(newMsg), 0) < 0){
                        printf("Failed to send error page GET request to client socket\n");
                    }

                int status; 
                while(1){
                    //in a while loop incase packets come in seperate recieves
                    //know there are no more packets when status is 0
                    status = recv(client_socket, sendmsg, 1000000, 0);
                    if(status <= 0){
                        break;
                    }else{
                        //send only the number of packets that were recieved 
                        if(send(accept_socket, sendmsg, status, 0) < 0){
                            printf("Failed to send\n");
                        }
                        //clear the variable to get more packets later 
                        memset(sendmsg, '\0', 1000000);
                    }
                }
                printf("\nPage should be delivered now\n");
            }
            ////IF THE WEBPAGE SHOULD BE NOT BLOCKED BASED ON URL////
            //passes GET request to the client and retrives data
            //handles the case where webpage should be block based on inner message 
            else{
                printf("Not blocking the webpage based on URL\n");

                //sending GET request to client socket 
                if(send(client_socket, revmsg, strlen(revmsg), 0) < 0){
                    printf("Failed to send GET request to the client\n");
                }
                printf("Sending GET request: %s\n", revmsg);

                ///IF JPG SEND MESSAGE 
                if(jpg == 1){
                    printf("Sending a jpg\n");
                    int status; 
                    while(1){
                        //in a while loop incase packets come in seperate recieves
                        //know there are no more packets when status is 0
                        status = recv(client_socket, sendmsg, 1000000, 0);
                        if(status <= 0){
                            break;
                        }else{
                            //send only the number of packets that were recieved 
                            if(send(accept_socket, sendmsg, status, 0) < 0){
                                printf("Failed to send\n");
                            }
                            //clear the variable to get more packets later 
                            memset(sendmsg, '\0', 1000000);
                        }
                    }
                }
                else{
                    printf("This is html, check message for key words");

                    //receiving message from client socket 
                    
                    if((recv(client_socket, sendmsg, 1000000, 0)) < 0){
                        printf("Failed to receive\n");
                    }

                    //CHECKING KEY WORDS IN MESSAGE BODY TO BE BLOCKED 
                    //check for Floppy in message body
                    char floppy1[] = "Floppy";
                    if(strstr(sendmsg, floppy1) != NULL){
                        printf("Floppy is in message body\n");
                        msgflop = 1; 
                    }else{
                        printf("Floppy is not in message body\n");
                    }
                    //check for curling in message body
                    char curling1[] = "curling";

                    ///
                    printf("LOLL: %s", sendmsg);
                    ///

                    if(strstr(sendmsg, curling1) != NULL){
                        printf("curling is in message body\n");
                        msgcurl = 1; 
                    }else{
                        printf("curling is not in message body\n");
                    }
                    //check for SpongeBob in message body
                    char sponge1[] = "SpongeBob";
                    if(strstr(sendmsg, sponge1) != NULL){
                        printf("SpongeBob is in message body\n");
                        msgspon = 1; 
                    }else{
                        printf("SpongeBob is not in message body\n");
                    }
                    printf("msgcurl: %d", msgcurl);
                    printf("msgspon: %d", msgspon);
                    printf("msgflop: %d", msgflop);
                    printf("msgfloppyb: %d", msgfloppyb);
                    printf("msgcurlingb: %d", msgcurlingb);
                    printf("msgspongeb: %d", msgspongeb);
                    
                    
                    //if data needs to be blocked send to error page 
                    if(((msgflop==1) && (msgfloppyb==1)) || ((msgcurl==1) && (msgcurlingb==1)) || ((msgspon==1) && (msgspongeb==1))){
                        
                        ////MAKING THE CLIENT SOCKET 
                        //client socket address initialization 
                        struct sockaddr_in address_client;
                        struct hostent *hosts;
                        //using host name to get IP address 
                        hosts = gethostbyname(token2);
                        //clearing the address
                        memset(&address_client, 0, sizeof(address_client));
                        //IPv4 protocol 
                        address_client.sin_family = AF_INET;
                        //port 80
                        address_client.sin_port = htons(80);
                        bcopy((char *) hosts->h_addr, (char *)&address_client.sin_addr.s_addr, hosts->h_length);
                        
                        //creating the client socket 
                        client_socket = socket(AF_INET, SOCK_STREAM, 0);
                        if(client_socket == -1){
                            printf("Could not create client socket\n");
                        }
                        
                        //connecting the client socket 
                        if(connect(client_socket, (struct sockaddr *)&address_client, sizeof(address_client)) < 0){
                            printf("connection of client failed\n");
                        }
                        printf("Client socket created and connected\n");

                        printf("\nKey word blocked in message, returning error page\n");
                        //hard code the error page to send back 
                        //creating a valid GET request to the error page 
                        char s1[] = "GET http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/error.html HTTP/1.1\r\n";
                        char s2[] = "Host: pages.cpsc.ucalgary.ca\r\n\r\n";
                        strcat(newMsg, s1); 
                        strcat(newMsg, s2);
                        printf("Sending GET request:\n%s\n", newMsg);

                        //sending GET request of error page to client socket 
                        if(send(client_socket, newMsg, strlen(newMsg), 0) < 0){
                            printf("Failed to send error page GET request to client socket\n");
                        }

                        int status; 
                        while(1){
                            //in a while loop incase packets come in seperate recieves
                            //know there are no more packets when status is 0
                            printf("we are here");
                            status = recv(client_socket, sendmsg, 1000000, 0);
                            printf("status: %d", status);
                            if(status <= 0){
                                break;
                            }else{
                                printf("hhhh: %s", sendmsg);
                                //send only the number of packets that were recieved 
                                if(send(accept_socket, sendmsg, status, 0) < 0){
                                    printf("Failed to send\n");
                                }
                                //clear the variable to get more packets later 
                                memset(sendmsg, '\0', 1000000);
                            }
                        }
                        printf("\nPage should be delivered now\n");
                    }
                    //data does not need to be blocked send their GET request 
                    else{
                        //send back to accept socket 
                        if(send(accept_socket, sendmsg, strlen(sendmsg), 0) < 0){
                            printf("Failed to send\n");
                        }
                        printf("\nPage should be delivered now\n");
                    }
                }
            }
        }
        //setting variables that check if key word in URL to not in URL 
        flop = 0;
        spon = 0;
        curl = 0;
        //setting variables that check if key word in message to not in message 
        msgflop = 0;
        msgspon = 0;
        msgcurl = 0; 
        //setting default status back to html 
        jpg = 0;
        //close the accept socket and the client socket 
        close(accept_socket);
        close(client_socket);
    }
    //closing the server socket
    close(server_socket);
    return 0;
}