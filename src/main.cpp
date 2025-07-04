#include "conn/connection_layer.h"
#include "conn/connection_manager.h"
#include "game/game_room/game_room.h"
#include "server/server.h"
// #include "server/core_server.h" // old
#include "grpc/bigtwo_server.h"
#include "util/env.h"

#include <iostream>
#include <memory>
// #include <future>

#include <boost/asio/ssl.hpp>

namespace net = boost::asio;
namespace ssl = net::ssl;

int main() {
    UINT out_codepage = GetConsoleOutputCP();
    UINT in_codepage = GetConsoleCP();
    std::cout << "Current console input codepage: " << in_codepage << '\n';
    std::cout << "Current console output codepage: " << out_codepage << '\n';

    std::cout << "Before SetConsoleCP(): " << in_codepage << "\n";
    std::cout << "Before SetConsoleOutputCP(): " << out_codepage << "\n";

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "After SetConsoleCP(): " << GetConsoleCP() << "\n";
    std::cout << "After SetConsoleOutputCP(): " << GetConsoleOutputCP() << "\n";

    if (GetConsoleCP() == GetConsoleOutputCP())
        std::cout << "stdio 初始化成功！" << '\n';
    else {
        std::cout << "stdio 初始化失敗" << '\n';
        return 1;
    }

    // ignore 可以避免前面的 input 沒處理好，但是要按一次 enter
    // std::string in;
    // std::cin >> in;
    // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string username, password;
    std::cout << "請輸入名稱：";
    std::getline(std::cin, username);
    std::cout << "請輸入密碼：";
    std::getline(std::cin, password);
    std::cout << "歡迎您：" << username << "！" << '\n';
    std::cout << "password：" << password[0] << "*******" << password[password.size() - 1] << '\n';

    /**
     * input: 
     * 
     * Aaron 謝
     * 12345
     * 
     * output: 
     * 
     * Current console input codepage: 65001
     * Current console output codepage: 950
     * Before SetConsoleCP(): 65001
     * Before SetConsoleOutputCP(): 950
     * After SetConsoleCP(): 65001
     * After SetConsoleOutputCP(): 65001
     * stdio 初始化成功！
     * 請輸入名稱：Aaron 謝
     * 請輸入密碼：12345
     * 歡迎您：Aaron 謝！
     * password：1*******5
     */

    // // 建立4個玩家
    // Player p1(1, "Alice");
    // Player p2(2, "Bob");
    // Player p3(3, "Carol");
    // Player p4(4, "Dave");
    // std::vector<Player*> players = {&p1, &p2, &p3, &p4};

    // // 建立遊戲房間
    // auto game_room = std::make_shared<GameRoom>(players);
    // game_room->start();

    // // 建立連線層
    // ConnectionLayer conn(game_room);

    // // 模擬收到玩家1出牌的訊息
    // std::string msg = R"({"type":"play_card","cards":["3D","3S"]})";
    // conn.handle_message(1, msg);

    /**
     * output: 
     * 
     * Game started with 4 players.
     * Player played cards.
     */

    // ConnectionManager cm;

    // cm.add_player(1, "Alice");
    // cm.add_player(2, "Bob");
    // cm.remove_player(1);
    // cm.add_player(3, "Charlie");

    /**
     * output: 
     * 
     * Player Alice (ID 1) connected.
     * Player Bob (ID 2) connected.
     * Player Alice (ID 1) disconnected.
     * Player Charlie (ID 3) connected.
     */

    /**
     * 同步、非同步的差異：
     * 
     * 同步版本：用 core_server（或類似的程式碼）直接用阻塞方式 accept() + read_until() + write()，一個連線一個線程（或阻塞等待），流程比較直覺但效能較差。
     * 非同步版本：用 Server + Connection 類別架構，利用 Boost.Asio 的非同步 async_accept 和非同步讀寫，透過事件迴圈 ioc.run() 驅動，單線程可以同時管理多連線，效率較高。
     * 兩者不是用同一套函式來處理，寫法與架構會不同，尤其：
     * 同步版多半在主線程阻塞等待連線和資料，連線一多就要拆多線程。
     * 非同步版設計非同步 callback，讓事件驅動架構能用單一線程高效處理大量連線。
     * 
     * 版本	            流程	                                        使用的類別/函式	         特點
     * 同步阻塞	        accept() → read_until() → write()	            core_server（或類似）	簡單易懂，但效能差，阻塞等待
     * 非同步事件驅動	async_accept() → async_read() → async_write()	Server + Connection	    非阻塞，多連線可單線程高效管理
     */
    // try {
    //     boost::asio::io_context ioc;
    //     boost::asio::ip::tcp::acceptor acceptor(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 12345));
    //     std::cout << "Server is listening on port：12345..." << '\n';

    //     while (true) {
    //         // 優點：
    //         // 容易理解，邏輯直觀。
    //         // 缺點：
    //         // 同步（blocking I/O）
    //         // 每個連線都會卡住在 accept() 等待，阻塞主執行緒。
    //         // 如果你要同時處理多個玩家，只能用 std::thread 拆線程，每條連線佔一個 thread，效能不佳。
    //         // 沒辦法好好整合 Boost.Asio 的事件驅動特性（像 IO multiplexing、coroutine、handler queue 等）。
    //         auto socket = std::make_shared<boost::asio::ip::tcp::socket>(ioc);
    //         acceptor.accept(*socket);
    //         std::thread(handle_client, socket).detach();
    //     }
    // } catch (std::exception& e) {
    //     std::cerr << "Exception: " << e.what() << '\n';
    // }

    /**
     * input: 
     * 
     * Alice
     * 
     * output: 
     * 
     * Server is listening on port：12345...
     * Client error: read_until: End of file [asio.misc:2]
     * 
     * 
     * intput: 
     * 
     * LOGIN Alice letmein\n
     * 
     * output: 
     * 
     * Server is listening on port：12345...
     * Received from client: LOGIN Alice letmein
     * Client 1 logged in: Alice
     * 
     * Server is listening on port：12345...
     * Received from client: LOGIN Alice letmein
     * Player Alice 加入等待區，目前等待人數: 1
     * Client 1 logged in: Alice
     */

    // 你不是在 main() 裡面用 acceptor.accept() 去等待連線。
    // 你已經在 Server 類裡用非同步的方式接受玩家連線。
    // try {
    //     boost::asio::io_context ioc;
    //     Server server(ioc, 12345);   // 建立 server 物件並初始化 acceptor
    //     std::cout << "Server is listening on port：12345..." << '\n';

    //     // 確認有多少可用的硬體執行緒（CPU 核心數）
    //     unsigned int thread_count = std::thread::hardware_concurrency();
    //     std::cout << "thread_count: " << thread_count << '\n';
    //     if (thread_count == 0) thread_count = 2; // 若偵測不到就預設 2 條

    //     std::vector<std::thread> threads;
    //     for (unsigned int i = 0; i < thread_count; ++i) {
    //         threads.emplace_back([&ioc]() {
    //             ioc.run(); // 每條 thread 都執行 ioc 的事件處理迴圈
    //         });
    //     }

    //     // 執行 gRPC Server 在另一條 thread 裡
    //     // std::thread grpc_thread([]() {
    //     //     grpc::Channel::RunGrpcServer();
    //     // });

    //     // ioc.run();   // 開始事件循環
    //     // grpc_thread.join();

    //     // 等待所有 threads 結束
    //     for (auto& t : threads) {
    //         t.join();
    //     }
    // } catch (std::exception& e) {
    //     std::cerr << "Server error: " << e.what() << '\n';
    // }

    /**
     * client.exe
     * 
     * input: 
     * 
     * LOGIN Alice letmein\n
     * 
     * output:
     * 
     * Server is listening on port：12345...
     * thread_count: 24
     * New player connected.
     * Player name: LOGIN Alice letmein
     * 
     * send_welcome 處理中
     * send_welcome async_write success
     * Disconnected: End of file
     * 1
     * 2
     * 3
     * 4
     * 5
     * 
     * multi_client.exe
     * 
     * input:
     * 
     * LOGIN Alice letmein\n
     * 
     * output:
     * 
     * Server is listening on port12345...
     * thread_count: 24
     * New player connected.
     * Player name: LOGIN Alice letmein
     * 
     * New player connected.
     * send_welcome 處理中
     * Player name: LOGIN Alice letmein
     * 
     * send_welcome async_write successsend_welcome async_write success
     * 
     * New player connected.
     * send_welcome 處理中
     * Disconnected: End of file
     * New player connected.
     * Player name: LOGIN Alice letmein
     * 
     * Disconnected: send_welcome 處理中
     * End of file
     * send_welcome async_write success
     * New player connected.
     * Player name: LOGIN Alice letmein
     * 
     * Disconnected: End of file
     * New player connected.
     * send_welcome 處理中
     * Player name: LOGIN Alice letmein
     * 
     * New player connected.
     * Player name: LOGIN Alice letmein
     * New player connected.
     * send_welcome async_write successPlayer name: LOGIN Alice letmein
     * 
     * send_welcome 處理中send_welcome 處理中
     * New player connected.
     * send_welcome async_write success
     * 
     * send_welcome async_write success
     * Player name: LOGIN Alice letmein
     * 
     * New player connected.
     * send_welcome 處理中
     * Disconnected: End of file
     * send_welcome async_write success
     * Player name: LOGIN Alice letmein
     * 
     * 
     * send_welcome async_write success
     * send_welcome 處理中
     * send_welcome async_write success
     * Player name: LOGIN Alice letmein
     * Disconnected: End of file
     * Disconnected: End of file
     * send_welcome 處理中
     * send_welcome async_write success
     * Disconnected: End of file
     * send_welcome 處理中
     * Disconnected: Disconnected: End of file
     * End of file
     * Disconnected: End of file
     * 1
     * 11
     * 1
     * 1
     * 1
     * 1
     * 1
     * 1
     * 1
     * 22
     * 2
     * 2
     * 2
     * 2
     * 2
     * 2
     * 2
     * 2
     * 33
     * 3
     * 3
     * 3
     * 3
     * 3
     * 3
     * 3
     * 3
     * 4
     * 4
     * 4
     * 4
     * 4
     * 4
     * 4
     * 4
     * 4
     * 4
     * 5
     * 5
     * 5
     * 5
     * 5
     * 5
     * 5
     * 5
     * 5
     * 5
     */

    /**
     * 整合 grpc
     * 
     * Future：
     * 讓 gRPC 與 ioc.run() 使用 std::jthread（C++20）
     * 如果未來轉到 C++20，可以用 std::jthread，自動 join()，更安全。
     * 
     * 把 thread 包成 ThreadManager 類別
     * 如伺服器再擴大，有更多 thread（例如資料庫 thread、AI 運算 thread）
     * 可以封裝 thread + promise/future 成 ManagedThread 類型。
     */
    try {
        // 確認有多少可用的硬體執行緒（CPU 核心數）
        unsigned int thread_count = std::thread::hardware_concurrency();
        if (thread_count == 0) thread_count = 2; // 若偵測不到就預設 2 條
        std::cout << "thread_count: " << thread_count << '\n';

        net::io_context ioc;
        // 設定 TLS 憑證與私鑰
        ssl::context ssl_ctx{ssl::context::tlsv12_server};

        auto env_ump = load_env();

        setup_ssl_context(ssl_ctx, env_ump["TLS_CERT_PATH"], env_ump["TLS_KEY_PATH"]);

        const unsigned short socket_port = 12345;
        const unsigned short http_port = 8080;
        const std::string grpc_addr = "0.0.0.0:50051";
        const unsigned short ws_port = 8443;

        // 建立 server 物件並初始化 acceptor
        Server socket_server(ioc, socket_port);

        // 建立 shared_ptr 的 HttpServer 並啟動非同步 accept
        auto http_server = std::make_shared<HttpServer>(ioc, http_port);
        http_server->run();

        /**
         * gRPC thread 的例外要自己包
         * 用 lambda 或封裝類包裝 thread
         * 為了更好管理多執行緒，可以用 std::function<void()> 或封裝類別來統一管理 worker 執行緒
         */
        // 啟動 gRPC server thread
        std::thread grpc_thread(std::move([&grpc_addr]() {
            try {
                rsosor::grpc_api::RunGrpcServer(grpc_addr);
            } catch (const std::exception& e) {
                std::cerr << "[gRPC Thread Error] " << e.what() << '\n';
            }
        }));

        // 啟動 websocket server
        auto ws_server = std::make_shared<WebSocketServer>(ioc, ssl_ctx, ws_port);
        ws_server->run();

        /**
         * v1.
         * 每條 thread 用 try-catch 包起來 + 傳送錯誤回主線程
         */
        // 啟動多個 ioc threads
        std::vector<std::thread> threads;
        std::vector<std::future<void>> thread_results;

        for (unsigned int i = 0; i < thread_count; ++i) {
            std::promise<void> p;
            thread_results.push_back(p.get_future());

            threads.emplace_back([&ioc, prom = std::move(p)]() mutable {
                try {
                    ioc.run();          // 每條 thread 都執行同一個 ioc 的事件處理迴圈
                    prom.set_value();   // 成功結束
                } catch (const std::exception& e) {
                    std::cerr << "[Thread Error] " << e.what() << '\n';
                    try {
                        prom.set_exception(std::current_exception());   // 傳例外給主線程
                    } catch (...) {}
                }
            });
        }

        // 主線程中等待與檢查
        for (auto& f : thread_results) {
            try {
                f.get();    // 會 rethrow 執行緒中發生的例外
            } catch (const std::exception& e) {
                std::cerr << "[Main] A thread threw exception: " << e.what() << '\n';
            }
        }

        // 等待所有 threads 結束
        for (auto& t : threads) {
            t.join();
        }
        grpc_thread.join();

        /**
         * v2.
         * 使用 thread-safe queue 傳送錯誤訊息給主線程
         * 如果有長時間運作的伺服器、或想做監控
         * 可用一個 std::queue<std::string> + mutex 將錯誤寫入
         * 主線程定期檢查 queue。
         */
    } catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << '\n';
    }

    return 0;
}
