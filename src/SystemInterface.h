#ifndef SYSTEMINTERFACE_H_
#define SYSTEMINTERFACE_H_

#include "Config.h"

#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>

class SystemInterface {
public:

	static std::string exec(const std::string& cmd) {
	    FILE* pipe = popen(cmd.c_str(), "r");
	    if (!pipe) return "ERROR";
	    char buffer[512];
	    std::string result = "";
	    while(!feof(pipe)) {
	    	if(fgets(buffer, 512, pipe) != NULL)
	    		result += buffer;
	    }
	    pclose(pipe);
	    return result;
	}

	static std::string callCurl(const std::string& params) {
		std::string curl(/*"\"" + std::string(HSIR_BASE_DIR) + "/" +*/ Config::getConfig().get<std::string>("config.paths.curl_path") + " -s ");
		return exec(curl + params);
	}

	static std::string callLivestreamer(std::string streamer, std::string quality = "mobile_Medium") {
		static std::string livestreamer("\"" + std::string(HSIR_BASE_DIR) + "/" + Config::getConfig().get<std::string>("config.paths.livestreamer_path") + "\" -j ");
		return exec(livestreamer + "twitch.tv/" + streamer + " " + quality);
	}

	static std::string createPastebin(const std::string& text, const std::string& key) {
		std::string str = text;
		boost::replace_all(str, "\n", "%0d");

		std::string params = "--data-binary \"api_dev_key=" + key + "&api_option=paste&api_paste_code=" + str + "\" http://pastebin.com/api/api_post.php";
		return callCurl(params);
	}

	static std::string createHastebin(const std::string& text) {
		std::ofstream myfile;
		//since transmitting line breaks is difficult(/impossible?)
		myfile.open("temp.txt");
		myfile << text;
		myfile.close();

		std::string params = "--data-binary \"@temp.txt\" hastebin.com/documents";
		std::string keyValue = callCurl(params);
		return "http://hastebin.com/" + keyValue.substr(8, keyValue.find_last_of("/\"") - 8) + ".hs";
	}

	static std::string createStrawpoll(const std::string& text, const std::vector<std::string>& choices, const bool& multi = true) {
		std::string params = "\"title=" + text;
		for (auto& s : choices) {
			params += "&options%5B%5D=" + s;
		}
		params += "&multi=";
		params += (multi)? "true" : "false";
		params += "\"";

		std::string curlParams = "-d " + params + " http://strawpoll.me/ajax/new-poll";
		std::string keyValue = callCurl(curlParams);
		return "http://strawpoll.me/" + keyValue.substr(6, keyValue.find_last_of("}") - 6);
	}

};

#endif /* SYSTEMINTERFACE_H_ */
