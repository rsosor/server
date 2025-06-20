#pragma once

#include "bigtwo.grpc.pb.h"

#include <grpcpp/grpcpp.h>

namespace gen = rsosor::generated;

class BigTwoClient {
public:
    explicit BigTwoClient(std::shared_ptr<grpc::Channel> channel);

    // 呼叫 StartGame RPC
    void StartGame(int initiator);

    // 呼叫 JoinGame RPC
    int JoinGame(const std::string& player_name);

    // 呼叫 PlayCards RPC
    void PlayCards(int player_id, const std::vector<gen::Card>& cards);

    // 呼叫 Pass RPC
    void Pass(int player_id);

    // 呼叫 GetGameState RPC
    void GetGameState(int player_id);

    // 呼叫 Chat RPC
    void Chat(int player_id, const std::string& message);

    void Deal(int player_id);

    void ValidatePlay(int player_id);

    void HandleTimeout(int player_id);

    void Score(const std::vector<int>& players);

    void GetRanking(int top);

    void GetPlayerWinRate(int player_id);

    void GetActivityLog(int limit);

private:
    std::unique_ptr<gen::BigTwoService::Stub> game_stub_;
    std::unique_ptr<gen::BigTwoTableService::Stub> table_stub_;
    std::unique_ptr<gen::BigTwoManagementService::Stub> management_stub_;
};