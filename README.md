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
遊戲管理	        支援配對、房間、洗牌、出牌規則、遊戲流程
資料同步	        玩家之間同步手牌、回合狀態、勝負結果
多人支援	        支援多桌同時進行
並發處理	        多個遊戲房間需並行運行
安全性與防作弊	基本檢查非法行為（例如出錯牌）與封包驗證
擴充性	        易於未來加入新規則、排行榜、帳號系統、聊天室等
~~~

# Architecture

~~~
[ Client ] <--> [ Server Core ]
                      |
        +-------------+-------------+
        |             |             |
  [ boost/beast ]  [ game ]      [ gRPC ]
        |             |             |
       tls         玩家狀態      內部通訊
        |            手牌     
  http、websocket    順序
                     規則
~~~

# Technology

~~~
功能	        推薦工具	                            原因說明
網路通訊	        boost.asio/beast / uWebSockets	    高效、跨平台、異步非阻塞
多執行緒	        boost.thread / std::thread          控制並行的遊戲房間
JSON        	nlohmann/json	                    header-only、易於整合
遊戲邏輯	        自行實作	                            自訂規則
封包加密      	OpenSSL             	            防止簡易封包偽造
測試	        googletest / Catch2                 單元測試
序列化	        FlatBuffers / Protobuf              若需要高效或二進位封包傳輸
日誌            spdlog                              高性能、header-only、格式靈活、支援多線程安全
~~~

# Flow

~~~
1.  玩家連線並通過身份驗證（token 與密碼檢查）
2.  玩家登入伺服器，初始化玩家資料並分配唯一 ID
3.  將玩家加入配對池以等待匹配對手
4.  配對系統根據條件（如等級、地域）匹配合適玩家
5.  配對成功後建立遊戲房間，初始化牌局與玩家狀態
6.  透過伺服器發牌給所有玩家，並同步初始手牌狀態
7.  玩家輪流出牌，伺服器即時檢查牌型合法性及回合順序
8.  當玩家操作不當或超時，伺服器自動執行跳過或自動出牌機制
9.  伺服器同步遊戲狀態給所有房間內玩家，保持資料一致
10. 任一玩家出完手牌時，伺服器計算最終排名並廣播遊戲結束訊息
11. 遊戲結果保存至資料庫，供排行榜或回放使用
12. 玩家可選擇重玩、離開房間或返回大廳等待下一場遊戲
~~~

# Extension & Optimization

- 資料持久化：儲存玩家勝率、牌局記錄
- 排行榜系統：加總統計數據廣播至所有人
- 配對系統：可自選進入房間而非自動配對

# 遊戲伺服器要素

## 連線管理

- 玩家身份認證（例如密碼、Token）與授權
  - tcp 網路管理
  - 密碼 / token 認證（目前玩家只是輸入名字，沒有驗證）
  - 玩家授權（分辨合法 / 非法玩家）
  - 儲存完整玩家資訊（例如 IP、連線時間）
  - 授權後的身份狀態維護
- 玩家連接與斷線通知（上/下線）

## 遊戲邏輯完整性

- 正確處理遊戲流程規則（例如：輪流出牌、結算、勝負判定）
- 多種遊戲模式與擴充性

## 指令與事件處理

- 接收並解析玩家指令（出牌、摸牌、聊天等）
- 驗證指令合法性（ex：是否輪到該玩家出牌，出牌是否合理）
- 回應成功或錯誤訊息給客戶端

## 遊戲狀態同步

- 正確維護遊戲狀態（回合、玩家手牌、牌堆等）
- 廣播狀態更新給所有玩家
- 具備狀態恢復功能，支援重連

## 錯誤處理

- 處理異常行為（如斷線、換設備、非法操作）

## 日誌紀錄

- 方便除錯和追蹤

## 性能與擴展

- 多線程或非同步處理來支援多個遊戲房間和玩家
- 伺服器資源管理與監控

## 測試

- 單元測試
- 整合測試
- 腳本測試
