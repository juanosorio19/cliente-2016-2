#include <iostream>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

int main(int argc, char const *argv[]) {
  if (argc != 4) {
    cerr << "Error calling the program" << endl;
    return 1;
  }

  context ctx;
  socket s(ctx, socket_type::request);

  cout << "Opening connection" << endl;
  s.connect("tcp://localhost:4242");

  string op(argv[1]);
  int op1 = atoi(argv[2]);
  int op2 = atoi(argv[3]);

  message req;
  req << op << op1 << op2;
  s.send(req);
  cout << "Request sent." << endl;

  message rep;
  s.receive(rep);
  int result = 0;
  rep >> result;
  cout << "Response " << result << endl;

  cout << "Finished." << endl;
  return 0;
}


