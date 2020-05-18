#include <breep/util/serialization.hpp>
#include <breep/network/tcp.hpp>
#include <chrono>
#include <future>
#include <iostream>

using namespace std::literals::chrono_literals;

BREEP_DECLARE_TYPE(std::string)

void co_listener(breep::tcp::network &network, const breep::tcp::peer &source) {
  std::string message;
  std::cout << "Enter message to send>> ";
  std::cin >> message;
  network.send_object_to(source, message);
}

void data_listener(breep::tcp::netdata_wrapper<std::string> &dw) {
  std::cout << dw.data << std::endl;
  dw.network.disconnect();
}

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

int main() {
  int number;
  std::cout << "Enter the number of people>> ";
  std::cin >> number;

  int arr[number];

    for (int i = 0; i < number; i++)
    {
    arr[i]=i+1234;  
	breep::tcp::network network(arr[i]);
    }
  std::string nick;
  std::cout << "Enter your nickname: ";
  std::getline(std::cin, nick);
 
 
  chat_manager chat(nick);

 
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

	if (number == 2) {
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