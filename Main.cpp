#include <iostream>
#include "TradierBroker.h"
using namespace std;

int main(){
    cout << "Hello World\n";
    TradierBroker broker = TradierBroker("Hello");
    broker.sendRequestAndGetResponse();
    return 0;
}