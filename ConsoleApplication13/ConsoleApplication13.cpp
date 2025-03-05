#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 54000

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");

    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    char recvBuffer[1024];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Ошибка инициализации Winsock!" << endl;
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Ошибка создания сокета!" << endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Ошибка привязки сокета!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Ошибка запуска прослушивания!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Сервер запущен и ожидает подключений..." << endl;

    int clientSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Ошибка принятия подключения!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Клиент подключился!" << endl;

    while (true) {
        int bytesReceived = recv(clientSocket, recvBuffer, 1024, 0);
        if (bytesReceived <= 0) {
            cout << "Клиент отключился." << endl;
            break;
        }

        recvBuffer[bytesReceived] = '\0';
        int receivedNumber = atoi(recvBuffer);
        receivedNumber++;

        string response = to_string(receivedNumber);
        send(clientSocket, response.c_str(), response.size(), 0);
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
