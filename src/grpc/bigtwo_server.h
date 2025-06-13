#pragma once

#include "bigtwo.grpc.pb.h"

#include <grpcpp/grpcpp.h>

class GameServiceImpl final : public bigtwo::BigTwoService::Service {
public:
  grpc::Status JoinGame(grpc::ServerContext* context,
                        const bigtwo::JoinRequest* request,
                        bigtwo::JoinResponse* response) override;

  grpc::Status PlayCards(grpc::ServerContext* context,
                        const bigtwo::PlayRequest* request,
                        bigtwo::PlayResponse* response) override;

  grpc::Status Pass(grpc::ServerContext* context,
                   const bigtwo::PassRequest* request,
                   bigtwo::PassResponse* response) override;

  grpc::Status GetGameState(grpc::ServerContext* context,
                            const bigtwo::GameStateRequest* request,
                            bigtwo::GameStateResponse* response) override;

  grpc::Status Chat(grpc::ServerContext* context,
                    const bigtwo::ChatRequest* request,
                    bigtwo::ChatResponse* response) override;
};

void RunGrpcServer();