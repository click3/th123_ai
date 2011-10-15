
#include <stdio.h>

#include <vector>
#include <string>

#include <Windows.h>

#include <boost/version.hpp>
#include <boost/static_assert.hpp>
#include <boost/shared_ptr.hpp>

#include <org/click3/utility.h>

#include "common.h"

namespace common {

bool GetVersion(std::string &out, const char *path) {
	const boost::shared_ptr<FILE> fp = org::click3::Utility::MyFOpen(path, "r");
	if(!fp) {
		return false;
	}
	char line[256];
	if(NULL == ::fgets(line, sizeof(line), fp.get())) {
		return false;
	}
	const unsigned int len = STRLEN(line);
	for(unsigned int i = 0; i < len; i++) {
		if(line[i] == '\t') {
			const unsigned int start = i + 1;
			const unsigned int copy_len = len - start + (line[len-1] == '\n' ? -1 : 0);
			out.assign(&line[start], copy_len);
			return true;
		}
	}
	return false;
}

}
