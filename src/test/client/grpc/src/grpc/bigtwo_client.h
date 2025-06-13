#pragma once

#include "bigtwo.grpc.pb.h"

#include <grpcpp/grpcpp.h>

class BigTwoClient {
public:
    explicit BigTwoClient(std::shared_ptr<grpc::Channel> channel);

    // 呼叫 JoinGame RPC
    int JoinGame(const std::string& player_name);

    // 呼叫 PlayCards RPC
    void PlayCards(int player_id, const std::vector<bigtwo::Card>& cards);

    // 呼叫 Pass RPC
    void Pass(int player_id);

    // 呼叫 GetGameState RPC
    void GetGameState(int player_id);

    // 呼叫 Chat RPC
    void Chat(int player_id, const std::string& message);

private:
    std::unique_ptr<bigtwo::BigTwoService::Stub> stub_;
};