#include "sha256.h"
#include <arpa/inet.h>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main() {

  int sender_socket = socket(AF_INET, SOCK_STREAM, 0); 
  if (sender_socket < 0) {
    perror("socket creation failed");
    return 1;  
  }

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(12345); // port number
  server_address.sin_addr.s_addr =
      inet_addr("1.22.3.44"); // IP address of the recipient

  if (connect(sender_socket, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0) {
    perror("connection failed");
    return 1;
  }


  string filename;
  cout << "Enter name of the file to send" << endl;
  cin >> filename;
  

  string hash = sha256(filename);

  string command = "./encrypt llave.key fileFolder/" + filename + " fileFolder/" + filename + ".enc";
  system(command.c_str());

  ifstream file("fileFolder/" + filename + ".enc", ios::binary | ios::ate);
  if (!file) {
    perror("error opening file");
    return 1;
  }

  int size = file.tellg();
  file.seekg(0, ios::beg);

  cout << "Name of the file: " << filename << endl;
  cout << "Size of the file: " << fixed << setprecision(2)
       << (float)size / 1000.0 << " KB" << endl;

  string str = hash + ";" + filename;
  
  int strSent = send(sender_socket, str.c_str(), str.size(), 0);
  if (strSent < 0) {
    perror("error sending file name and hash");
    return 1;
  }

  int sizeSent = send(sender_socket, (char *)&size, sizeof(size), 0);
  if (sizeSent < 0) {
    perror("error sending file size");
    return 1;
  }

  const int bufferSize = 1024;
  char buffer[bufferSize];
  int bytes = 0;
  while (!file.eof()) {
    file.read(buffer, bufferSize);
    int bytesRead = file.gcount();
    if (bytesRead <= 0)
      break;

    int bytesSent = send(sender_socket, buffer, bytesRead, 0);
    bytes += bytesSent;
    cout << "Sending: " << fixed << setprecision(2)
         << (float)bytes * 100 / (float)size << " %\r";
    if (bytesSent < bytesRead) {
      perror("error sending file");
      break;
    }
  }
  cout << endl;

  file.close();
  close(sender_socket);

  return 0;
}