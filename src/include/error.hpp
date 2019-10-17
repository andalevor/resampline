#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>
#include <string>

class error: public std::exception {
	public:
	error(std::string s) : msg{s} {}
	virtual const char* what() const noexcept {return msg.data();}
	private:
	std::string msg;
};

#endif //ERROR_HPP
