/**
\mainpage Afkanerd OpenSMS
\author Sherlock Wisdom
\todo document namespace file

*/

#include "libs/mmcl1_5m5/src/SMS.hpp"
#include "libs/mmcl1_m0d3m/src/Modem.hpp"


using namespace std;
using namespace sms;

map<string, string>
SMS::configurations = SMS::config_file("/usr/local/etc/afkanerd_openos/confs/sms.conf");

map<string, string>
Modem::configurations = Modem::config_file("/usr/local/etc/afkanerd_openos/confs/modem.conf");
string unescape(const string& s)
{
  string res;
  string::const_iterator it = s.begin();
  while (it != s.end())
  {
    char c = *it++;
    if (c == '\\' && it != s.end())
    {
      switch (*it++) {
      case '\\': c = '\\'; break;
      case 'n': c = '\n'; break;
      case 't': c = '\t'; break;
      // all other escapes
      default: 
        // invalid escape sequence - skip it. alternatively you can copy it as is, throw an exception...
        continue;
      }
    }
    res += c;
  }

  return res;
}
int main(int argc, char** argv) {
  string type, number, message, _class = "1", group;
  bool debug;

  if(argc > 1) {
    for(int i=1;i<argc;++i) {
      if((string)(argv[i]) == "--send") {
        type = "send";
      }
      else if((string)(argv[i]) == "--number") {
        number = argv[i+1];
        ++i;
      }
      else if((string)(argv[i]) == "--message") {
        message = argv[i+1];
	message = unescape(message);
        ++i;
        // sms::output::debug("Message: " + message);
      }
      else if((string)(argv[i]) == "--class") {
        _class = argv[i+1];
        ++i;
      }
      else if((string)(argv[i]) == "--group") {
        group = argv[i+1];
        ++i;
      }
      else if((string)(argv[i]) == "--debug") {
        debug = true;
        ++i;
      }
    }
  }
  else {
    sms::output::warning("Not enough arguments...");
    return 1;
  }

  if(type == "send") {
    Modem modemObject;
    modemObject.debug = true;
    auto modems = modemObject.list();
    for(auto modem : modems) {
      if(!group.empty()) {
	      cout << "[MODEM GROUP: " << modem.get_group() << " - " << group << " ]" << endl;
        if(modem.get_group() == group) {
          cout << "FOUND MODEM FOR GROUP!";
          SMS sms(modem.get_index(), number, message);
          sms.set_class(_class);
          if(sms.prepared()) {
            cout << number << " | " << message << " | ";
            sms.send();
            sms.save();
            sms.remove();
            //sms::output::debug("SMS sent!");
          } else {
            sms::output::warning("SMS not prepared");
          }
          break;
        }
      }
      else {
        sms::output::warning("Group not found!");
	cout<<"[GROUP: " << group << "]"<<endl;
        SMS sms(modem.get_index(), number, message);
        sms.set_class(_class);
        if(sms.prepared()) {
          sms.send();
          // sms.save();
          sms::output::debug("SMS sent!");
        } else {
          sms::output::warning("SMS not prepared");
        }
        break;
      }
    }
  }

  else if(type == "receive") {

  }

  else {
    sms::output::warning("No valid command found");
  }

  return 0;
}
