#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

std::string getExtension(const std::string& url) {
    size_t dotPos = url.find_last_of('.');
    if (dotPos != std::string::npos)
        return url.substr(dotPos + 1);
    return "";
}

bool directoryExists(const std::string& path) {
    std::ifstream dir(path);
    return dir.good();
}

std::vector<char> readImageFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return std::vector<char>();
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
        return std::vector<char>();
    }

    return buffer;
}

void sendResponse(int clientSocket, const std::string& mimeType, const std::vector<char>& data) {
    std::string response;
    response += "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + mimeType + "\r\n";
    response += "Content-Length: " + std::to_string(data.size()) + "\r\n";
    response += "\r\n";

    send(clientSocket, response.c_str(), response.size(), 0);
    send(clientSocket, data.data(), data.size(), 0);
}

void sendFile(int clientSocket, const std::string& filePath, std::string extension) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
      std::vector<char> htmlData = readImageFile("./errores/404.html");

      std::string response;
      response += "HTTP/1.1 404 Not Found\r\n";
      response += "Content-Type: text/html\r\n";
      response += "Content-Length: " + std::to_string(htmlData.size()) + "\r\n";
      response += "\r\n";

      send(clientSocket, response.c_str(), response.size(), 0);
      send(clientSocket, htmlData.data(), htmlData.size(), 0);
      return;
    }

    std::string contentType = "application/octet-stream";

    std::ostringstream oss;
    oss << file.rdbuf();
    std::string fileContent = oss.str();

    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    response += "Content-Length: " + std::to_string(fileContent.size()) + "\r\n";
    response += "\r\n";
    response += fileContent;

    send(clientSocket, response.c_str(), response.size(), 0);
}

void handleRequest(int clientSocket, const std::string& request) {
    std::istringstream requestStream(request);
    std::string method, url, httpVersion;
    requestStream >> method >> url >> httpVersion;

    if (url.empty() || url[0] != '/') {
      std::vector<char> htmlData = readImageFile("./errores/400.html");

      std::string response;
      response += "HTTP/1.1 400 Bad Request\r\n";
      response += "Content-Type: text/html\r\n";
      response += "Content-Length: " + std::to_string(htmlData.size()) + "\r\n";
      response += "\r\n";

      send(clientSocket, response.c_str(), response.size(), 0);
      send(clientSocket, htmlData.data(), htmlData.size(), 0);
      return;
    }

    if (method != "GET") {
      std::vector<char> htmlData = readImageFile("./errores/405.html");

      std::string response;
      response += "HTTP/1.1 405 Method Not Allowed\r\n";
      response += "Content-Type: text/html\r\n";
      response += "Content-Length: " + std::to_string(htmlData.size()) + "\r\n";
      response += "\r\n";

      send(clientSocket, response.c_str(), response.size(), 0);
      send(clientSocket, htmlData.data(), htmlData.size(), 0);
      return;
    }

    if (httpVersion != "HTTP/1.1") {
      std::vector<char> htmlData = readImageFile("./errores/505.html");

      std::string response;
      response += "HTTP/1.1 505 HTTP Version Not Supported\r\n";
      response += "Content-Type: text/html\r\n";
      response += "Content-Length: " + std::to_string(htmlData.size()) + "\r\n";
      response += "\r\n";

      send(clientSocket, response.c_str(), response.size(), 0);
      send(clientSocket, htmlData.data(), htmlData.size(), 0);
      return;
    }



    std::string filePath = "." + url;
    std::string extension = getExtension(url);

    if (extension == "png" || extension == "jpeg" || extension == "jpg") {
        std::vector<char> imageData = readImageFile(filePath);

        if (imageData.empty()) {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
            send(clientSocket, response.c_str(), response.size(), 0);
        } else {
            sendResponse(clientSocket, "image/" + extension, imageData);
        }
    } else if (extension == "html"){
        std::vector<char> htmlData = readImageFile(filePath);

        if (htmlData.empty()) {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
            send(clientSocket, response.c_str(), response.size(), 0);
        } else {
            sendResponse(clientSocket, "text/" + extension, htmlData);
        }
    } else if (extension == "css"){
        std::vector<char> cssData = readImageFile(filePath);

        if (cssData.empty()) {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
            send(clientSocket, response.c_str(), response.size(), 0);
        } else {
            sendResponse(clientSocket, "text/" + extension, cssData);
        }
    } else if (extension == "js"){
        std::vector<char> jsData = readImageFile(filePath);

        if (jsData.empty()) {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
            send(clientSocket, response.c_str(), response.size(), 0);
        } else {
            sendResponse(clientSocket, "application/javascript", jsData);
        }
    } else if (extension == "" && directoryExists(filePath)){
          
        if(url[url.length() - 1] == '/'){
          std::vector<char> htmlData = readImageFile(filePath + "index.html");
          sendResponse(clientSocket, "text/html", htmlData);
        }
        else{
        
          std::string response = "HTTP/1.1 301 Moved Permanently\r\n";
          response += "Location: http://localhost:8080" + url + "/\r\n";
          response += "Content-Length: 0\r\n";
          response += "\r\n";

          send(clientSocket, response.c_str(), response.size(), 0);
        
        }
    }
    else{
      sendFile(clientSocket, filePath, extension);
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error al crear el socket." << std::endl;
        return 1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error al vincular el socket." << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error al escuchar por conexiones entrantes." << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Esperando conexiones en el puerto " << PORT << "..." << std::endl;

    while (true) {
        clientAddressLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            std::cerr << "Error al aceptar la conexión entrante." << std::endl;
            close(serverSocket);
            return 1;
        }

        char requestBuffer[BUFFER_SIZE];
        memset(requestBuffer, 0, sizeof(requestBuffer));
        recv(clientSocket, requestBuffer, sizeof(requestBuffer) - 1, 0);

        handleRequest(clientSocket, requestBuffer);

        close(clientSocket);
    }

    close(serverSocket);

    return 0;
}
