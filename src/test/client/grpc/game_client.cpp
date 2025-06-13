/**
 * cl /std:c++23 /I. /I<grpc include> game.pb.cc game.grpc.pb.cc server.cpp /Fe:server.exe /link /LIBPATH:<grpc lib> grpc++.lib libprotobuf.lib
 * 
 * cl /std:c++23 src/test/client/grpc/game_client.cpp src/generated/game.pb.cc src/generated/game.grpc.pb.cc /Fe:src/test/client/grpc -I C:\Users\godpk\vcpkg\installed\x64-windows\include -link C:\Users\godpk\vcpkg\installed\x64-windows\lib grpc++.lib libprotobuf.lib gpr.lib
 */

#pragma once

#include "game.grpc.pb.h"

#include <grpcpp/grpcpp.h>

#include <memory>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class GameClient {
public:
    GameClient(std::shared_ptr<Channel> channel)
        : stub_(GameService::NewStub(channel)) {}

    void Move(int player_id, int x, int y) {
        MoveRequest request;
        request.set_player_id(player_id);
        request.set_x(x);
        request.set_y(y);

        MoveResponse response;
        ClientContext context;

        Status status = stub_->Move(&context, request, &response);

        if (status.ok()) {
            std::cout << "Move success: " << response.success() << "\n";
        } else {
            std::cerr << "Move RPC failed: " << status.error_message() << "\n";
        }
    }

private:
    std::unique_ptr<GameService::Stub> stub_;
};

int main() {
    GameClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
    client.Move(1, 100, 200);
    return 0;
}