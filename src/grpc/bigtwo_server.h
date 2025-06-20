#pragma once

#include "bigtwo.grpc.pb.h"
#include "game/state/game_state_data.h"

#include <grpcpp/grpcpp.h>

namespace state = rsosor::game::state;
namespace gen = rsosor::generated;

namespace rsosor {
namespace grpc_api {

  void RunGrpcServer(const std::string& grpc_addr);
  

  class BigTwoServiceImpl final : public gen::BigTwoService::Service {
  public:
    state::GameStateData game_data;

    grpc::Status StartGame(grpc::ServerContext* context,
                      const gen::StartGameRequest* request,
                      gen::StartGameResponse* response) override;

    grpc::Status JoinGame(grpc::ServerContext* context,
                          const gen::JoinRequest* request,
                          gen::JoinResponse* response) override;

    grpc::Status PlayCards(grpc::ServerContext* context,
                          const gen::PlayRequest* request,
                          gen::PlayResponse* response) override;

    grpc::Status Pass(grpc::ServerContext* context,
                      const gen::PassRequest* request,
                      gen::PassResponse* response) override;

    grpc::Status GetGameState(grpc::ServerContext* context,
                              const gen::GameStateRequest* request,
                              gen::GameStateResponse* response) override;

    grpc::Status Chat(grpc::ServerContext* context,
                      const gen::ChatRequest* request,
                      gen::ChatResponse* response) override;
  };

  class BigTwoTableServiceImpl final : public gen::BigTwoTableService::Service {
  public:
    grpc::Status Deal(grpc::ServerContext* context,
                      const gen::DealRequest* request,
                      gen::DealResponse* response) override;

    grpc::Status ValidatePlay(grpc::ServerContext* context,
                              const gen::PlayValidationRequest* request,
                              gen::PlayValidationResponse* response) override;

    grpc::Status HandleTimeout(grpc::ServerContext* context,
                              const gen::TimeoutRequest* request,
                              gen::TimeoutResponse* response) override;

    grpc::Status Score(grpc::ServerContext* context,
                      const gen::ScoreRequest* request,
                      gen::ScoreResponse* response) override;
  };

  class BigTwoManagementServiceImpl final : public gen::BigTwoManagementService::Service {
  public:
      grpc::Status GetRanking(grpc::ServerContext* context,
                              const gen::RankingRequest* request,
                              gen::RankingResponse* response) override;
    
      grpc::Status GetPlayerWinRate(grpc::ServerContext* context,
                                    const gen::PlayerRateRequest* request,
                                    gen::PlayerRateResponse* response) override;
    
      grpc::Status GetActivityLog(grpc::ServerContext* context,
                                  const gen::ActivityLogRequest* request,
                                  gen::ActivityLogResponse* response) override;
  };

} // grpc_api
} // rsosor