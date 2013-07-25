
#ifndef _ACTIONS_HPP_
#define _ACTIONS_HPP_

namespace SphereSim{
	
	/*
	 * enum Actions:
	 * Lists actions provided by the server and requested by the clients. 
	 * Actions are used for TCP communication between clients and server.
	 */
	namespace Actions{
		enum Action{
			getVersion
		};
	}
	
}

#endif
