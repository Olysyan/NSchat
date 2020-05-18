#include <breep/util/serialization.hpp>
#include <breep/network/tcp.hpp>
#include <chrono>
#include <future>
#include <iostream>

BREEP_DECLARE_TYPE(std::string)

struct name {
	name() : name_() {}

	name(const std::string& val)
			: name_(val)
	{}

	std::string name_;

	BREEP_ENABLE_SERIALIZATION(name, name_)
};
BREEP_DECLARE_TYPE(name)

class chat_manager {
public:
	chat_manager(const std::string& name)
			: m_name(name)
			, m_nicknames()
	{}

	void connection_event(breep::tcp::network& network, const breep::tcp::peer& peer) {
		if (peer.is_connected()) {
			network.send_object_to(peer, m_name);
		} else {
			std::cout << m_nicknames.at(peer.id()) << " disconnected." << std::endl;
		}
	}

	void name_received(breep::tcp::netdata_wrapper<name>& dw) {
		m_nicknames.insert(std::make_pair(dw.source.id(), dw.data.name_));
		std::cout << dw.data.name_ << " connected." << std::endl;
	}

	void message_received(breep::tcp::netdata_wrapper<std::string>& dw) {
		std::cout << m_nicknames.at(dw.source.id()) << ": " << dw.data << std::endl;
	}

private:
	name m_name;
	std::unordered_map<boost::uuids::uuid, std::string,  boost::hash<boost::uuids::uuid>> m_nicknames;
};

int main(int argc, char* argv[]) {
	if (argc != 2 && argc != 4) {
		std::cerr<< "Usage: " << argv[0] << " <hosting port> [<target ip> <target port>]\n";
		return 1;
	}

	std::string nick;
	std::cout << "Enter your nickname: ";
	std::getline(std::cin, nick);

	chat_manager chat(nick);

	breep::tcp::network network(std::atoi(argv[1]));

	network.add_data_listener<name>([&chat](breep::tcp::netdata_wrapper<name>& dw) -> void {
		chat.name_received(dw);
	});
	network.add_data_listener<std::string>([&chat](breep::tcp::netdata_wrapper<std::string>& dw) -> void {
		chat.message_received(dw);
	});

	network.add_connection_listener([&chat](breep::tcp::network& net, const breep::tcp::peer& peer) -> void {
		chat.connection_event(net, peer);
	});

	network.add_disconnection_listener([&chat](breep::tcp::network& net, const breep::tcp::peer& peer) -> void {
		chat.connection_event(net, peer);
	});

	if (argc == 2) {
		network.awake();
	} else {
		if(!network.connect(boost::asio::ip::address::from_string(argv[2]), std::atoi(argv[3]))) {
			std::cerr << "Connection failed.\n";
			return 1;
		}
	}

	std::string message;
	std::getline(std::cin, message);
	while (message != "/q") {
		network.send_object(message);
		std::getline(std::cin, message);
	}

	network.disconnect();
	return 0;
}