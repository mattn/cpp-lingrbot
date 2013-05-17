#include <cstdlib>
#include <boost/network/include/http/server.hpp>
#include <picojson.h>

namespace http = boost::network::http;

struct cpp_bot {
  typedef http::server<cpp_bot> server;

  void
  operator ()(server::request const &request, server::response &response) {
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

    std::string ret = data.str();
    size_t e = ret.find_last_not_of(" ¥t¥r¥n");
    if (e != string::npos) {
      ret = string(ret, 0, e);
    }
    response = server::response::stock_reply(
      server::response::ok, ret
    );
  }

  void
  log(...){
    
  }
};


int
main(){
  const char* p = std::getenv("PORT");
  typedef cpp_bot::server server;
  
  auto address = "localhost";
  auto port = p ? p : "80";

  try{
    cpp_bot handler;
    server server_(address, port, handler, http::_reuse_address=true);
    std::cout << address << ":" << port << " started" << std::endl;
    server_.run();
  }
  catch(std::exception& e){
    std::cout << e.what() << std::endl;
    return 1;
  }

  return 0;
}
