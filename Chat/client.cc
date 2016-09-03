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

void playSound(message &msg,SoundBuffer &buffer){
  size_t count;
  msg >>count;
  size_t rate;
  msg >>rate;
  size_t channelCount;
  msg >>channelCount;
  const Int16 *sample;
  msg>>sample;
  buffer.loadFromSamples(sample,count,channelCount,rate);

    // Create a sound instance and play it
  sf::Sound sound(buffer);
  sound.play();

  // Loop while the sound is playing
  while (sound.getStatus() == sf::Sound::Playing) {
    // Leave some CPU time for other processes
    sf::sleep(sf::milliseconds(100));

    // Display the playing position
    cout << "\rPlaying... " << sound.getPlayingOffset().asSeconds() << " sec        ";
    cout << flush;
  }
  cout<<"termine de reproducir"<<endl;

    

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

SoundBuffer recordSound(SoundBufferRecorder &recorder, bool isCall=false){
  //SoundBufferRecorder recorder;
  unsigned int sampleRate=44100;
  // Audio capture is done in a separate thread, so we can block the main thread while it is capturing
  recorder.start(sampleRate);
  if(!isCall){
    cout << "Recording... press enter to stop";
    cin.ignore(10000, '\n');

  }else{
   // time.sleep(1000);
    sleep(milliseconds(1000));
  }
  recorder.stop();
  // Get the buffer containing the captured data
  const SoundBuffer& buffer = recorder.getBuffer();
  // captured sound informations
  
  return buffer;
}

void voiceCall(message &msg,SoundBuffer &buffer,bool &callState,SoundBufferRecorder &recorder,
  vector<string> &tokens,socket &s){ 
  if (!sf::SoundBufferRecorder::isAvailable()) {
    cout << "No mic interface avaiable " << endl;
  }
  while (callState) {
   /* vector<string> v;
    v.push_back("voicec");
    v.push_back(name_sender);
    */
    buffer=recordSound(recorder,callState);
    // tokens.push_front("voice");
    const SoundBuffer &buffer = recorder.getBuffer();
    //send_voice(buffer, v, s, userName, tiempo);
    sendVoice(buffer,tokens,s);
  }
  cout << "Call has ended" << endl;
}

void handleInputFromServer(message &msg,SoundBuffer &buffer,bool &callState,thread *listening,
  SoundBufferRecorder &recorder,vector<string> &tokens,socket &s){
  cout<<"remanenteenelcliente "<<msg.remaining()<<endl; //debuggear
  string id;
  msg>> id;
  string action;
  msg>> action;
  //SoundBuffer receive_buffer;
  if (action=="voice"){
    string dest;
    msg>> dest; // sacamos la posicion del mensaje que no nos sirve
    playSound(msg,buffer);
    
  }else if(action=="call") {
    callState=true;
    listening =new thread(voiceCall,ref(msg),ref(buffer),ref(callState),ref(recorder),ref(tokens),ref(s));


  }else{
    
    cout << "Socket> " << action << endl;

  }
  

  // si es de otro tipo solamente muestremlo 

}

void handleInputFromConsole(SoundBuffer &buffer,SoundBufferRecorder &recorder,vector<string> &tokens,socket &s){
  if(tokens[0]== "voice"){
    buffer=(recordSound(recorder));
    cout<< "buffer es : "<<buffer.getSampleRate()<<endl;
    cout<< "buffer es : "<<buffer.getChannelCount()<<endl;
    sendVoice(buffer,tokens,s);
  } else if (tokens[0]=="call"){
    // do some call actions
    cout<<"voy a hacer una llamada \n";
  } else{
    //sending text
    message m;
    for (const string &str : tokens)
      m << str;
      s.send(m);
  }

}

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
  SoundBuffer receive_buffer;
  SoundBufferRecorder recorder;
  thread *listening;
  //thread *speaking;
  bool callState=false;

  while (true) {
    if (poll.poll()) { // There are events in at least one of the sockets
      if (poll.has_input(s)) {
        // Handle input in socket
        message m;
        s.receive(m);
        //listening = new thread (receiver,ref(m),ref(receive_buffer));
        handleInputFromServer(m,receive_buffer,callState,listening,recorder,tokens,s);
        
        
        //message m;
        //s.receive(m);
       // handleInputFromServer(m,receive_buffer);
        
      }
      if (poll.has_input(console)) {
        //thread speaking ();
        // Handle input from console
        string input;
        getline(cin, input);
        tokens = tokenize(input);
        handleInputFromConsole(buffer,recorder,tokens,s);
        

      }
    }
  }
  listening->join();
  return EXIT_SUCCESS;
}
