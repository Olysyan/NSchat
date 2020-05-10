#include "lib.hpp"
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

int main() {
  std::cout << "\nEnter listening port>> ";
  unsigned short my_port, out_port;
  std::cin >> my_port;
  std::cout << "\nEnter outgoing port>> ";
  std::cin >> out_port;
  breep::tcp::network network(my_port);
  network.set_log_level(breep::log_level::none);
  network.add_connection_listener(&co_listener);
  network.awake();
  std::string data = " ";
  network.add_data_listener<std::string>(&data_listener);
  bool result = 0;
  do {
    result = network.connect(boost::asio::ip::address_v4::loopback(), out_port);
    std::cout << "retrying in...  1 sec\n";
    std::this_thread::sleep_for(1s);
    if (result) {
      network.join();
      network.disconnect();
      result = 0;
    }
  } while (!result);
}