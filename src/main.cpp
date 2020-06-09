#include "lib.hpp"

bool flag = true;
std::string ans;
bool g_accept = false;
bool g_accept1 = false;

int main(int argc, char *argv[]) {
  std::string str;
  std::string str4;
  std::string str1;
  std::string name;
  std::string str2;
  do {
    nana::form fo;
    fo.caption("NSchat");
    nana::textbox usr{fo}, port{fo}, target_ip{fo}, target_port{fo};
    nana::button login{fo, "Login"}, cancel{fo, "Cancel"};
    usr.tip_string("User:").multi_lines(false);
    port.tip_string("Port:").multi_lines(false);
    target_ip.tip_string("Target ip:").multi_lines(false);
    target_port.tip_string("Target port:").multi_lines(false);
    // Define a place for the form.
    nana::place plc{fo};
    // Divide the form into fields
    login.events().click([&login, &fo, &name, &usr, &port, &argv, &str1, &str,
                          &target_ip, &target_port, &str2] {
      name = usr.text();
      str = port.text();
      str1 = target_ip.text();
      str2 = target_port.text();
      fo.close();
    });
    cancel.events().click([&fo] {
      fo.close();
      g_accept = true;
    });
    if (g_accept) {
      return 1;
    }

    // plc.div("margin= 10%   gap=20 vertical< weight=70 gap=20 vertical
    // textboxs arrange=[25,25]> <min=20> <weight=25 gap=10 buttons>  > ");
    plc.div("<><weight=80% vertical<><weight=70% vertical <vertical gap=10 "
            "textboxs arrange=[25,25]>  <weight=25 gap=10 buttons> ><>><>");
    // Insert widgets
    // The field textboxs is vertical, it automatically adjusts the widgets' top
    // and height.
    plc.field("textboxs") << usr << port << target_ip << target_port;
    plc.field("buttons") << login << cancel;
    // Finially, the widgets should be collocated.
    // Do not miss this line, otherwise the widgets are not collocated
    // until the form is resized.
    plc.collocate();
    fo.show();
    nana::exec();
  } while ((name == "" || str == "") || ((str1 == "") != (str2 == "")));

  // Start to event loop process, it blocks until the form is closed.

  /*              we will be listening on port given as first parameter -v */
  breep::tcp::network network(static_cast<unsigned short>(std::stoi(str)));

  // Disabling all logs (set to 'warning' by default).
  network.set_log_level(breep::log_level::none);

  chat_room cr(name);
  cr.start_listening(network);

  // If we receive a class for which we don't have any listener (such as an int,
  // for example), this will be called.
  network.set_unlistened_type_listener(
      [](breep::tcp::network &, const breep::tcp::peer &, breep::deserializer &,
         bool, uint64_t) -> void {
        std::cout << "Unlistened class received." << std::endl;
      });

  if ((str1 == "") && (str2 == "")) {
    // runs the network in another thread.
    network.awake();
  } else {
    // let's try to connect to a buddy at address argv[2] and port argv[3]
    boost::asio::ip::address address =
        boost::asio::ip::address::from_string(str1);
    auto val = static_cast<unsigned short>(stoi(str2));
    if (!network.connect(address, val)) {
      // oh noes, it failed!
      std::cout << "Connection failed." << std::endl;
      return 1;
    }
  }

  // Define a form.
  bool g_exit = false;
  while (flag) {
    do {
      nana::form yy;
      yy.caption("NSchat");
      nana::textbox mess{yy};

      nana::button send_mess{yy, "send"}, cancel1{yy, "Cancel"};
      mess.tip_string("Enter mess:").multi_lines(false);
      // Define a place for the form.
      nana::place plc{yy};
      // Divide the form into fields

      send_mess.events().click([&yy, &mess] {
        ans = mess.text();
        yy.close();
      });

      cancel1.events().click([&yy, &network] {
        network.disconnect();
        yy.close();
        g_accept1 = true;
      });
      if (g_accept1) {
        return 1;
      }

      // plc.div("margin= 10%   gap=20 vertical< weight=70 gap=20 vertical
      // textboxs arrange=[25,25]> <min=20> <weight=25 gap=10 buttons>  > ");
      plc.div("<><weight=80% vertical<><weight=70% vertical <vertical gap=10 "
              "textboxs arrange=[25,25]>  <weight=25 gap=10 buttons> ><>><>");
      // Insert widgets
      // The field textboxs is vertical, it automatically adjusts the widgets'
      // top and height.
      plc.field("textboxs") << mess;
      plc.field("buttons") << send_mess << cancel1;
      // Finially, the widgets should be collocated.
      // Do not miss this line, otherwise the widgets are not collocated
      // until the form is resized.
      plc.collocate();
      yy.show();
      nana::exec();

    } while (g_exit);
    if (ans != "") {
      network.send_object(chat_message<std::string>(ans));
      ans = "";
    }
  }
  // we'll remove any listeners (useless here, as we're going out of scope.
  network.clear_any();
  return 0;
}
