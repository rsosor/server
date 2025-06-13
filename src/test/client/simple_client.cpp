// SimpleClient.cpp
#include <winsock2.h>

#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Connect failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // std::string name = "Alice";
    std::string name = "LOGIN Alice letmein\n";
    send(sock, name.c_str(), name.size(), 0);
    std::cout << "Sent: " << name << std::endl;

    /**
     * bytesReceived > 0  → 收到正常資料
     * bytesReceived == 0 → 對方正常關閉連線，發送了 FIN
     * bytesReceived < 0  → 發生錯誤（例如 RST、網路中斷）
     * 
     * 添加這段
     * 
     * 遠端返回：End of file
     */
    char buffer[1024];
    int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0'; // 確保是 null 結尾的字串
        printf("[Client] Received: %s\n", buffer);
    } else if (bytesReceived == 0) {
        printf("[Client] Connection closed by server (FIN received)\n");
    } else {
        printf("[Client] recv failed: %d\n", WSAGetLastError());
    }

    /**
     * SD_BOTH、SD_SEND 只有在 -+（沒讀但睡眠）這個測試上有差異
     * 
     * + recv
     * + Sleep(2000)
     * 
     * 遠端返回：End of file
     * 
     * + recv
     * - Sleep(2000)
     * 
     * 遠端返回：End of file
     * 
     * - recv
     * + Sleep(2000)
     * 
     * 遠端返回（SD_BOTH）：遠端主機已強制關閉一個現存的連線。
     * 遠端返回（SD_SEND）：End of file
     * 
     * - recv
     * - Sleep(2000)
     * 
     * 遠端返回：連線已被您主機上的軟體中止。
     */
    // Sleep(2000);    // 給 server 足夠時間處理讀取。上面 -+ 這段才有差

    /**
     * 關閉寫，傳送 FIN 告知不會傳送訊息，伺服器會收到一個正常的 FIN。 
     * 傳送 FIN，對方 recv() 回傳 0（EOF）
     * 
     * 遠端返回：End of file
     */
    shutdown(sock, SD_SEND);    // 表示不再發送資料（但還可以接收）
    
    /**
     * 只是不允許自己再 recv()，系統不做任何 TCP 操作。
     * 
     * 遠端返回：遠端主機已強制關閉一個現存的連線。
     */
    // shutdown(sock, SD_RECEIVE); // 表示不再接收資料（但還可以發送）

    /**
     * 在 Windows 上等價於先 SD_RECEIVE，再 SD_SEND。
     * 若立即關閉，系統可能用 RST 中止連線，否則，會傳送 FIN
     * 
     * 遠端返回有兩種可能：
     * 遠端主機已強制關閉一個現存的連線。
     * End of file
     */
    // shutdown(sock, SD_BOTH);    // 同時關閉讀寫端
    
    // Sleep(5000);    // 不會改變原本的遠端返回：遠端主機已強制關閉一個現存的連線。

    /**
     * 若直接 closesocket() 而沒有先 shutdown()，則系統可能會：
     * 直接發送 TCP RST(reset) 封包
     * 
     * 導致伺服器報錯：「連線已被您主機上的軟體中止。」
     */
    closesocket(sock);
    WSACleanup();
    
    printf("Press Enter to exit...");
    std::cin.get();

    return 0;
}
