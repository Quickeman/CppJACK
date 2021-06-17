#ifndef CPPJACK_MAIN_H
#define CPPJACK_MAIN_H

#include <vector>
#include <string>

using namespace std;

#include "jack/jack.h"

/** The main namespace for CppJack.
 * All of the API is contained within this namespace.
 */
namespace jack {

typedef jack_default_audio_sample_t sample_t;

/** Abstract sample retrieval callback class.
 * Inherit from this class and override the @ref process method to implement
 * your audio client.
 */
class Callback {
public:
    /** Method called by a @ref Client object to process samples.
     * @param n the number of samples to process. A.K.A. the buffer size.
     * @param output an empty vector to fill with output samples. 2D, indexed as [channel][sample].
     * @param input a vector of input samples. 2D, indexed as [channel][sample].
     * @note the 2nd dimension of `output` and `input` are not vectors, they're raw pointers.
     */
    virtual void process(int n, vector<sample_t*> output, vector<sample_t*> input) {};
};


/** Jack client class.
 * This is the main port of call for using CppJack.
 */
class Client {
public:
    /** Default constructor.
     * Constructs the object but does not specify any parameters.
     * If using this constructor, you *must* call @ref open to set up the @ref Client.
     */
    Client() = default;

    /** Constructor.
     * Specifies client parameters.
     * If using this constructor, you do not need to call @ref open.
     * @param nChannelsOut number of output channels.
     * @param nChannelsIn number of input channels.
     * @param clientName desired name of the Jack client.
     * @param serverName name of the desired server to connect to. Leave blank to use the default Jack server.
     */
    Client(int nChannelsOut, int nChannelsIn, string clientName, string serverName = "");

    /** Destructor.
     * Closes the Jack client if still open.
     */
    ~Client();

    /** Sets up the Jack client.
     * Optional depending on which object constructor was used.
     * @param nChannelsOut number of output channels.
     * @param nChannelsIn number of input channels.
     * @param clientName desired name of the Jack client.
     * @param serverName name of the desired server to connect to. Leave blank to use the default Jack server.
     */
    void open(int nChannelsOut, int nChannelsIn, string clientName, string serverName = "");

    /** Informs Jack the client is ready to process samples.
     * Once this is called, Jack will begin to periodically request and/or deliver
     * sample buffers.
     * @param cb @ref Callback-type object to process samples.
     */
    void start(Callback* cb);

    /** Informs Jack that the client wants to stop processing samples. */
    void stop();

    /** Closes the Jack client. */
    void close();

    /** Returns the client's sample rate.
     * Note: Cannot be called before @ref open.
     */
    inline jack_nframes_t sampleRate() { return jack_get_sample_rate(client); }

    /** @return `true` if the client is open. */
    inline bool isOpen() { return client != NULL; }

    /** Method called by the Jack server to process samples.
     * Calls the callback's process method.
     * @param nFrames number of frames to process – the buffer size.
     * @param arg `this`.
     * @return Jack error code.
     */
    static int _process(jack_nframes_t nFrames, void *arg);

    /*! Shutdown method to exit the program should the Jack server shut down
    or disconnect the client.
    @param arg `this`. */
    static void _shutdown(void *arg);
private:
    /** Sets the number of ports. */
    void setNumPorts(int nOut, int nIn);

    /** Pointer to the @ref Callback object that fetches output samples. */
    Callback* callback;

    /** Pointer to the Jack client. */
    jack_client_t* client;
    /** Jack client name. */
    string clientName;
    /** Jack server name. */
    string serverName;
    /** Jack output ports. */
    vector<jack_port_t*> outPorts;
    /** Jack input ports. */
    vector<jack_port_t*> inPorts;
    /** Jack ports string. */
    vector<string> ports;
    /** Jack options. */
    jack_options_t options = JackNullOption;
    /** Jack status. */
    jack_status_t jackStatus;

    /** Output buffer. */
    vector<sample_t*> outBuff;
    /** Input buffer. */
    vector<sample_t*> inBuff;
};

} // namespace jack


#endif