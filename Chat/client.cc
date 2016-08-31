#include "json.hpp"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <zmqpp/zmqpp.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

vector<string> tokenize(string &input) {
  stringstream ss(input);
  vector<string> result;
  string s;
  while (ss >> s)
    result.push_back(s);
  return result;
}
//assertions
int main(int argc, char const *argv[]) {
  if (argc != 2) {
    cerr << "Invalid arguments" << endl;
    return EXIT_FAILURE;
  }
  string address(argv[1]);
  //string userName(argv[2]);
  //string password(argv[3]);
  string sckt("tcp://");
  sckt += address;

  context ctx;
  socket s(ctx, socket_type::xrequest);

  cout << "Connecting to: " << sckt << endl;
  s.connect(sckt);

  //message login;
  //login << "login" << userName << password;
  //s.send(login);

  int console = fileno(stdin);
  poller poll;
  poll.add(s, poller::poll_in);
  poll.add(console, poller::poll_in);
  while (true) {
    if (poll.poll()) { // There are events in at least one of the sockets
      if (poll.has_input(s)) {
        // Handle input in socket
        message m;
        s.receive(m);
        string response;
        m >> response;
        cout << "Socket> " << response << endl;
      }
      if (poll.has_input(console)) {
        // Handle input from console
        string input;
        getline(cin, input);
        vector<string> tokens = tokenize(input);
        message m;
        for (const string &str : tokens)
          m << str;
        s.send(m);
      }
    }
  }
  return EXIT_SUCCESS;
}
