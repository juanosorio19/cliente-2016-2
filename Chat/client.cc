#include "json.hpp"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <zmqpp/zmqpp.hpp>
#include <SFML/Audio.hpp>
#include <bits/stdc++.h>

using namespace sf;
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

void handleInputFromServer(message &msg){
  string id;
  msg>> id;
  string type;
  msg>> type;
  cout<<"Hasta aqui llegue"<<endl;
  if (type=="voice"){
  // descomponer el mensaje y reproducir audio
  }else{
    
    cout << "Socket> " << type << endl;

  }
  

  // si es de otro tipo solamente muestremlo 

}

void sendVoice(const SoundBuffer &buffer, vector<string> &tokens, socket &s){ //, string userName , Time &time1 ??

    const Int16 *sample = buffer.getSamples();
    size_t count = buffer.getSampleCount();
    size_t rate = buffer.getSampleRate();
    size_t channelCount = buffer.getChannelCount();
    message m;
    //int tiempo = time1.asMilliseconds();
    m << tokens[0] << tokens[1] << count << rate << channelCount;
    m.add_raw(sample, count * sizeof(sf::Int16));
    //m << tiempo << userName;
    //m<<userName;
    s.send(m);
}

SoundBuffer recordSound(){
  SoundBufferRecorder recorder;
  unsigned int sampleRate=44100;
  // Audio capture is done in a separate thread, so we can block the main thread while it is capturing
  recorder.start(sampleRate);
  cout << "Recording... press enter to stop";
  cin.ignore(10000, '\n');
  recorder.stop();
  // Get the buffer containing the captured data
  const SoundBuffer& buffer = recorder.getBuffer();
  // captured sound informations
  
  return buffer;
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
  vector<string> tokens;
  SoundBuffer buffer;

  while (true) {
    if (poll.poll()) { // There are events in at least one of the sockets
      if (poll.has_input(s)) {
        // Handle input in socket
        message m;
        s.receive(m);
        handleInputFromServer(m);
        
      }
      if (poll.has_input(console)) {
        // Handle input from console
        string input;
        getline(cin, input);
        tokens = tokenize(input);
        if(tokens[0]== "voice"){

          buffer=(recordSound());
          cout<< "buffer es : "<<buffer.getSampleRate()<<endl;
          cout<< "buffer es : "<<buffer.getChannelCount()<<endl;
          sendVoice(buffer,tokens,s);

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
