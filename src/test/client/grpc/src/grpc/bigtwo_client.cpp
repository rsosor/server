#include "grpc/bigtwo_client.h"

#include <iostream>
#include <memory>
#include <string>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using namespace rsosor::generated;

// TODO: 暫時的 log 格式
#define LOG_INFO(msg) std::cout << "[Info] " << msg << '\n'
#define LOG_ERROR(msg) std::cerr << "[Error] " << msg << '\n'

BigTwoClient::BigTwoClient(std::shared_ptr<Channel> channel)
    : game_stub_(BigTwoService::NewStub(channel)), 
    table_stub_(BigTwoTableService::NewStub(channel)), 
    management_stub_(BigTwoManagementService::NewStub(channel)) {};

gen::LoginResponse BigTwoClient::Login(const std::string player_name, const std::string& psw) {
    std::cout << "---------- StartGame Started ----------" << '\n';
    LoginRequest req;
    LoginResponse res;
    ClientContext context;
    req.set_player_name(player_name);
    req.set_psw(psw);
    auto s = game_stub_->Login(&context, req, &res);
    if (s.ok()) {
        // std::cout << "StartGame: " << res.message() << '\n';
        LOG_INFO("StartGame: " << res.message());
        // TODO: 開始遊戲後的處理邏輯
    } else {
        // std::cerr << "[Error] StartGame RPC failed: " << s.error_message() << '\n';
        LOG_ERROR("StartGame RPC failed: " << s.error_message());
    }
    std::cout << "---------- StartGame Ended ----------" << '\n';
    std::cout << '\n';
    return res;
}

// 呼叫 JoinGame RPC
gen::JoinResponse BigTwoClient::JoinGame(const int player_id, const std::string& player_name, const std::string& room_id) {
    std::cout << "---------- JoinGame Started ----------" << '\n';

    JoinRequest request;
    request.set_player_id(player_id);
    request.set_player_name(player_name);
    request.set_room_id(room_id);

    JoinResponse response;
    ClientContext context;

    Status status = game_stub_->JoinGame(&context, request, &response);

    if (status.ok()) {
        if (response.success()) {
            int player_id = response.player_id();
            std::string player_name = response.player_name();
            std::cout << "JoinGame success! Player name: " << player_name << "（ID：" << player_id << "）\n";
            std::cout << "---------- JoinGame Ended ----------" << '\n';
            std::cout << '\n';
        } else {
            std::cout << "Joined room failed" << '\n';
        }
        // 初始化玩家本地狀態
    } else {
        std::cerr << "[Error] JoinGame RPC failed: " << status.error_message() << '\n';
        std::cout << "---------- JoinGame Ended ----------" << '\n';
        std::cout << '\n';
    }
    return response;
}

// 呼叫 PlayCards RPC
void BigTwoClient::PlayCards(int player_id, const std::vector<Card>& cards) {
    std::cout << "---------- PlayCards Started ----------" << '\n';
    PlayRequest request;
    request.set_player_id(player_id);
    for (const auto& card : cards) {
        *request.add_cards() = card;
    }

    PlayResponse response;
    ClientContext context;

    Status status = game_stub_->PlayCards(&context, request, &response);
    if (status.ok()) {
        std::cout << "PlayCards response: " << response.message() << '\n';
    } else {
        std::cerr << "[Error] PlayCards RPC failed: " << status.error_message() << '\n';
    }
    std::cout << "---------- PlayCards Ended ----------" << '\n';
    std::cout << '\n';
}

// 呼叫 Pass RPC
void BigTwoClient::Pass(int player_id) {
    std::cout << "---------- Pass Started ----------" << '\n';
    PassRequest request;
    request.set_player_id(player_id);

    PassResponse response;
    ClientContext context;

    Status status = game_stub_->Pass(&context, request, &response);
    if (status.ok()) {
        std::cout << "Pass response: " << response.message() << '\n';
    } else {
        std::cerr << "[Error] Pass RPC failed: " << status.error_message() << '\n';
    }
    std::cout << "---------- Pass Ended ----------" << '\n';
    std::cout << '\n';
}

// 呼叫 GetGameState RPC
void BigTwoClient::GetGameState(int player_id) {
    std::cout << "---------- GetGameState Started ----------" << '\n';
    GameStateRequest request;
    request.set_player_id(player_id);

    GameStateResponse response;
    ClientContext context;

    Status status = game_stub_->GetGameState(&context, request, &response);
    if (status.ok()) {
        std::cout << "Game phase: " << response.phase() << '\n';
        std::cout << "Current turn player ID: " << response.current_turn_player_id() << '\n';
        std::cout << "Your hand cards:" << '\n';
        for (const auto& card : response.hand()) {
            std::cout << "  " << card.suit() << " " << card.rank() << '\n';
        }
        std::cout << "Last played cards:" << '\n';
        for (const auto& card : response.last_played_cards()) {
            std::cout << "  " << card.suit() << " " << card.rank() << '\n';
        }
    } else {
        std::cerr << "[Error] GetGameState RPC failed: " << status.error_message() << '\n';
    }
    std::cout << "---------- GetGameState Ended ----------" << '\n';
    std::cout << '\n';
}

