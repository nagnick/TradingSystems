#include <iostream>
#include "TradierBroker.h"
#include "HTTPSClient.h"
using namespace std;

int main(){
    cout << "Hello World\n";
    HTTPSClient client("http://pocoproject.org/images/front_banner.jpg",443);
    return 0;
}