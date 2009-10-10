#include "picojson.h"
#include "nanowww.h"

using namespace std;
using namespace picojson;
using namespace nanowww;

#ifdef _WIN32
static char* utf8_to_str_alloc(const char* utf8) {
    size_t in_len = strlen(utf8);
    wchar_t* wcsdata;
    char* mbsdata;
    size_t mbssize, wcssize;

    wcssize = MultiByteToWideChar(CP_UTF8, 0, utf8, in_len,  NULL, 0);
    wcsdata = (wchar_t*) malloc((wcssize + 1) * sizeof(wchar_t));
    wcssize = MultiByteToWideChar(CP_UTF8, 0, utf8, in_len, wcsdata, wcssize + 1);
    wcsdata[wcssize] = 0;

    mbssize = WideCharToMultiByte(GetACP(), 0, (LPCWSTR) wcsdata, -1, NULL, 0, NULL, NULL);
    mbsdata = (char*) malloc((mbssize + 1));
    mbssize = WideCharToMultiByte(GetACP(), 0, (LPCWSTR) wcsdata, -1, mbsdata, mbssize, NULL, NULL);
    mbsdata[mbssize] = 0;
    free(wcsdata);
    return mbsdata;
}
#endif

value get_json(string url, map<string, string>& postdata) {
  value v;
  char error[256];
  string data;

  Client client;
  Response res;
  if (client.send_post(&res, url.c_str(), postdata)) {
    const char* ptr = res.content().c_str();
    string err = parse(v, ptr, ptr + strlen(ptr));
    if (!err.empty()) cerr << err << endl;
  }
  return v;
}

int
main(int argc, char* argv[]) {
  map<string, string> postdata;
  value v;
  bool loop;

  if (argc != 4) {
      std::cerr << "usage: kankeinaibot [nick] [channel] [word]" << std::endl;
      return -1;
  }
  string nick = argv[1];
  string channel = argv[2];
  string word = argv[3];

  postdata.clear();
  postdata["nick"] = nick;
  v = get_json("http://webchat.freenode.net/e/n", postdata);
  string sid = v.get<array>()[1].get<string>();

  loop = true;
  postdata.clear();
  postdata["s"] = sid;
  while (loop) {
    v = get_json("http://webchat.freenode.net/e/s", postdata);
    array a = v.get<array>();
    for (array::iterator it = a.begin(); it != a.end(); it++) {
      if (!it->is<array>()) continue;
      array line = it->get<array>();
      if (line[0].to_str() == "c") {
        if (line[1].to_str() == "443") {
            postdata["nick"] += "_";
            v = get_json("http://webchat.freenode.net/e/n", postdata);
            sid = v.get<array>()[1].get<string>();
            break;
        }
        if (line[1].to_str() == "376") {
            loop = false;
            break;
        }
      }
    }
  }

  loop = true;
  postdata.clear();
  postdata["c"] = "JOIN #";
  postdata["c"] += channel;
  postdata["s"] = sid;
  v = get_json("http://webchat.freenode.net/e/p", postdata);
  while (loop) {
    v = get_json("http://webchat.freenode.net/e/s", postdata);
    array a = v.get<array>();
    for (array::iterator n = a.begin(); n != a.end(); n++) {
      array line = n->get<array>();
      if (line[0].to_str() == "c") {
        array aa = line[3].get<array>();
        if (aa.size() > 1 && aa[0].to_str() != nick) {
          cout << aa[0].serialize() << endl;
          cout << aa[1].serialize() << endl;
          if (!strstr(aa[1].to_str().c_str(), word.c_str())) {
              map<string, string> saydata;
              saydata["s"] = sid;
              std::stringstream s;
              s << "PRIVMSG #" << channel << " :" <<
                  "それ" << word << "関係ないだろ！jk";
              saydata["c"] = s.str();
              cout << saydata["c"] << endl;
              v = get_json("http://webchat.freenode.net/e/p", saydata);
          }
        }
        string s = line[3].serialize();
#ifdef _WIN32
        char* p = utf8_to_str_alloc(s.c_str());
        cout << p;
        cout << " ";
        free((void*)p);
        cout << endl;
#else
        cout << s << endl;
#endif
      }
    }
  }
  
  return 0;
}
