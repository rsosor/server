#pragma once

#include "player/player.h"

#include <iostream>
#include <memory>
#include <string>

#include <boost/asio.hpp>

extern int next_client_id;
extern std::unordered_map<int, Player> player_registry;

// extern int next_client_id;
// extern std::unordered_map<int, Player> player_registry;
// extern std::vector<Player> waiting_room;
// extern std::mutex waiting_room_mutex;
// extern const std::string hardcoded_token;

class Connection : public std::enable_shared_from_this<Connection> {
public:
    explicit Connection(boost::asio::ip::tcp::socket socket);
    std::string get_time_string(std::time_t);
    void start();
    
private:
    void read_name();
    void send_welcome();
    void do_read();
    bool process_command(const std::string& line);

    boost::asio::ip::tcp::socket socket_;
    std::string player_name_;
    /**
     * 使用 boost::asio::streambuf buffer 搭配 async_read_until
     * ✅ 特點：
     * 項目	        說明
     * 緩衝區型別	boost::asio::streambuf（可動態擴充）
     * 結束條件	    直到遇到特定的字串（例如 "\n"）才停止
     * 適合	        處理文字資料（指令、聊天訊息、JSON 字串）
     * 輸入方式	    可用 std::istream 方便地解析輸入
     * 優點	        自動幫你處理資料邊界問題（很適合行為式協定）
     * 缺點	        若資料沒結束符號，會一直等；動態緩衝區略慢
     */
    boost::asio::streambuf buffer_;
    /**
     * 固定緩衝區 char data_[1024]
     * ✅ 特點：
     * 項目	        說明
     * 緩衝區型別	固定長度的 char[] 或 std::vector<char>
     * 結束條件	    資料一到就立刻返回，無需結束符號
     * 適合	        處理二進位協定、自訂封包（例如遊戲封包）
     * 輸入方式	    自己手動管理 buffer 的內容與長度
     * 優點	        更快、更靈活
     * 缺點	        自己要處理資料黏包、拆包問題（麻煩！）
     */
    char data_[1024];
};
