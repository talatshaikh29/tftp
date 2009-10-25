#ifndef TFTPSERVER
#define TFTPSERVER

#include "main.h"
#include "../client/tftp_packet.h"

#define TFTP_SERVER_MAX_CLIENTS 10

#define TFTP_SERVER_REQUEST_UNDEFINED 0
#define TFTP_SERVER_REQUEST_READ 1
#define TFTP_SERVER_REQUEST_WRITE 2

#define TFTP_SERVER_CLIENT_NOT_CONNECTED 0
#define TFTP_SERVER_CLIENT_CONNECTED 1

using namespace std;

class TFTPServer {

  public:

	struct ServerClient {

		ServerClient() {

			request = TFTP_SERVER_REQUEST_UNDEFINED;
			connected = TFTP_SERVER_CLIENT_NOT_CONNECTED;

		}

		int connected;						//- prisijunges | neprisijunges
		int request;						//- prisijungusio kliento request tipas
		int block;							//- paketu numeracija
		int temp;							//- del winsock/recv() implementacijos

		char* ip;							//- ip adresas, pranesimu apie klienta rodymui

		FD_SET set;							//- fd sarasas, kuriuose yra duomenu
		SOCKET client_socket;				
		sockaddr_in address;

		ifstream* file_rrq;
		ofstream* file_wrq;

		TFTP_Packet last_packet;			//- kadangi siunciame, kai gauname ACK, tai reikia tik 1 last packeta saugoti
											//- The sender has to keep just one packet on hand for retransmission, since
											//- the lock step acknowledgment guarantees that all older packets have
											//- been received.

	} clients[TFTP_SERVER_MAX_CLIENTS];


	TFTPServer(int port, char* ftproot);
	~TFTPServer();

  private:

    int server_port;
	char* server_ftproot;

    SOCKET server_socket;
    sockaddr_in server_address;
    int listener;

  protected:

    int sendBuffer(char *);

	bool sendPacket(TFTP_Packet* packet, int client_socket);

	bool waitForPacket(TFTP_Packet* packet, int current_client_socket, int timeout_ms);
	bool waitForPacketACK(int packet_number, int timeout_ms);

	void acceptClients();
	bool acceptClient(ServerClient* client);

	bool receivePacket(ServerClient* client);

	bool sendError(ServerClient* client, int error_no, char* custom_message = "");
	bool disconnectClient(ServerClient* client);

	void clientStatus(ServerClient* client, char* message);

};

class ETFTPSocketBind: public std::exception {
  virtual const char* what() const throw() {
    return "Unable to bind to an address";
  }
};

class ETFTPSocketCreate: public std::exception {
  virtual const char* what() const throw() {
    return "Unable to create a socket";
  }
};

class ETFTPSocketInitialize: public std::exception {
  virtual const char* what() const throw() {
    return "Unable to find socket library";
  }
};

class ETFTPSocketListen: public std::exception {
  virtual const char* what() const throw() {
    return "Unable to start listening for incoming connectinos";
  }
};

#endif