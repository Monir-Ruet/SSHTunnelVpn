/**
 * @author      : Monir (monir81.mk@gmail.com)
 * @file        : A
 * @created     : Wednesday Mar 02, 2022 19:30:16 +06
 */

#include<bits/stdc++.h>
#include "INIReader.h"
using namespace std;
typedef long long ll;
const char nl = '\n';
void ssh(string host, string username, string password, string port) {
  string cmd = "sshpass -p " + password + " ssh -o \"ProxyCommand=corkscrew 127.0.0.1 8080 %h %p\" " + username + "@" + host + " -p " + port + " -v -CND 1080 -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null";
  FILE* pipe = popen(cmd.c_str(), "r");
  char buffer[1024];
  string result = "";
  while (!feof(pipe)) {
    if (fgets(buffer, 1024, pipe) != NULL) {
      result += buffer;
      cout << result << nl;
    }
  }
  pclose(pipe);
}

bool multi = 0;

int main() {
  ios_base::sync_with_stdio(false);
  INIReader reader("settings.ini");
  string username = reader.Get("ssh", "username", "UNKNOWN");
  string password = reader.Get("ssh", "password", "UNKNOWN");
  string host = reader.Get("ssh", "host", "UNKNOWN");
  string port = reader.Get("ssh", "port", "UNKNOWN");
  ssh(host, username, password, port);
}

