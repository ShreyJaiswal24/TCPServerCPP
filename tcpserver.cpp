#include <iostream> //For standard Input/Output.
#include <sys/types.h> //For data types used in socket function.
#include <unistd.h> //for close() function.
#include <sys/socket.h> //for socket functions like socket(), bind(), listen(), accept(),etc.
#include <netdb.h> //for getnameinfo().
#include <arpa/inet.h> //for inet_pton(), inet_ntop().
#include <string.h> //for memset
#include <string> //for C++ strings
 
using namespace std;
 
int main()
{
    // Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) //Checking for error.
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return -1;
    }
 
    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000); //Port number (54000), converted into network byte order.
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr); //Bind to any IP Address.
 
    bind(listening, (sockaddr*)&hint, sizeof(hint)); //Assigns the address and port to the socket.
 
    // Tells the OS that socket listening is ready to accept incoming connection requests
    listen(listening, SOMAXCONN); //SOMAXCONN is the maximum connections allowed.
 
    // Wait for a connection
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
 
    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize); //accepts waits for the client to connect and clientSocket is the new socket.
 
    char host[NI_MAXHOST];      // Client's remote name
    char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on
 
    memset(host, 0, NI_MAXHOST); 
    memset(service, 0, NI_MAXSERV); // same as memset(host, 0, NI_MAXHOST);
 
    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) //tries to get hostname and service
    {
        cout << host << " connected on port " << service << endl;
    }
    else  //otherwise falls back to IP and port.
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on port " << ntohs(client.sin_port) << endl;
    }
 
    // Close listening socket
    close(listening);
 
    // While loop: accept and echo message back to client
    char buf[4096]; //buffer for incoming data.
 
    while (true)
    {
        memset(buf, 0, 4096);
 
        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0); //recv waits for the data from the client
        if (bytesReceived == -1)
        {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }
 
        if (bytesReceived == 0)
        {
            cout << "Client disconnected " << endl;
            break;
        }
 
        cout << string(buf, 0, bytesReceived) << endl;
 
        // Echo message back to client
        send(clientSocket, buf, bytesReceived + 1, 0);
    }
 
    // Close the socket
    close(clientSocket);
 
    return 0;
}
