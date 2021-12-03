#include <string>
#include "Socket.h"
#include "Messages.h"




class OutSocket: public Socket {
public:
	OutSocket() {}
	~OutSocket() {}

	bool Init(std::string ip, int port);
	
};
