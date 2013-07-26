
#ifndef _ACTIONS_HPP_
#define _ACTIONS_HPP_

namespace SphereSim{
	
	/*
	 * namespace ActionGroups:
	 * Lists action groups provided by the server and requested by the clients.
	 * Actions are used for TCP communication between clients and server.
	 */
	namespace ActionGroups{
		enum Group{
			basic = 1
		};
	}
	
	/*
	 * namespace BasicActions:
	 * Basic actions concerning client and server software.
	 */
	namespace BasicActions{
		enum Action{
			getVersion = 1,
			getTrueString,
			terminateServer
		};
	}
	
}

#endif
