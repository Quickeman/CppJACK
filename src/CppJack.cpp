#include "CppJack.h"

#include <iostream>

namespace jack {

// class Client

Client::Client(int nChannelsOut, int nChannelsIn, string clientName, string serverName) {
    open(nChannelsOut, nChannelsIn, clientName, serverName);
}

Client::~Client() {
    if (isOpen()) close();
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
            cout << "Could not connect to Jack server\n";
        }
        cout << "Client failed to open\n";
    }

    if (jackStatus & JackNameNotUnique) {
        // Non-unique name given, retrieve assigned name
        clientName.assign(jack_get_client_name(client));
    }

    // Assign shutdown method
    jack_on_shutdown(client, Client::_shutdown, this);

    // Create the ports/channels
    // Outputs
    for (int i = 0; i < outPorts.size(); i++) {
        string name = "output";
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
            cout << "No output ports available\n";
        }
    }

    // Inputs
    for (int i = 0; i < inPorts.size(); i++) {
        string name = "input";
        name.append(to_string(i + 1));
        outPorts[i] = jack_port_register(
            client,
            name.data(),
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsInput,
            0
        );

        // No available ports error
        if (outPorts[i] == NULL) {
            cout << "No input ports available\n";
        }
    }
}

void Client::start(Callback* cb) {
    // Check the Jack client is open first
    if (!isOpen()) {
        cout << "Jack client not open\n";
        return;
    }

    // Set callback
    callback = cb;
    jack_set_process_callback(client, Client::_process, this);

    // Activate the client
    int err = jack_activate(client);
    if (err) {
        cout << "Could not activate client\n";
    }

    // Connect ports
    char** pTmp;
    // Outputs (labelled here as inputs as they're 'input' to the backend)
    pTmp = const_cast<char**>(jack_get_ports(client, NULL, NULL, JackPortIsPhysical|JackPortIsInput));
    for (int i = 0; i < outPorts.size(); i++) {
        ports[i].assign(pTmp[i]);
        err = jack_connect(
            client,
            jack_port_name(outPorts[i]), 
            ports[i].data()
        );
        if (err) {
            cout << "Could not connect output port\n";
        }
    }
    jack_free(pTmp);

    // Inputs (labelled here as outputs as they're 'output' from the backend)
    pTmp = const_cast<char**>(jack_get_ports(client, NULL, NULL, JackPortIsPhysical|JackPortIsOutput));
    for (int i = 0; i < inPorts.size(); i++) {
        ports[outPorts.size() + i].assign(pTmp[i]);
        err = jack_connect(
            client,
            ports[outPorts.size() + i].data(),
            jack_port_name(inPorts[i])
        );
        if (err) {
            cout << "Could not connect input port\n";
        }
    }
    jack_free(pTmp);
}

void Client::stop() {
    jack_deactivate(client);
}

void Client::close() {
    jack_client_close(client);
}

int Client::_process(jack_nframes_t nFrames, void* arg) {
    // `arg` should be a pointer to the JackClient object
    Client* self = static_cast<Client*>(arg);

    // Prepare output buffer
    for (int i = 0; i < self->outPorts.size(); i++) {
        self->outBuff[i] = (sample_t*)jack_port_get_buffer(self->outPorts[i], nFrames);
    }
    // Get input samples
    for (int i = 0; i < self->inPorts.size(); i++) {
        self->inBuff[i] = (sample_t*)jack_port_get_buffer(self->inPorts[i], nFrames);
    }

    // Process the samples/get output samples
    self->callback->process(nFrames, self->outBuff, self->inBuff);

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