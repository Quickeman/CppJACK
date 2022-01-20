#include "CppJack.h"
#include "exceptions.hpp"

#include <iostream>
#include <cstring>

using namespace std;

namespace jack {

// class Client

Client::Client(int nChannelsOut, int nChannelsIn, string clientName, string serverName) {
    open(nChannelsOut, nChannelsIn, clientName, serverName);
}

Client::~Client() {
    if (isOpen() && !closed) close();
}

void Client::open(int nChannelsOut, int nChannelsIn, string clientName, string serverName) {
    setNumPorts(nChannelsOut, nChannelsIn);

    this->clientName = clientName;
    this->serverName = serverName;
    if (this->serverName != "") {
        // If a server name is provided, use it
        options = JackServerName;
        client = jack_client_open(
            this->clientName.data(),
            options,
            &jackStatus,
            this->serverName.data()
        );
    } else {
        client = jack_client_open(
            this->clientName.data(),
            options,
            &jackStatus
        );
    }

    // Check the client opened
    if (client == NULL) {
        // Check the server connection was successful
        if (jackStatus & JackServerFailed) {
            throw ClientException("Could not connect to Jack server.");
        }
        throw ClientException("Client failed to open.");
    } else {
        closed = false;
    }

    if (jackStatus & JackNameNotUnique) {
        // Non-unique name given, retrieve assigned name
        clientName.assign(jack_get_client_name(client));
    }

    // Assign shutdown method
    jack_on_shutdown(client, Client::_shutdown, this);

    // Create the ports/channels
    // Outputs
    for (int i { 0 }; i < nChannelsOut; i++) {
        string name { "output" };
        name.append(to_string(i + 1));
        outPorts[i] = jack_port_register(
            client,
            name.data(),
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsOutput,
            0
        );

        // No available ports error
        if (outPorts[i] == NULL) {
            throw ClientException("No output ports available.");
        }
    }

    // Inputs
    for (int i { 0 }; i < nChannelsIn; i++) {
        string name { "input" };
        name.append(to_string(i + 1));
        inPorts[i] = jack_port_register(
            client,
            name.data(),
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsInput,
            0
        );

        // No available ports error
        if (outPorts[i] == NULL) {
            throw ClientException("No input ports available.");
        }
    }
}

void Client::start(Callback* cb) {
    // Check the Jack client is open first
    if (!isOpen()) {
        throw ClientException("start: Jack client not open.");
        return;
    }

    // Set callback
    callback = cb;
    jack_set_process_callback(client, Client::_process, this);

    // Activate the client
    int err { jack_activate(client) };
    if (err) {
        throw ClientException("Could not activate client.");
    }

    // Connect ports
    // Outputs (labelled here as inputs as they're 'input' to the backend)
    auto pTmp { const_cast<char**>(jack_get_ports(client, NULL, NULL, JackPortIsPhysical|JackPortIsInput)) };
    for (size_t i {0}; i < outPorts.size(); i++) {
        if (pTmp[i] == NULL) {
            cerr << "Trying to connect too many (" << i+1 << ") output ports.\n";
            cout << "Limiting number of output ports to " << i << ".\n";
            setNumPorts(i, inPorts.size());
            break;
        }
        ports[i].assign(pTmp[i]);
        err = jack_connect(
            client,
            jack_port_name(outPorts[i]), 
            ports[i].data()
        );
        if (err) {
            throw ClientException("Could not connect output port.");
        }
    }
    jack_free(pTmp);

    // Inputs (labelled here as outputs as they're 'output' from the backend)
    pTmp = const_cast<char**>(jack_get_ports(client, NULL, NULL, JackPortIsPhysical|JackPortIsOutput));
    for (size_t i {0}; i < inPorts.size(); i++) {
        if (pTmp[i] == NULL) {
            cerr << "Trying to connect too many (" << i+1 << ") input ports.\n";
            cout << "Limiting number of input ports to " << i << ".\n";
            setNumPorts(outPorts.size(), i);
            break;
        }
        ports[outPorts.size() + i].assign(pTmp[i]);
        err = jack_connect(
            client,
            ports[outPorts.size() + i].data(),
            jack_port_name(inPorts[i])
        );
        if (err) {
            throw ClientException("Could not connect input port.");
        }
    }
    jack_free(pTmp);
}

void Client::stop() {
    if (isOpen())
        jack_deactivate(client);
}

void Client::close() {
    if (isOpen())
        jack_client_close(client);
    closed = true;
}

bool Client::isOpen() const {
    return client != nullptr;
}

jack_nframes_t Client::sampleRate() const {
    return jack_get_sample_rate(client);
}

jack_nframes_t Client::bufferSize() const {
    return jack_get_buffer_size(client);
}

jack_nframes_t Client::bufferSize(jack_nframes_t size) {
    jack_set_buffer_size(client, size);
    return jack_get_buffer_size(client);
}

size_t Client::nOutputPorts() const {
    return outPorts.size();
}

size_t Client::nInputPorts() const {
    return inPorts.size();
}

int Client::_process(jack_nframes_t nFrames, void* arg) {
    // `arg` should be a pointer to the Client object
    Client& self { *static_cast<Client*>(arg) };

    // Prepare output buffer
    for (auto& buff : self.outBuff)
        buff.assign(nFrames, 0.f);
        
    // Get input samples
    for (size_t i {0}; i < self.inPorts.size(); i++) {
        self.inBuff[i].resize(nFrames);
        memcpy(self.inBuff[i].data(),
            jack_port_get_buffer(self.inPorts[i], nFrames),
            nFrames * sizeof(sample_t));
    }

    // Send buffers to DSP callback
    self.callback->process(nFrames, self.outBuff, self.inBuff);

    // Send samples to output
    for (size_t i {0}; i < self.outPorts.size(); i++) {
        memcpy(jack_port_get_buffer(self.outPorts[i], nFrames),
            self.outBuff[i].data(),
            nFrames * sizeof(sample_t));
    }

    return 0;
}

void Client::_shutdown(void* arg) {
    exit(1);
}

void Client::setNumPorts(int nOut, int nIn) {
    outPorts.resize(nOut);
    inPorts.resize(nIn);
    ports.resize(nOut + nIn);

    outBuff.resize(nOut);
    inBuff.resize(nIn);
}

} // namespace jack
