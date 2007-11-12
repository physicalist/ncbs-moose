#include <string>
using namespace std;

#define NB_DISABLE 0
#define NB_ENABLE 1

int kbhit();
void nonblock(int state);
bool nonBlockingGetLine( string& s );


