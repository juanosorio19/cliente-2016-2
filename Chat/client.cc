#include "json.hpp"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <zmqpp/zmqpp.hpp>
#include <SFML/Audio.hpp>
#include <unistd.h>

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;
// gets the path of ogg file
string getPath(string ext) {
  char buff[256];
  getcwd(buff, 256);
  string cwd(buff);
  return cwd + ext;
}

vector<string> tokenize(string &input) {
  stringstream ss(input);
  vector<string> result;
  string s;
  while (ss >> s)
    result.push_back(s);
  return result;
}

void send_voice(const SoundBuffer &buffer, vector<string> &tokens, socket &s, string userName){ // , Time &time1 ??

    const Int16 *sample = buffer.getSamples();
    size_t count = buffer.getSampleCount();
    size_t rate = buffer.getSampleRate();
    size_t channelCount = buffer.getChannelCount();
    message m;
    int tiempo = time1.asMilliseconds();
    m << tokens[0] << tokens[1] << count << rate << channelCount;
    m.add_raw(sample, count * sizeof(sf::Int16));
    m << tiempo << userName;
    
    s.send(m);
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
  sf::SoundBuffer buffer;

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
        if(tokens[0]== "voice"){
          
        } else{

          //sending text
          message m;
          for (const string &str : tokens)
            m << str;
          s.send(m);
        }

      }
    }
  }
  return EXIT_SUCCESS;
}
