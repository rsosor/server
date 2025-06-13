# cmake & compile

~~~
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE --no-warn-unused-cli -S . -B build -G "Visual Studio 17 2022" -T host=x64 -A x64 -DCMAKE_TOOLCHAIN_FILE=C://Users/godpk/vcpkg/scripts/buildsystems/vcpkg.cmake

msbuild build\你的方案檔.sln /p:Configuration=Release
cmake --build build --config Release
~~~
- MSVC + MSBuild | 通用
~~~
cmake -B build -S . -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=C:/Users/godpk/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_C_COMPILER=C:/ProgramData/mingw64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/ProgramData/mingw64/bin/g++.exe -DVCPKG_TARGET_TRIPLET=x64-mingw-static

mingw32-make
~~~
- MinGW

# gRPC

https://grpc.io/docs/languages/cpp/quickstart/

# Coding Style

自由函式
- 數學計算（sin, cos, sqrt, clamp, dot）
- 圖像處理中的轉換（如 RGB to HSV）
- 文件處理：讀檔轉字串
- 單純演算法：排序、搜尋

這些函式不需要物件來記住前後狀態，甚至設計上就希望避免副作用（Side Effect）。

# 常見需求整理

~~~
類型	        需求描述
連線管理	        玩家可加入/離開房間，維持連線、心跳、重連
遊戲管理	        支援配對、房間、洗牌、出牌規則、大老二遊戲流程
資料同步	        玩家之間同步手牌、回合狀態、勝負結果
多人支援	        支援多桌同時進行，每桌 4 位玩家
並發處理	        多個遊戲房間需並行運行
安全性與防作弊	基本檢查非法行為（例如出錯牌）與封包驗證
擴充性	        易於未來加入排行榜、帳號系統、聊天室等
~~~

# 架構設計概覽

~~~
[ Client ] <--> [ Server Core ]
                      |
        +-------------+-------------+
        |             |             |
  [ Connection ]  [ Game Room ]  [ Matchmaking ]
        |             |
  asio / WebSocket    |---> 玩家狀態、手牌、順序、回合邏輯
                      |
                出牌合法判斷、大老二邏輯
~~~

# 技術選型

~~~
功能	        推薦工具	                        原因說明
網路通訊	        [Boost.Asio] 或 [uWebSockets]	高效、跨平台、非阻塞
JSON 封包格式	nlohmann/json	                header-only、易於整合
多執行緒	        C++17 thread / task pool	    控制並行的遊戲房間
遊戲邏輯	        自行實作	                        需自訂出牌規則
封包加密（可選）	OpenSSL 或簡單 checksum	        防止簡易封包偽造
測試	        Catch2	                        單元測試與斷言
序列化（可選）	FlatBuffers / Protobuf	        若需要高效或二進位封包傳輸
~~~

# 伺服器實作概念模組

- Connection Layer（Boost.Asio TCP）
  - 處理連線、接收與解析封包、轉發到遊戲邏輯：
- Matchmaking / Lobby
  - 將玩家自動配對進遊戲房間：
- Game Room / Engine
  - 管理一桌的玩家、狀態、出牌邏輯：
- Card Engine（自訂）
  - 實作大老二的出牌判斷邏輯：
- 封包格式設計（JSON）

~~~
// Client -> Server
{
  "type": "play_card",
  "cards": ["3D", "3S"]
}

// Server -> Client
{
  "type": "update_state",
  "player_id": 2,
  "played_cards": ["3D", "3S"]
}
~~~

# 示例遊戲流程（簡化）

~~~
1. 玩家連線 → 傳送登入資訊
2. 加入配對池（Matchmaking）
3. 四人配對成功 → 建立 GameRoom → 發牌
4. 玩家輪流出牌（Server 判斷是否合法）
5. 有人出完 → Server 廣播結束與名次
~~~

# 可能需要自我實作的部分

