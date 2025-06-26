#include "grpc/bigtwo_client.h"
#include "login/login.h"
#include "websocket/websocket.h"
#include "game/game_client.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

using tcp = net::ip::tcp;

namespace gen = rsosor::generated;

int main() {
    // 0. 登入
    /**
     * 可以用什麼 C++ 庫實作 REST Client？
     * cpp-httplib — header only，簡單好用
     * cpr — C++ HTTP requests library，使用起來像 Python requests
     * Boost.Beast — Boost.Asio 的 HTTP/HTTPS 實作，稍微複雜但非常強大
     * 
     * Server 如果要做 REST 服務，可用：
     * cpp-httplib 也有 Server 端實作
     * Crow、Pistache、Restbed 等 C++ REST 框架
     * 用你熟悉的其他語言快速實作（Python Flask、Node.js Express），C++ 作為遊戲邏輯後端
     */
    Client http_client;

    std::string host = "127.0.0.1";
    std::string socket_port = "12345";
    std::string http_port = "8080";
    std::string grpc_url = "localhost:50051";
    std::string ws_port = "8443";
    std::string username = "Alice";
    std::string password = "mypassword";
    std::string login_msg = username + " " + password;

    /**
     * tcp socket 方式
     * - connect() 伺服器
     * - send() 資料
     * - receive() 回答
     * - 手動處理序列化/解讀
     */
    // if (http_client.connect(host, port)) {
    //     if (http_client.send(login_msg)) {
    //         std::string reply = http_client.receive();
    //         std::cout << "Received from server: " << reply << '\n';
    //     }
    // }

    /**
     * http blocking io
     * 封裝後
     * 它內部自己 connect
     * POST /login
     * await 伺服器響應
     * 解析響應
     * 最後只回 true 或 false
     */
    // if (http_client.login(host, http_port, username, password)) {
    //     std::cout << "Login successful!" << '\n';
    // } else {
    //     std::cout << "Login failed!" << '\n';
    // }

    /**
     * http async io
     */
    net::io_context ioc;

    std::string body = R"({"username": "Alice", "password": "mypassword"})";
    auto async_http_client = std::make_shared<AsyncClient>(ioc, host, http_port, body);
    async_http_client->async_login();

    ioc.run();

    /**
     * 模擬 bigtwo，client 先自主驗證
     */
    GameClient client;
    client.play();

    /**
     * grpc io
     */
    // 1. 由第一位玩家（Alice）啟動遊戲
    // BigTwoClient rpc_client(grpc::CreateChannel(grpc_url, grpc::InsecureChannelCredentials()));

    // int alice_id = rpc_client.JoinGame("Alice");
    // int bob_id = rpc_client.JoinGame("Bob");
    // int charlie_id = rpc_client.JoinGame("Charlie");
    // int dave_id = rpc_client.JoinGame("Dave");

    // // 2. 由 Alice 發起遊戲
    // rpc_client.StartGame(alice_id);

    // // 3. 由 server 發牌
    // rpc_client.Deal(alice_id);

    // // 4. 每位可以依序出牌
    // // 假如我們事先知道每人的第一張牌
    // gen::Card alice_card;
    // alice_card.set_suit("clubs");
    // alice_card.set_rank(3);
    // rpc_client.PlayCards(alice_id, {alice_card});

    // gen::Card bob_card;
    // bob_card.set_suit("diamonds");
    // bob_card.set_rank(4);
    // rpc_client.PlayCards(bob_id, {bob_card});

    // // Charlie pass
    // rpc_client.Pass(charlie_id);

    // gen::Card dave_card;
    // dave_card.set_suit("hearts");
    // dave_card.set_rank(5);
    // rpc_client.PlayCards(dave_id, {dave_card});

    // // 5. 檢查遊戲當前狀態
    // rpc_client.GetGameState(alice_id);

    // // 6. 玩家可以發送聊天室消息
    // rpc_client.Chat(bob_id, "Nice");

    // /**
    //  * websocket io
    //  */
    // const std::string message = "Hello from WSS client!";
    // run_tls_websocket_client(host, ws_port, message);

    // // 7. 一旦遊戲結束，可以由 server 計分
    // std::vector<int> players;
    // players.push_back(alice_id);
    // players.push_back(bob_id);
    // players.push_back(charlie_id);
    // players.push_back(dave_id);
    // rpc_client.Score(players);

    // // 8. 檢視排行與勝率
    // rpc_client.GetRanking(5);
    // rpc_client.GetPlayerWinRate(alice_id);
    // rpc_client.GetActivityLog(10);

    return 0;
}