#pragma once

#include "bigtwo.grpc.pb.h"
#include "game/state/game_state_data.h"

#include <grpcpp/grpcpp.h>

#include <queue>

namespace state = rsosor::game::state;
namespace gen = rsosor::generated;

namespace rsosor {
namespace grpc_api {

  void RunGrpcServer(const std::string& grpc_addr);

  struct Subscriber {
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<gen::GameState> queue;
    bool active = true;
  };

  struct RoomInfo {
    std::mutex mtx;
    std::vector<std::shared_ptr<Subscriber>> subscribers;
  };

  struct RoomRegistry {
    std::mutex mutex;
    std::unordered_map<std::string, std::vector<int>> room_to_players;

    std::mutex hand_mutex;
    std::unordered_map<int, std::vector<gen::Card>> player_hands; // player_id -> 玩家手牌（Card 列表）

    std::mutex room_mutex;
    std::map<std::string, RoomInfo> game_rooms;
  };

  // inline RoomRegistry global_room_registry;  // 放在 *.h
  extern RoomRegistry global_room_registry;

  class BigTwoServiceImpl final : public gen::BigTwoService::Service {
  public:
    state::GameStateData game_data;

    grpc::Status Login(grpc::ServerContext* context,
                      const gen::LoginRequest* request,
                      gen::LoginResponse* response) override;

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
  private:
    int current_turn_player_id_ = -1;
    std::string current_phase_;
    std::vector<gen::Card> last_played_cards_;
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

    grpc::Status SubscribeGameState(grpc::ServerContext* context,
        const gen::GameRoomId* request,
        grpc::ServerWriter<gen::GameState>* writer);
  private:
    // 你還可以寫個推播用的成員函式
    void BroadcastGameState(const std::string& room_id, const gen::GameState& state);
    std::vector<int> getAllPlayersInRoom(const std::string& room_id);
    int cnt = 0;
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