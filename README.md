# swinsock

swinsock (Simple Windows socket service) is a simple implementation of socket
service on Windows system.

This work is based on [Windows Sockets API (WSA)][wsa], which supports both TCP and UDP protocol,
it can also play the role of both server and client. When the connection between
remote client/server is ready, swinsock ONLY acts as **receiver**

[wsa]: https://docs.microsoft.com/zh-tw/windows/win32/winsock/about-winsock

## Usage

swinsock options are listed below:

```shell
Simple Windows socket service (swinsock)
Usage: swindock [OPTION]

Main options:
-h              show help
-v              show version
-s              enable file save
-t [s/c]        set socket type to TCP server(s)/client(c)
-u [s/c]        set socket type to UDP server(s)/client(c)
-i addr         specify remote server address
-p port         specify socket port number
```

- When socket type is TCP/UDP client, users **MUST** specify remote IP address by
using ```-i addr``` option
- Use ```-s``` option can enable file saving function, the file naming rule is listed
below:

    ```shell
    File name: [sock_type]_[service_type]_[hh]_[mm]_[ss]

    [sock_type]: tcp or udp
    [service_type]: server or client
    [hh]_[mm]_[ss]: local time representation
    ```

## Example

Initial swinsock as ``TCP client`` and start receive data from ``192.168.7.89``
port ``8989``

```shell
jasper@Jasper-PC:/mnt/d/$ ./swinsock.exe -t c -p 8989 -i 192.168.7.89 -s
Select TCP Client
Initializing Winsock...
filename = tcp_client_13_34_18
TCP client Ready to reveive data
Receive 97 bytes
Receive 27740 bytes
Receive 29200 bytes
Receive 47001 bytes
Receive 65700 bytes
Receive 7300 bytes
Receive 8729 bytes
Receive 21891 bytes
Receive 20424 bytes
Receive 28889 bytes
Interrupt signal (2) received.
Receive 41467 bytes
```
