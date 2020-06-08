#include <breep/network/tcp.hpp>
#include <breep/util/serialization.hpp>
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>
<<<<<<< HEAD
#include <string>
#include <iostream>
bool flag = true;
std::string ans;
bool g_accept = false;
bool g_accept1 = false;
template <typename T>
class chat_message {
public:

	/* Default constructor is required by serialization. */
	chat_message(): m_message{} {}
	explicit chat_message(const T& message)
			: m_message(message)
	{}

	const T& message() const {
		return m_message;
	}

	template <typename U>
	friend breep::deserializer& operator>>(breep::deserializer&, chat_message<U>&);

private:
	T m_message;
	BREEP_ENABLE_SERIALIZATION(chat_message<T>, m_message)
};

/* This class will be sent to each newly connected peer, so that he can know our name. */
struct name {

	/* Default constructor is required by serialization. */
	name() : value{} {}
	explicit name(const std::string& name_) : value(name_) {}
	std::string value;
};

// Writing the serialization operators manually this time (just to demonstrate how to do this)
breep::serializer& operator<<(breep::serializer& s, const name& local_name) {
	s << local_name.value;
	return s;
}

breep::deserializer& operator>>(breep::deserializer& d, name& local_name) {
	d >> local_name.value;
	return d;
}


/* Here is where we declare the types for breep::network.
 * These macros must be called outside of all namespace. */
BREEP_DECLARE_TYPE(name)
/* chat_message is a partially intiated type (missing template parameter */
BREEP_DECLARE_TEMPLATE(chat_message)


/* We need to declare these two types because they will be used as
 * template parameter of chat_message. */
BREEP_DECLARE_TYPE(std::string)


class chat_room {
public:
	explicit chat_room(const std::string& name)
			: m_name(name)
            , peer_map()
            , m_co_id()
            , m_dc_id()
			, m_ids()
	{}

	void start_listening(breep::tcp::network& net) {
		// because we overloaded the () operator with [breep::tcp::network&, const breep::tcp::peer& parameters,
		// we can register ourselves this way. the () method will be called.
		m_co_id = net.add_connection_listener(std::ref(*this));
		m_dc_id = net.add_disconnection_listener(std::ref(*this));
		//nb: we are using std::ref to avoid a copy of ourself to be made

		// add_data_listener returns a different type than (add_connection/add_disconnecton)_listener.
		m_ids.push_back(net.add_data_listener<name>(std::ref(*this)));

		/* We can also record listeners this way */
		m_ids.push_back(net.add_data_listener<chat_message<std::string>>(
		                [this] (breep::tcp::netdata_wrapper<chat_message<std::string>>& dw) -> void {
			                this->string_received(dw);
		                }));

		
	}

	void stop_listening(breep::tcp::network& net) {
		// unregistering listeners.
		net.remove_connection_listener(m_co_id);
		net.remove_disconnection_listener(m_dc_id);
		for (const breep::type_listener_id& tli : m_ids) {
			net.remove_data_listener(tli);
		}
	}

	// A peer just connected // disconnected
	void operator()(breep::tcp::network& network, const breep::tcp::peer& peer) {
		if (peer.is_connected()) {
			// if it's someone new, we send her/him our name
			network.send_object_to(peer, m_name);
		} else {
			// (s)he left :/
			std::cout << peer_map.at(peer.id()) << " disconnected." << std::endl;
			peer_map.erase(peer.id());
		}
	}
	void string_received(breep::tcp::netdata_wrapper<chat_message<std::string>>& dw) {
		// Someone sent you a nice little message.
		//std::cout << peer_map.at(dw.source.id()) << ": " << dw.data.message() << std::endl;

		using namespace nana;
		form tt;
		tt.caption("NSchat");
		nana::place pl(tt);
		nana::label lab00{ tt,  peer_map.at(dw.source.id()) };
		nana::label lab0{ tt,  dw.data.message() };
		lab0.format(true);
		lab00.format(true);
		tt.div( "<vertical text>");
		tt["text"] << lab0<<lab00;
		pl.collocate();
		tt.show();
        nana::exec();
	}

	void operator()(breep::tcp::netdata_wrapper<name>& dw) {
		// The guy to whom we just connected sent us his/her name.
		std::cout << dw.data.value << " connected." << std::endl;
		peer_map.insert(std::make_pair(dw.source.id(), dw.data.value));
	}

private:
	const name m_name;
	std::unordered_map<boost::uuids::uuid, std::string, boost::hash<boost::uuids::uuid>> peer_map;

	breep::listener_id m_co_id, m_dc_id;
	std::vector<breep::type_listener_id> m_ids;
};

