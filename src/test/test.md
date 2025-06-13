# cl

常用選項:
- /c：僅編譯但不鏈接，生成 .obj 檔案。
- /Ox：最快編譯方式。
- /EHa：
  - 啟用 C++ 異常處理
  - 允許拋出和捕獲 C++ 異常
  - 默認的異常處理模型
  - 當與 /clr (Common Language Runtime) 的選項一起使用時，它可以處理託管程式中的 C++ 異常
- /EHsc：
  - 類似 /EHa，啟用 C++ 異常處理
  - 允許拋出和捕獲 C++ 異常
  - 在某些情況下，與 /EHa 相比，可能會產生更優化的程式
  - 通常用於提高性能，尤其是在不需要經常拋出異常的情況下
- /EHs：
  - 啟用 C++ 異常處理
  - 允許拋出和捕獲 C++ 異常
  - 當與 /clr 選項一起使用時，意味著程式在託管和非託管程式之間切換，並且需要支持 C++ 處理異常
  - 如果與 /clr 一起使用，那麼 C++ 異常會被轉換成託管異常
- /EHs-: 
  - 禁止 C++ 異常處理
  - 如果在此模式下嘗試拋出異常，程式將終止
- /I：添加包含文件目錄。
- /L：指定庫檔案。
- /link：使用link.exe 鏈接。
- /o：指定輸出檔名。
- /d：啟用除錯資訊。
- /Zi：啟用除錯資訊。

## test/client

simple_client.cpp
~~~
g++ simple_client.cpp -o client.exe -lws2_32
~~~
- MinGW

~~~
cl simple_client.cpp /Fe:client.exe /MD /link ws2_32.lib
~~~
- MSVC

simple_client.py
~~~
python simple_client.py
~~~

Test-NetConnection（powershell 內建，不能傳送資料）
~~~
Test-NetConnection -ComputerName localhost -Port 12345
~~~

## proto

~~~
protoc --proto_path=src/proto --cpp_out=src/generated --grpc_out=src/generated --plugin=protoc-gen-grpc=C://Users/godpk/vcpkg/installed/x64-windows/tools/grpc/grpc_cpp_plugin.exe bigtwo.proto
~~~

## grpc 注意事項

官方 gRPC 文件提到：
- Using gRPC C++ as a DLL is not recommended, but you can enable it with -DBUILD_SHARED_LIBS=ON.

原因主要是：
- gRPC 的 DLL 版本在 Windows 下容易遇到穩定性、相依性問題
- 尤其是跨多個 DLL 共享 CRT（C Runtime）可能導致潛在問題
- Debug/Release 版本間 DLL 相容性問題

## test/client/grpc

~~~
| 編譯器選項 | 意義                             | 結果                     |
| ----------|----------------------------------|-------------------------|
| `/MD`     | 使用 **動態連結** 的 MSVC runtime | 需要依賴 `MSVCRT.dll` 等 |
| `/MT`     | 使用 **靜態連結** 的 MSVC runtime | 所有 runtime 都進 exe 裡 |
~~~

靜態連結 bigtwo_client.cpp
~~~
cl /std:c++23 /EHsc /MT src/test/client/grpc/bigtwo_client.cpp src/generated/bigtwo.pb.cc src/generated/bigtwo.grpc.pb.cc /Isrc /Isrc/generated /IC:\Users\godpk\vcpkg\installed\x64-windows-static\include /DPROTOBUF_USE_DLLS=0 /DGRPC_USE_DLLS=0 /DABSL_FORCE_INTERNAL_IMPLEMENTATION=1 /Fe:src/test/client/grpc /link /LIBPATH:C:\Users\godpk\vcpkg\installed\x64-windows-static\lib grpc++.lib grpc.lib gpr.lib libprotobuf.lib libprotoc.lib absl_synchronization.lib absl_time.lib absl_strings.lib absl_status.lib absl_base.lib ws2_32.lib
~~~
- fatal error LNK1120: 414 個無法解析的外部符號

動態連結 bigtwo_client.cpp（無 absl_synchronization.lib、absl_time.lib、absl_strings.lib、absl_status.lib、absl_base.lib）
~~~
cl /std:c++23 /EHsc /MD src/test/client/grpc/bigtwo_client.cpp src/generated/bigtwo.pb.cc src/generated/bigtwo.grpc.pb.cc /Isrc /Isrc/generated /IC:\Users\godpk\vcpkg\installed\x64-windows\include /DPROTOBUF_USE_DLLS /DGRPC_USE_DLLS /Fe:src/test/client/grpc /link /LIBPATH:C:\Users\godpk\vcpkg\installed\x64-windows\lib grpc++.lib grpc.lib gpr.lib libprotobuf.lib libprotoc.lib ws2_32.lib
~~~
- fatal error LNK1120: 455 個無法解析的外部符號