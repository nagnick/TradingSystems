#include <iostream>
#include "TradierBroker.h"
#include "HTTPSClient.h"
using namespace std;

int main(){
    cout << "Hello World\n";
    HTTPSClient client("www.google.com",443);
    client.sendRequest("{\"Hello\"}");
    return 0;
}