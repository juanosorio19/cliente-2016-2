#include "json.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

class User {
private:
  string name;
  string password;
  string netId;
  bool connected;
  list<string> contacts;

public:
  User() {}
  User(const string &name, const string &pwd, const string &id)
      // Attribute initialization
      : name(name),
        password(pwd),
        netId(id),

        connected(false) {}

  bool isPassword(const string &pwd) const { return password == pwd; }

  bool logged( ) const { return connected; }

  void connect(const string &id) {
    connected = true;
    netId = id;
  }
  const string &identity() const { return netId; }
};

class ServerState {
private:
  // Connected users
  unordered_map<string, User> users;
  socket &sckt;
  unordered_map<string,vector<string>> groups;

public:
  ServerState(socket &s) : sckt(s) {}

  void send(message &m) { sckt.send(m); }
  bool newUser(const string &name, const string &pwd, const string &id) {
    if (users.count(name) > 0)
      return false;
    else {
      users[name] = User(name, pwd, id);
      return true;
    }
  }

  bool isConnected(const string &name) {
    bool ok = users[name].logged();
    return ok;
  }

  bool login(const string &name, const string &pwd, const string &id) {
    if (users.count(name) > 0) {
      // User is registered
      bool ok = users[name].isPassword(pwd);
      if (ok){
        users[name].connect(id);
        groups["default"].push_back(name); }
      return ok;
    }
    return false;
  }
  void sendMessage(const string &dest, const string &text) {
    message m;
    m << users[dest].identity() << text;
    send(m);
  }

  void sendGroup(const string &dest, const string &text) {
    //message m;
    for (int i = 0; i < groups[dest].size(); ++i)
    {

      cout<<endl<<i<<" "<<groups[dest][i]<< endl;

      sendMessage(groups[dest][i],text);

      //m << groups[dest][i].identity() << text;
      //send(m);

    }

  }

  void registerGroup(const string &group,const string &sender, const string &name) {
    groups[group].push_back(name);
  }
};

void login(message &msg, const string &sender, ServerState &server) {
  if (msg.remaining() == 2) {
    string userName;
    msg >> userName;
    string password;
    msg >> password;
    if (server.login(userName, password, sender)) {
      cout << "User " << userName << " joins the chat server" << endl;
    } else {
      cerr << "Wrong user/password " << endl;
    }
  }
}

void newUser(message &msg, const string &sender, ServerState &server) {
  if (msg.remaining() == 2) {
    string userName;
    msg >> userName;
    string password;
    msg >> password;
    if (server.newUser(userName, password, sender)) {
      cout << "User " << userName << " registered succesfully :D" << endl;
    } else {
      cerr << "User is already registered D:" << endl;
    }
  }
  else {
    cerr << "Malo"; //TODO enviar mensaje al usuario ..
  }
}

void sendMessage(message &msg, const string &sender, ServerState &server) {
  if (msg.remaining() == 2) {
    string dest;
    msg >> dest;

    string text;
    msg >> text;
    server.sendMessage(dest, text);
  }
}

void sendGroup(message &msg, const string &sender, ServerState &server) {
  if (msg.remaining() == 2) {
    string group;
    msg >> group;

    string text;
    msg >> text;

    server.sendGroup(group, text);
  }
}

void registerGroup(message &msg, const string &sender, ServerState &server) {
  if (msg.remaining() == 2) {
    string group;
    msg >> group;
    //TODO obtner el id automáticamente
    string userName;
    msg>> userName;

    if (server.isConnected(userName)) {// si el usuario esta logueado

      server.registerGroup(group,sender,userName);
      //cout << "User " << userName << " joins the chat server" << endl; // entonces llamemos al metodo register group



    } else {
      cerr << "Wrong user/password " << endl; // si no, pues entonces no juju.
    }
  }
}

void dispatch(message &msg, ServerState &server) {
  if (msg.parts() > 2) {
    string sender;
    msg >> sender;

    string action;
    msg >> action;

    cout << "Action: " << action << endl;
    if (action == "login") {
      login(msg, sender, server);
    } else if (action == "msg") {
      sendMessage(msg, sender, server);
    } else if (action == "register") {
      newUser(msg, sender, server);
    } else if (action == "gregister") {
      registerGroup(msg, sender, server);
    } else if (action == "gmsg") {
      //sendGroup(msg, sender, server);
      sendGroup(msg, sender, server);
    }
     else {
      cerr << "Action not supported/implemented for " << action << endl;
      message reply;
      reply << sender << "unsupported" << action;
      server.send(reply);
    }
  }
  else {
    cerr << "Wrong arguments" << endl;
  }
}

int main(int argc, char *argv[]) {
  context ctx;
  socket s(ctx, socket_type::xreply);
  s.bind("tcp://*:4242");

  ServerState state(s);
  //state.newUser("Gustavo", "123", "");
  //state.newUser("Roberth", "123", "");
  while (true) {
    message req;
    s.receive(req);
    dispatch(req, state);
  }
  cout << "Finished." << endl;
}
