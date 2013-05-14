#include <boost/network/include/http/server.hpp>
#include <picojson.h>

struct cpp_bot {
  typedef boost::network::http::server<cpp_bot> server;

  void
  operator ()(server::request const &request, server::response &response) {
    namespace http = boost::network::http;
    typedef server::string_type string;
    string body = request.body;
    std::ostringstream data;

    std::cout << body << std::endl;
    picojson::value v;
    string err;
    picojson::parse(v, body.begin(), body.end(), &err);
    if (!err.empty()) {
      response = server::response::stock_reply(
        server::response::bad_request, "invalid request"
      );
      return;
    }

    if (!v.is<picojson::object>()) {
      response = server::response::stock_reply(
        server::response::bad_request, "invalid request"
      );
      return;
    }
    picojson::array arr;
    picojson::object obj;
    
    obj = v.get<picojson::object>();
    v = obj["events"];
    if (!v.is<picojson::array>()) {
      response = server::response::stock_reply(
        server::response::bad_request, "invalid request"
      );
      return;
    }

    arr = v.get<picojson::array>();
    BOOST_FOREACH(auto x, arr) {
      if (!x.is<picojson::object>()) {
        continue;
      }
      picojson::value message = x.get<picojson::object>()["message"];
      if (!message.is<picojson::object>()) {
        continue;
      }
      picojson::value text = message.get<picojson::object>()["text"];
      if (!text.is<string>()) {
        continue;
      }
      string str = text.get<string>();
      if (str.find("cpp", 0) != string::npos) {
        data << "くぴぴ\n";
      }
    }

    response = server::response::stock_reply(
      server::response::ok, data.str()
    );
  }

  void
  log(...){
    
  }
};


int
main(){
  typedef cpp_bot::server server;
  
  auto address = "localhost";
  auto port = "11614";
  try{
    cpp_bot handler;
    server server_(address, port, handler);
    server_.run();
  }
  catch(std::exception& e){
    std::cout << e.what() << std::endl;
    return 1;
  }

  return 0;
}
