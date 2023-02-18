#include "IBroker.h"
// THIS is To be scrapped in favor of an HTTPClient class that this class will hold. Composition not inheritence.
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

class TradierBroker: public IBroker{ // fix to keep socket alive ??? and allow for repeated requests 
    boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> address;
    int version;
    std::string host;
    std::string port;
    std::string target;
    public:
    TradierBroker(std::string brokerAPIToken){
            /*
            "    http-client-sync www.example.com 80 /\n" <<
            "    http-client-sync www.example.com 80 / 1.0\n";*/
            net::io_context ioc;
            tcp::resolver resolver(ioc);
            host = "www.google.com"; // host: www.example.com
            port = "80"; // port: 80
            target = "/"; // target: /
            version = 11;//argc == 5 && !std::strcmp("1.0", argv[4]) ? 10 : 11; // http verion: 1.1 default
            // Look up the domain name
            address = resolver.resolve(host, port);
    };
    // Performs an HTTP GET and prints the response
    void sendRequestAndGetResponse(){
        try{
            // The io_context is required for all I/O
            net::io_context ioc;
            // These objects perform our I/O
            beast::tcp_stream stream(ioc);
            // Make the connection on the IP address we get from a lookup
            stream.connect(address);

            // Set up an HTTP GET request message
            http::request<http::string_body> req{http::verb::get, target, version};
            req.set(http::field::host, host);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

            // Send the HTTP request to the remote host
            http::write(stream, req);

            // This buffer is used for reading and must be persisted
            beast::flat_buffer buffer;

            // Declare a container to hold the response
            http::response<http::dynamic_body> res;

            // Receive the HTTP response
            http::read(stream, buffer, res);

            // Write the message to standard out
            std::cout << res << std::endl;

            // Gracefully close the socket
            beast::error_code ec;
            stream.socket().shutdown(tcp::socket::shutdown_both, ec);

            // not_connected happens sometimes
            // so don't bother reporting it.
            //
            if(ec && ec != beast::errc::not_connected)
                throw beast::system_error{ec};

            // If we get here then the connection is closed gracefully
        }
        catch(std::exception const& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};
