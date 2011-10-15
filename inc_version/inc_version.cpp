
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <string>

#include <Windows.h>

#include <boost/version.hpp>
#include <boost/static_assert.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

#include <org/click3/utility.h>

#include "../common/common.h"

unsigned int GetInnerVersion(const char *path) {
	const boost::shared_ptr<FILE> fp = org::click3::Utility::MyFOpen(path, "r");
	BOOST_ASSERT(fp);
	char line[256];
	const char * const result = ::fgets(line, sizeof(line), fp.get());
	BOOST_ASSERT(result != NULL);
	BOOST_ASSERT(STRLEN(line) >= 17);
	BOOST_ASSERT(::isdigit(line[16]));
	return ::atoi(&line[16]);
}

std::string history_path;
std::string out_path;

bool ParseArgs(unsigned int argc, const char * const *argv) {
	boost::program_options::options_description opt("オプション");
	opt.add_options()
		("help,h",										"ヘルプを表示")
		("version,v",	boost::program_options::value<std::string>(&history_path),	"versionを取得するhistory.txtのパス")
		("out,o",	boost::program_options::value<std::string>(&out_path),		"出力先");
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opt), vm);
	boost::program_options::notify(vm);
	if(vm.count("help") > 0 || vm.count("version") == 0 || vm.count("out") == 0) {
		std::cout << opt << std::endl;
		return false;
	}
	return true;
}


int main(unsigned int argc, const char * const *argv) {
	if(!::ParseArgs(argc, argv)) {
		return 1;
	}

	std::string version;
	if(!common::GetVersion(version, history_path.c_str())) {
		return 1;
	}
	const unsigned int inner_version = GetInnerVersion(out_path.c_str());

	const boost::shared_ptr<FILE> fp = org::click3::Utility::MyFOpen(out_path.c_str(), "w");
	if(!fp) {
		return 1;
	}
	const char * const template_text = 
"#define VERSION %d\n"
"#define OPEN_VERSION \"%s\"\n";
	::fprintf(fp.get(), template_text, inner_version + 1, version.c_str());
	return 0;
}

