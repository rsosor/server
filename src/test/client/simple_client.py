# SimpleClient.py
import socket

HOST = 'localhost'
PORT = 12345
NAME = 'Alice'
# NAME = 'LOGIN Alice letmein\n'

with socket.create_connection((HOST, PORT)) as s:
    s.sendall(NAME.encode('utf-8'))
    print(f"Sent: {NAME}")
    # s.shutdown(socket.SHUT_WR)  # 不再傳送資料
    # s.shutdown(socket.SHUT_RD)  # 不再接收資料
    # s.shutdown(socket.SHUT_RDWR)  # 關閉兩邊