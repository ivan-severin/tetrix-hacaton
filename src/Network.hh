#include <cstdlib>
#include <cstring>
#include <iostream>
#include <functional>
#include <thread>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class ConnectionToWebServer {
  boost::asio::io_service io_service;
  tcp::socket s;
  std::thread loop_runner;

  public: ConnectionToWebServer(std::string ip, std::string port):
    s(io_service)
  {
    try {
      tcp::resolver resolver(io_service);
      boost::asio::connect(s, resolver.resolve({ip, port}));
    }
    catch (std::exception &e)
    {
      std::cerr << "Exception: " << e.what() << "\n";
      throw e;
    }
  }

  public: void sendMsg(std::string msg) {
    std::cout << "sent" << std::endl;
    boost::asio::write(this->s, boost::asio::buffer(msg.c_str(), msg.length()));
  };

  std::string readMsg()
  {
    boost::asio::streambuf buff;
    size_t reply_length = boost::asio::read_until(s, buff, '\n');

    std::istream buff_stream(&buff);
    std::string data;
    std::getline(buff_stream, data);

    std::cout << "Got reply: " << data << std::endl;
    return data;
  };

  public: void launch() {
    this->loop_runner = std::thread([this](){
      this->done = false;

      try {
        while(!done) {
          std::cout << "getting something..." << std::endl;
          auto msg = this->readMsg();

          if (msg == "ping")
            this->sendMsg("pong");
        }
      } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
        done = true;
      }
    });
  }

  public: ~ConnectionToWebServer() {
    this->done = true;
    s.close();
    std::cout << "Joining..." << std::endl;
    // this->loop_runner.join();
    std::cout << "Network connection stopped" <<  std::endl;
  }

  public: bool done=false;
};
