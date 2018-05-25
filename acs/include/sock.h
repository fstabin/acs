#pragma once
#if _WIN32

#include <utility>
#include <memory>
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "attribute.h"

#define DEFAULT_PORT "27015"

namespace acs {

	namespace sock {		

		//アプリのインスタンスごとに一つだけ生成すること
		class SockModule: public acs::CopyDisable  {
			bool mIsEnable = false;
		public:
			SockModule & operator=(SockModule&&) = default;
			SockModule(SockModule&&) = default;

			SockModule() {
				WSAData data;
				mIsEnable = (0 == WSAStartup(WINSOCK_VERSION, &data));
			}
			~SockModule() {
				WSACleanup();
			}
		
			bool isEnable() {
				return mIsEnable;
			}
		};

		class SockBase : public acs::CopyDisable {
			bool mIsEnable = false;
			SOCKET mhSock = INVALID_SOCKET;
		protected:
			void enableSockBase(SOCKET s) {
				mIsEnable = true;
				mhSock = s;
			}
		public:
			SockBase & operator=(SockBase&&) = default;
			SockBase(SockBase&&) = default;

			SockBase() {}
			~SockBase() {
				shutdown();
				closesocket(mhSock);
			}

			bool send(const char buf[], int bufLen) {
				return (SOCKET_ERROR != ::send(mhSock, buf, bufLen, 0));
			}

			bool shutdown() {
				return(SOCKET_ERROR !=::shutdown(mhSock, SD_SEND));
			}

			int recv(char buf[], int bufLen) {
				int iResult = ::recv(mhSock, buf, bufLen, 0);
				if (iResult < 0) {
#ifdef _DEBUG
					std::cout << "[ERROR]::recv failed\nIn ClientSocket::recv\nWSACode = " << WSAGetLastError() << std::endl;
#endif
					mIsEnable = false;
				}
				return iResult;
			}

			bool isEnable() const {
				return mIsEnable;
			}
		};

		//クライアント側の通信用
		class ClientSocket : public SockBase, public acs::CopyDisable {
		public:
			ClientSocket & operator=(ClientSocket&&) = default;
			ClientSocket(ClientSocket&&) = default;

			ClientSocket(const char* serverName) {
				SOCKET ConnectSocket = INVALID_SOCKET;
				struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;
				int iResult;

				ZeroMemory(&hints, sizeof(hints));
				hints.ai_family = AF_UNSPEC;
				hints.ai_socktype = SOCK_STREAM;
				hints.ai_protocol = IPPROTO_TCP;

				// Resolve the server address and port
				iResult = getaddrinfo(serverName, DEFAULT_PORT, &hints, &result);
				if (iResult != 0)return;

				// Attempt to connect to an address until one succeeds
				for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

					// Create a SOCKET for connecting to server
					ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
						ptr->ai_protocol);
					if (ConnectSocket == INVALID_SOCKET)continue;

					// Connect to server.
					iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
					if (iResult == SOCKET_ERROR)continue;
					break;
				}

				freeaddrinfo(result);

				if (ConnectSocket == INVALID_SOCKET)return;
				enableSockBase(ConnectSocket);
			}
		};

		//サーバ側のクライアントとの通信用ソケット
		class ServerClientSocket : public SockBase, public acs::CopyDisable {
		public:
			ServerClientSocket & operator=(ServerClientSocket&&) = default;
			ServerClientSocket(ServerClientSocket&&) = default;

			ServerClientSocket(SOCKET sock) {
				SockBase::enableSockBase(sock);
			}
		};

		//サーバ側の通信設立用
		class ServerSocket : public acs::CopyDisable {
			bool mIsEnable = false;
			SOCKET mhSock = INVALID_SOCKET;
		public:
			ServerSocket & operator=(ServerSocket&&) = default;
			ServerSocket(ServerSocket&&) = default;

			ServerSocket(){
				int iResult;

				SOCKET ClientSocket = INVALID_SOCKET;

				struct addrinfoDetach {
					void operator() (addrinfo* r) noexcept {
						if (r != NULL)freeaddrinfo(r);
					}
				};

				std::unique_ptr<addrinfo, addrinfoDetach> result;
				struct addrinfo hints;

				int iSendResult;

				ZeroMemory(&hints, sizeof(hints));
				hints.ai_family = AF_INET;
				hints.ai_socktype = SOCK_STREAM;
				hints.ai_protocol = IPPROTO_TCP;
				hints.ai_flags = AI_PASSIVE;

				// Resolve the server address and port
				addrinfo* tresult;
				iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &tresult);
				if (iResult != 0) return;
				result = std::unique_ptr<addrinfo, addrinfoDetach>(tresult);

				// Create a SOCKET for connecting to server
				mhSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
				if (mhSock == INVALID_SOCKET) return;

				// Setup the TCP listening socket
				iResult = bind(mhSock, result->ai_addr, (int)result->ai_addrlen);
				if (iResult == SOCKET_ERROR) return;

				result.release();
				mIsEnable = true;
			}
			~ServerSocket() {
				closesocket(mhSock);
			}

			bool listen() {
				return (SOCKET_ERROR !=::listen(mhSock, SOMAXCONN));
			}

			ServerClientSocket accept() {
				return ServerClientSocket(::accept(mhSock, NULL, NULL));
			}

			bool isEnable() const {
				return mIsEnable;
			}
			
		};

	}

}
#else 
namespace acs {

	namespace sock {

		static_assert("acs::sock がサポートされていない環境です");
	}

}
#endif