// 呼叫 Chat RPC
void BigTwoClient::Chat(int player_id, const std::string& message) {
    std::cout << "---------- Chat Started ----------" << '\n';
    ChatRequest request;
    request.set_player_id(player_id);
    request.set_message(message);

    ChatResponse response;
    ClientContext context;

    Status status = game_stub_->Chat(&context, request, &response);
    if (status.ok()) {
        std::cout << "Chat message sent successfully." << '\n';
    } else {
        std::cerr << "[Error] Chat RPC failed: " << status.error_message() << '\n';
    }
    std::cout << "---------- Chat Ended ----------" << '\n';
    std::cout << '\n';
}

void BigTwoClient::Deal(int player_id) {
    std::cout << "---------- Deal Started ----------" << '\n';
    DealRequest req;
    DealResponse res;
    ClientContext context;

    // req.add_player_ids(player_id);
    if (table_stub_->Deal(&context, req, &res).ok()) {
        std::cout << "Dealt successfully to players in game " << player_id << '\n';
    }
    std::cout << "---------- Deal Ended ----------" << '\n';
    std::cout << '\n';
}

void BigTwoClient::ValidatePlay(int player_id) {
    std::cout << "---------- ValidatePlay Started ----------" << '\n';
    PlayValidationRequest req;
    PlayValidationResponse res;
    ClientContext context;

    req.set_player_id(player_id);
    if (table_stub_->ValidatePlay(&context, req, &res).ok()) {
        std::cout << "Valid: " << res.is_valid()
                    << " message: " << res.message()
                    << '\n';
    }
    std::cout << "---------- ValidatePlay Ended ----------" << '\n';
    std::cout << '\n';
}

void BigTwoClient::HandleTimeout(int player_id) {
    std::cout << "---------- HandleTimeout Started ----------" << '\n';
    TimeoutRequest req;
    TimeoutResponse res;
    ClientContext context;

    req.set_player_id(player_id);
    if (table_stub_->HandleTimeout(&context, req, &res).ok()) {
        std::cout << "Timeout processed: " << res.success()
                    << " message: " << res.message()
                    << '\n';
    }
    std::cout << "---------- HandleTimeout Ended ----------" << '\n';
    std::cout << '\n';
}

void BigTwoClient::Score(const std::vector<int>& players) {
    std::cout << "---------- Score Started ----------" << '\n';
    ScoreRequest req;
    ScoreResponse res;
    ClientContext context;

    for (auto p : players) {
        req.add_player_ids(p);
    }
    if (table_stub_->Score(&context, req, &res).ok()) {
        for (auto& p : res.scores()) {
            std::cout << "Player " << p.first << " score: " << p.second << '\n';
        }
    }
    std::cout << "---------- Score Ended ----------" << '\n';
    std::cout << '\n';
}

// 查詢排行榜
void BigTwoClient::GetRanking(int top) {
    std::cout << "---------- GetRanking Started ----------" << '\n';
    RankingRequest req;
    RankingResponse res;
    ClientContext context;

    req.set_top(top);
    if (management_stub_->GetRanking(&context, req, &res).ok()) {
        for (auto& r : res.ranking()) {
            std::cout << "Player " << r.player_id()
                        << " (" << r.player_name()
                        << ") wins: " << r.wins()
                        << '\n';
        }
    }
    std::cout << "---------- GetRanking Ended ----------" << '\n';
    std::cout << '\n';
}

// 查詢勝率
void BigTwoClient::GetPlayerWinRate(int player_id) {
    std::cout << "---------- GetPlayerWinRate Started ----------" << '\n';
    PlayerRateRequest req;
    PlayerRateResponse res;
    ClientContext context;

    req.set_player_id(player_id);
    if (management_stub_->GetPlayerWinRate(&context, req, &res).ok()) {
        std::cout << "Win rate: " << res.win_ratio()
                    << "% (" << res.wins()
                    << "/" << res.total_games()
                    << ")\n";
    }
    std::cout << "---------- GetPlayerWinRate Ended ----------" << '\n';
    std::cout << '\n';
}

// 查詢勝敗紀錄
void BigTwoClient::GetActivityLog(int limit) {
    std::cout << "---------- GetActivityLog Started ----------" << '\n';
    ActivityLogRequest req;
    ActivityLogResponse res;
    ClientContext context;

    req.set_limit(limit);
    if (management_stub_->GetActivityLog(&context, req, &res).ok()) {
        for (auto& e : res.entries()) {
            std::cout << "Activity - game " << e.game_id()
                        << ", winner " << e.winner()
                        << ", at " << e.timestamp()
                        << '\n';
        }
    }
    std::cout << "---------- GetActivityLog Ended ----------" << '\n';
    std::cout << '\n';
}
