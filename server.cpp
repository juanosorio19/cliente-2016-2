#include <iostream>
#include <string>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

int main(int argc, char *argv[]) {
  // initialize the 0MQ context
  context ctx;

  // generate a pull socket
  socket s(ctx, socket_type::reply);

  // bind to the socket
  cout << "Binding " << endl;
  s.bind("tcp://*:4242");

  while (true) {
    // receive the message
    cout << "Receiving message..." << endl;
    message req;
    s.receive(req);

    string op;
    int op1 = 0;
    int op2 = 0;
    req >> op >> op1 >> op2;
    int result = op1 + op2;

    message rep;
    rep << result;
    s.send(rep);
    cout << "Sent: " << result << endl;
  }
  cout << "Finished." << endl;
}
