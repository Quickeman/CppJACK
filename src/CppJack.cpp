#include "CppJack.h"

namespace jack {

// class Client

Client::Client() {

}

Client::Client(int nChannelsOut, int nChannelsIn, string clientName, string serverName) {

}

Client::~Client() {

}

void Client::open(int nChannelsOut, int nChannelsIn, string clientName, string serverName) {

}

void Client::start(Callback* cb) {

}

void Client::stop() {

}

void Client::close() {

}

int Client::_process(jack_nframes_t nFrames, void* arg) {

}

void Client::_shutdown(void* arg) {
    
}

} // namespace jack
