#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include <windows.h>

#include <boost/version.hpp>
#include <boost/static_assert.hpp>
#include <boost/assert.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/fstream.hpp>

#include <org/click3/utility.h>

#include "../common/common.h"

namespace {

typedef boost::shared_ptr<boost::remove_pointer<HKEY>::type> SHARED_REG_KEY;

void MyRegCloseKey(HKEY key) {
  const LSTATUS result = ::RegCloseKey(key);
  BOOST_ASSERT(result == ERROR_SUCCESS);
}

SHARED_REG_KEY MyReadRegOpen(HKEY key, const wchar_t *sub_key) {
  HKEY reg_key;
  const LSTATUS result = ::RegOpenKeyExW(key, sub_key, 0, KEY_READ, &reg_key);
  BOOST_ASSERT(result == ERROR_SUCCESS);
  return SHARED_REG_KEY(reg_key, MyRegCloseKey);
}

bool Patch(const boost::filesystem::path &app_dir) {
  const boost::filesystem::path path(app_dir / "th123_ai.exe");
  boost::filesystem::ifstream ifs(path, std::ios::binary);
  if (!ifs.is_open()) {
    return false;
  }
  std::vector<unsigned char> data(boost::filesystem::file_size(path));
  ifs.read(reinterpret_cast<char *>(&data.front()), data.size());
  if (!ifs.good()) {
    return false;
  }
  ifs.close();

  unsigned char * const baseAddr = &data.front();
  const IMAGE_DOS_HEADER * const mz = reinterpret_cast<const IMAGE_DOS_HEADER *>(baseAddr);
  IMAGE_NT_HEADERS32 * const pe = reinterpret_cast<IMAGE_NT_HEADERS32 *>(baseAddr + mz->e_lfanew);
  if (pe->OptionalHeader.MajorSubsystemVersion == 5 && pe->OptionalHeader.MinorSubsystemVersion == 1) {
    return true;
  }
  return true;
}

}

bool GetRegString(std::wstring &out, HKEY key, const wchar_t *sub_key, const wchar_t *value_name) {
  const SHARED_REG_KEY reg = MyReadRegOpen(key, sub_key);
  DWORD type;
  DWORD buffer_size;
  const LSTATUS get_size_result = ::RegQueryValueExW(reg.get(), value_name, NULL, &type, NULL, &buffer_size);
  if(get_size_result != ERROR_SUCCESS || type != REG_SZ || buffer_size == 0) {
    return false;
  }
  out.resize(buffer_size / sizeof(wchar_t));
  const LSTATUS result = ::RegQueryValueExW(reg.get(), value_name, NULL, NULL, reinterpret_cast<BYTE *>(&out.front()), &buffer_size);
  if(result != ERROR_SUCCESS) {
    return false;
  }
  out.resize(::wcsnlen(&out.front(), out.size()));
  return true;
}

std::string history_path;

const char * const list[] = {
  "th123_ai.exe",   "SWRSAddr.ini",     "SwrAddr.ini",  "th123_ai.ini",
  "../History.txt", "../motion.txt",    "Readme.txt",   "../skill.txt",
  "../spell.txt",   "../document.txt",  "../card.txt"
};

bool ParseArgs(unsigned int argc, const char * const *argv) {
  boost::program_options::options_description opt("オプション");
  opt.add_options()
    ("help,h",                                                                "ヘルプを表示")
    ("version,v", boost::program_options::value<std::string>(&history_path),  "versionを取得するhistory.txtのパス");
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opt), vm);
  boost::program_options::notify(vm);
  if(vm.count("help") > 0 || vm.count("version") == 0) {
    std::cout << opt << std::endl;
    return false;
  }
  return true;
}

int main(unsigned int argc, const char * const *argv) {
  if(!::ParseArgs(argc, argv)) {
    return 1;
  }

  org::click3::Utility::SetAppDir();

  std::string app_dir_str;
  if(!org::click3::Utility::GetAppDir(app_dir_str)) {
    return 1;
  }
  const boost::filesystem::path app_dir(app_dir_str);

  std::string version;
  if(!common::GetVersion(version, history_path.c_str())) {
    return 1;
  }

  boost::filesystem::copy_file(app_dir / "../README", app_dir / "Readme.txt", boost::filesystem::copy_option::overwrite_if_exists);

  if (!Patch(app_dir)) {
    return 1;
  }

  const boost::filesystem::path out_path = boost::filesystem::system_complete(app_dir / "../" / (version + ".zip"));

  std::wstring zip_app;
  if(!::GetRegString(zip_app, HKEY_CURRENT_USER, L"Software\\HoeHoe\\Lhaplus", L"InstallPath")) {
    return 1;
  }
  const boost::filesystem::path zip_app_path = boost::filesystem::path(zip_app) / "Lhaplus.exe";

  std::wstring cmd = std::wstring(L"\"") + zip_app_path.wstring() + L"\" /c:zip /o:./";

  BOOST_FOREACH(const char * const p, list) {
    const boost::filesystem::path filename = boost::filesystem::system_complete(app_dir / p);
    wchar_t add_cmd[256];
    WSPRINTF(add_cmd, L" \"%s\"", filename.wstring().c_str());
    cmd.append(add_cmd);
  }

  STARTUPINFOW si;
  PROCESS_INFORMATION pi;
  ::ZeroMemory(&si, sizeof(si));
  ::ZeroMemory(&pi, sizeof(pi));
  si.cb = sizeof(si);
  cmd.resize(cmd.size()+1);
  cmd[cmd.size()-1] = '\0';
  if(0 == ::CreateProcessW(zip_app_path.c_str(), &cmd.front(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    return 1;
  }
  if(WAIT_OBJECT_0 != ::WaitForSingleObject(pi.hProcess, INFINITE)) {
    return 1;
  }
  ::CloseHandle(pi.hThread);
  ::CloseHandle(pi.hProcess);
  ::system((std::string("move th123_ai.zip ") + out_path.string()).c_str());
  return 0;
}
