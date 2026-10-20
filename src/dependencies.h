// C++ standard, this take about 3s of compile time
#include <iostream>
#include <functional>
#include <cmath>
#include <filesystem>
#include <print>
#include <vector>
#include <string>
#include <expected>
#include <set>
#include <bit>

namespace sfs=std::filesystem;

// gmp lib
#include <gmp.h>
#include <gmpxx.h>
#include <mpfr.h>

namespace gmp {
	using bitcnt=mp_bitcnt_t;
	using fptr=mpf_srcptr;
	using fbig=mpf_class;
	using ibig=mpz_class;
	using iptr=mpz_ptr;
	using exp=mp_exp_t;

	inline void fbig_to_ibig(iptr i,fptr f) { __gmpz_set_f(i,f); }
}

namespace CLI {
	// only linux  CLI themes
	constexpr char RED       []="\e[31m";
	constexpr char GREEN     []="\e[32m";
	constexpr char YELLOW    []="\e[33m";
	constexpr char DARKER    []="\e[2m";
	constexpr char BASHED    []="\e[9m";
	constexpr char SELECTED  []="\e[7m";
	constexpr char UNDERLINED[]="\e[4m";
	constexpr char BLUE      []="\e[34m";
	constexpr char NON       []="\e[0m";
	constexpr char BOLD      []="\e[1m";
}


// this file hold same inlined functions definitions

template<typename ...Args> // print error
inline void colored_print(const char *color, std::format_string<Args...> fmt, Args&&... args) {
	std::print("{}{}{}{}", color, CLI::BOLD,
        std::format(fmt, std::forward<Args>(args)...), CLI::NON);
}

template<typename ...Args> // print error
inline void perr(std::format_string<Args...> fmt, Args&&... args) {
	std::print("{}{}Error{}: ", CLI::RED, CLI::BOLD, CLI::NON);
	std::println(fmt, std::forward<Args>(args)...);
}
template<typename ...Args> // print warning
inline void pwrg(std::format_string<Args...> fmt, Args&&... args) {
	std::print("{}{}Warning{}: ", CLI::YELLOW, CLI::BOLD, CLI::NON);
	std::println(fmt, std::forward<Args>(args)...);
}
template<typename ...Args> // print help
inline void phlp(std::format_string<Args...> fmt, Args&&... args) {
	std::print("{}{}Help{}: ", CLI::GREEN, CLI::BOLD, CLI::NON);
	std::println(fmt, std::forward<Args>(args)...);
}


std::vector<sfs::path> get_files_in(const sfs::path& dir_path)
/* this actuly getting both of files and folders in `dir_path` */ {
	std::vector<sfs::path> files;
	for (const auto& entry : std::filesystem::directory_iterator(dir_path))
		//if (std::filesystem::is_regular_file(entry.status()))
		files.push_back(entry.path());
	return files;
}


// getting lenght of utf8 char
inline std::uint8_t utf8_len(char32_t ch) {
	if ((ch & 0xE0) == 0xC0) return 2;
	else if ((ch & 0xF0) == 0xE0) return 3;
	else if ((ch & 0xF8) == 0xF0) return 4;
	return 1;
}
