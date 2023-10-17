#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iomanip>
#include "sha256.h"
#include <vector>

using namespace std;

vector<string> splitString(const string& str, char delimiter) {
    vector<string> parts;
    string part;
    size_t startPos = 0;
    size_t foundPos = str.find(delimiter);

    while (foundPos != string::npos) {
        part = str.substr(startPos, foundPos - startPos);
        parts.push_back(part);
        startPos = foundPos + 1;
        foundPos = str.find(delimiter, startPos);
    }

    part = str.substr(startPos);
    parts.push_back(part);

    return parts;
}

int main() {
    int recipient_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (recipient_socket < 0) {
        perror("socket creation failed");
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(12345); 
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); 

    if (bind(recipient_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind failed");
        return 1;
    }

    if (listen(recipient_socket, 1) < 0) { 
        perror("listen failed");
        return 1;
    }

    struct sockaddr_in client_address;
    socklen_t client_addr_len = sizeof(client_address);
    int client_sock = accept(recipient_socket, (struct sockaddr *)&client_address, &client_addr_len); 
    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }

    const int MAX_NAME_LENGTH = 256;
    char str[256];
    int strReceived = recv(client_sock, str, MAX_NAME_LENGTH - 1, 0);
    if(strReceived < 0){
      perror("error receiving file name");
      return 1;
    }

    vector<string> parts = splitString(str,';');
    string hash = parts[0];
    string filename = parts[1];

    ofstream file("fileFolder/" + filename + ".enc", ios::binary);
    if (!file) {
        cerr << "Error creating file" << endl;
        return 1;
    }

    int size;
    int sizeReceived = recv(client_sock, (char *)&size, sizeof(size), 0);
    if(sizeReceived < 0){
      perror("error receiving file size");
      return 1;
    }

    cout << "Name of the file: " << filename << endl;
    cout << "Size of the file: " << fixed << setprecision(2) <<(float)size/1000.0 << " KB"<<endl;

    int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    int bytesReceived = 0;

    while(true){
      int bytesRead = recv(client_sock, buffer, BUFFER_SIZE, 0);
      if (bytesRead <= 0)
        break;

      file.write(buffer, bytesRead);
      bytesReceived += bytesRead;
      cout<< "Receiving : " << fixed << setprecision(2) <<(float)bytesReceived * 100/(float)size<<" %\r";
    }
    file.close();

    cout<<endl;

    string command = "./decrypt llave.key fileFolder/" + filename + ".enc" + " fileFolder/" + filename;
    system(command.c_str());

    if(hash == sha256(filename))
      cout<<"File recieved correctly"<<endl;
    else
      cout<<"Error recieving the file"<<endl;

    close(client_sock);
    close(recipient_socket);
    return 0;
}