int main(int argc, char* argv[]) {
    std::string str;
	std::string str1;
	std::string name;
	std::string str2;
	do{
		nana::form fo;
		fo.caption("NSchat");
        nana::textbox usr  {fo},   
                port {fo},
				target_ip {fo},
				target_port {fo};
        nana::button  login {fo, "Login"}, 
                cancel{fo, "Cancel"};
        usr.tip_string("User:"    ).multi_lines(false);
        port.tip_string("Port:"   ).multi_lines(false);
		target_ip.tip_string("Target ip:"    ).multi_lines(false);
		target_port.tip_string("Target port:"    ).multi_lines(false);
                // Define a place for the form.
        nana::place plc {fo};
                // Divide the form into fields
        login.events().click([&login, &fo, &name, &usr, &port, &argv, &str1, &str, &target_ip,&target_port,&str2]{
       
       name = usr.text();
	   str = port.text();
	   str1 = target_ip.text();
	   str2 = target_port.text();
	   fo.close();
  	});
	   cancel.events().click([&fo]{
		   fo.close();
		   g_accept = true;
	   });
	if(g_accept){
		return 1;
	}


        //plc.div("margin= 10%   gap=20 vertical< weight=70 gap=20 vertical textboxs arrange=[25,25]> <min=20> <weight=25 gap=10 buttons>  > ");
        plc.div("<><weight=80% vertical<><weight=70% vertical <vertical gap=10 textboxs arrange=[25,25]>  <weight=25 gap=10 buttons> ><>><>");
        //Insert widgets
        //The field textboxs is vertical, it automatically adjusts the widgets' top and height. 
        plc.field("textboxs")<< usr  << port << target_ip <<target_port;
        plc.field("buttons") <<login << cancel;
        // Finially, the widgets should be collocated.
        // Do not miss this line, otherwise the widgets are not collocated
        // until the form is resized.
        plc.collocate();
        fo.show();
        nana::exec();
	}while((name == "" || str == "") || ((str1 == "") != (str2 == "")));

  //Start to event loop process, it blocks until the form is closed.

	/*              we will be listening on port given as first parameter -v */
	breep::tcp::network network(static_cast<unsigned short>(std::stoi(str)));

	// Disabling all logs (set to 'warning' by default).
	network.set_log_level(breep::log_level::none);

	chat_room cr(name);
	cr.start_listening(network);
    
	// If we receive a class for which we don't have any listener (such as an int, for example), this will be called.
	network.set_unlistened_type_listener([](breep::tcp::network&,const breep::tcp::peer&,breep::deserializer&,bool,uint64_t) -> void {
		std::cout << "Unlistened class received." << std::endl;
	});

	if ((str1 == "") && (str2 == "")) {
		// runs the network in another thread.
		network.awake();
	} else {
		// let's try to connect to a buddy at address argv[2] and port argv[3]
		boost::asio::ip::address address = boost::asio::ip::address::from_string(str1);
		auto val =static_cast<unsigned short>(stoi(str2)); 
        if(!network.connect(address, val)) { 
			// oh noes, it failed!
			std::cout << "Connection failed." << std::endl;
			return 1;
		}
	}

  //Define a form.
  	bool g_exit = false;
	while(flag){
	do
	{
		nana::form yy;
		yy.caption("NSchat");
        nana::textbox mess  {yy};   

        nana::button  send_mess {yy, "send"}, 
                cancel1 {yy, "Cancel"};
        mess.tip_string("Enter mess:"    ).multi_lines(false);
                // Define a place for the form.
        nana::place plc {yy};
                // Divide the form into fields

        send_mess.events().click([&yy, &mess]{

		ans  = mess.text();
        yy.close();
	    
  	});
   
	   cancel1.events().click([&yy,&network]{
		   network.disconnect();
		   yy.close();
		   g_accept1 = true;
	   });
	if(g_accept1){
		return 1;
	}

        //plc.div("margin= 10%   gap=20 vertical< weight=70 gap=20 vertical textboxs arrange=[25,25]> <min=20> <weight=25 gap=10 buttons>  > ");
        plc.div("<><weight=80% vertical<><weight=70% vertical <vertical gap=10 textboxs arrange=[25,25]>  <weight=25 gap=10 buttons> ><>><>");
        //Insert widgets
        //The field textboxs is vertical, it automatically adjusts the widgets' top and height. 
        plc.field("textboxs")<< mess;
        plc.field("buttons") << send_mess << cancel1;
        // Finially, the widgets should be collocated.
        // Do not miss this line, otherwise the widgets are not collocated
        // until the form is resized.
        plc.collocate();
        yy.show();
        nana::exec();

	}while(g_exit);
	 if(ans != ""){
	  network.send_object(chat_message<std::string>(ans));
	  ans = "";
	  }
	}
	// we'll remove any listeners (useless here, as we're going out of scope.
	network.clear_any();
	return 0;
}
=======

>>>>>>> 54ada369b81faa2453f3434dfcfd54728b1d35e4
