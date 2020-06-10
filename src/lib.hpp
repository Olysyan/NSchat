#pragma once

#include <breep/network/tcp.hpp>
#include <breep/util/serialization.hpp>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>


#include <iostream>
#include <string>

template <typename T> class chat_message {
public:
  /* Default constructor is required by serialization. */
  chat_message() : m_message{} {}
  explicit chat_message(const T &message) : m_message(message) {}

  const T &message() const { return m_message; }

  template <typename U>
  friend breep::deserializer &operator>>(breep::deserializer &,
                                         chat_message<U> &);

private:
  T m_message;
  BREEP_ENABLE_SERIALIZATION(chat_message<T>, m_message)
};

/* This class will be sent to each newly connected peer, so that he can know our
 * name. */
struct name {

  /* Default constructor is required by serialization. */
  name() : value{} {}
  explicit name(const std::string &name_) : value(name_) {}
  std::string value;
};

// Writing the serialization operators manually this time (just to demonstrate
// how to do this)
breep::serializer &operator<<(breep::serializer &s, const name &local_name) {
  s << local_name.value;
  return s;
}

breep::deserializer &operator>>(breep::deserializer &d, name &local_name) {
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
  explicit chat_room(const std::string &name)
      : m_name(name), peer_map(), m_co_id(), m_dc_id(), m_ids() {}

  void start_listening(breep::tcp::network &net) {
    // because we overloaded the () operator with [breep::tcp::network&, const
    // breep::tcp::peer& parameters, we can register ourselves this way. the ()
    // method will be called.
    m_co_id = net.add_connection_listener(std::ref(*this));
    m_dc_id = net.add_disconnection_listener(std::ref(*this));
    // nb: we are using std::ref to avoid a copy of ourself to be made

    // add_data_listener returns a different type than
    // (add_connection/add_disconnecton)_listener.
    m_ids.push_back(net.add_data_listener<name>(std::ref(*this)));

    /* We can also record listeners this way */
    m_ids.push_back(net.add_data_listener<chat_message<std::string>>(
        [this](breep::tcp::netdata_wrapper<chat_message<std::string>> &dw)
            -> void { this->string_received(dw); }));
  }

  void stop_listening(breep::tcp::network &net) {
    // unregistering listeners.
    net.remove_connection_listener(m_co_id);
    net.remove_disconnection_listener(m_dc_id);
    for (const breep::type_listener_id &tli : m_ids) {
      net.remove_data_listener(tli);
    }
  }

  // A peer just connected // disconnected
  void operator()(breep::tcp::network &network, const breep::tcp::peer &peer) {
    if (peer.is_connected()) {
      // if it's someone new, we send her/him our name
      network.send_object_to(peer, m_name);
    } else {
      // (s)he left :/
      // std::cout << peer_map.at(peer.id()) << " disconnected." << std::endl;
      using namespace nana;
      form tp;
      tp.caption("NSchat");
      nana::place pl(tp);
      nana::label lab_11{tp, peer_map.at(peer.id())};
      nana::label lab_22{tp, " disconnected."};
      lab_11.format(true);
      lab_22.format(true);
      tp.div("<vfit=200 vertical text>");
      tp["text"] << lab_11 << lab_22;
      pl.collocate();
      tp.show();
      nana::exec();
      peer_map.erase(peer.id());
    }
  }
  void
  string_received(breep::tcp::netdata_wrapper<chat_message<std::string>> &dw) {
    // Someone sent you a nice little message.
    // std::cout << peer_map.at(dw.source.id()) << ": " << dw.data.message() <<
    // std::endl;

    using namespace nana;
    form tt;
    tt.caption("NSchat");
    nana::place pl(tt);
    nana::label lab00{tt, peer_map.at(dw.source.id())};
    nana::label lab0{tt, dw.data.message()};
    lab0.format(true);
    lab00.format(true);
    tt.div("<vfit=200 vertical text>");
    tt["text"] << lab0 << lab00;
    pl.collocate();
    tt.show();
    nana::exec();
  }

  void operator()(breep::tcp::netdata_wrapper<name> &dw) {
    // The guy to whom we just connected sent us his/her name.
    // std::cout << dw.data.value << " connected." << std::endl;
    using namespace nana;
    form ta;
    ta.caption("NSchat");
    nana::place pl(ta);
    nana::label lab_1{ta, dw.data.value};
    nana::label lab_2{ta, " connected."};
    lab_1.format(true);
    lab_2.format(true);
    ta.div("<vfit=200 vertical text>");
    ta["text"] << lab_1 << lab_2;
    pl.collocate();
    ta.show();
    nana::exec();
    peer_map.insert(std::make_pair(dw.source.id(), dw.data.value));
  }

private:
  const name m_name;
  std::unordered_map<boost::uuids::uuid, std::string,
                     boost::hash<boost::uuids::uuid>>
      peer_map;

  breep::listener_id m_co_id, m_dc_id;
  std::vector<breep::type_listener_id> m_ids;
};