~~~
模組	        原因
出牌合法性判斷	大老二牌型多、需要客製
遊戲狀態管理	    輪到誰出牌、跳過、清場等
封包路由	        因應多玩家同時處理
玩家同步	        廣播 vs 單播邏輯
安全檢查	        防止非法出牌與作弊
~~~

# 延伸與優化建議

- 支援 WebSocket：若前端使用瀏覽器，建議用 uWebSockets 或 Boost.Beast
- 資料持久化：儲存玩家勝率、牌局記錄，可用 SQLite 或簡易 JSON
- 排行榜系統：Server 加總統計數據廣播至所有人
- 房間系統：可自選進入房間而非自動配對

# 遊戲伺服器應該具備的基本要素

## 連線管理

- Todo
  - 玩家身份認證（例如密碼、Token）與授權
    - 連線底層的網路管理（用 TCP 或 WebSocket）
    - 密碼 / Token 認證（目前玩家只是輸入名字，沒有驗證）
    - 玩家授權（尚未分辨合法/非法玩家）
    - 儲存完整玩家資訊（例如 IP、連線時間）
    - 授權後的身份狀態維護（還沒定義 Player struct/class 來儲存）
      - ✅ 定義 Player 類別：包含名稱、ID、IP、登入時間、是否通過驗證等
      - ✅ 在連線後要求玩家輸入格式為：LOGIN \<username> \<token>
      - ✅ 驗證格式與合法性（token 暫時可以是硬編碼測試）
      - ✅ 建立 ConnectionManager 或 PlayerRegistry 管理登入玩家
      - ✅ 若驗證成功 → 加入遊戲房間等待區（之後再啟用 matchmaking）
      - ✅ 若驗證失敗 → 關閉連線，提示「身份驗證失敗」
- Complete
  - 玩家連接與斷線通知（誰連上/誰掉線）

## 遊戲狀態同步

- Todo
  - 正確維護遊戲狀態（回合、玩家手牌、牌堆等）
  - 廣播狀態更新給所有玩家（目前只是單人 Welcome 回應）
  - 具備狀態恢復功能，支援重連（尚未記錄玩家狀態或做身份綁定）
- Complete

## 指令與事件處理

- Todo
  - 接收並解析玩家指令（出牌、摸牌、聊天等）
  - 驗證指令合法性（例如：是否輪到該玩家出牌，出牌是否合理）
  - 回應成功或錯誤訊息給客戶端
- Complete

## 錯誤處理

- Todo
  - 處理異常行為（如非法操作）
  - 日誌紀錄，方便除錯和追蹤
- Complete

## 遊戲邏輯完整性

- Todo
  - 正確處理遊戲流程規則（例如：輪流出牌、結算、勝負判定）
  - 多種遊戲模式與擴充性
- Complete

## 性能與擴展

- Todo
  - 多線程或非同步處理來支援多個遊戲房間和玩家
  - 伺服器資源管理與監控
- Complete

## 簡單輸出和測試 vs 真實伺服器的差異

- Todo
  - 現在的輸出只有確認流程有跑起來，沒有顯示玩家詳細資訊或錯誤狀況。
  - 真正伺服器要做到玩家互動，必須有更多上下文訊息和狀態同步。
  - 可能還需要用網路通訊（TCP/UDP/WebSocket等）將資料傳給玩家，而不是只在伺服器端 std::cout。
- Complete

## 遊戲伺服器，其他建議

- Todo
  - 設計玩家狀態結構，包含玩家ID、名稱、連線狀態等。
  - 設計事件系統，把玩家命令轉成事件，並且廣播狀態改變。
  - 用網路通訊框架（如 Boost.Asio、asio、或 WebSocket lib）做訊息收發。
  - 強化錯誤與異常處理，防止非法行為或網路錯誤導致伺服器崩潰。
  - 加入日誌系統，記錄玩家行為與錯誤資訊。
  - 編寫測試用例，確保遊戲邏輯正確。
- Complete