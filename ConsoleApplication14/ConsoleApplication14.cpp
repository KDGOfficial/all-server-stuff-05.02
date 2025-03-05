#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 54000
#define SERVER_IP "127.0.0.1"

using namespace std;

bool StartServerIfNotRunning() {
    HWND hwnd = FindWindow(NULL, L"Server Window"); // Проверяем, запущен ли сервер
    if (hwnd != NULL) {
        cout << "Сервер уже запущен." << endl;
        return true;
    }

    wstring serverPath = L"G:\\ConsoleApplication10\\x64\\Debug\\Server.exe"; // Путь к серверу
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcess(serverPath.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        cout << "Сервер успешно запущен." << endl;
        Sleep(2000); // Даем серверу время на запуск
        return true;
    }
    else {
        cout << "Ошибка запуска сервера!" << endl;
        return false;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    if (!StartServerIfNotRunning()) {
        return 1;
    }

    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
    char recvBuffer[1024];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Ошибка инициализации Winsock!" << endl;
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Ошибка создания сокета!" << endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Не удалось подключиться к серверу!" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Подключение к серверу прошло успешно!" << endl;

    while (true) {
        string message;
        cout << "Введите число: ";
        cin >> message;

        if (send(clientSocket, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
            cout << "Ошибка отправки данных!" << endl;
            break;
        }

        int bytesReceived = recv(clientSocket, recvBuffer, 1024, 0);
        if (bytesReceived > 0) {
            recvBuffer[bytesReceived] = '\0';
            cout << "Ответ от сервера: " << recvBuffer << endl;
        }
        else {
            cout << "Ошибка получения данных!" << endl;
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
