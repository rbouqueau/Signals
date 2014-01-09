#include "print.hpp"
#include "internal/log.hpp"


Print* Print::create(const Param &parameters) {
	return new Print();
}

bool Print::process(Data *data) {
	Log::get(Log::Error) << "Print: received data of size: " << data->size() << std::endl;
	delete data;
	return true;
}

bool Print::handles(const std::string &url) {
	return Print::canHandle(url);
}

bool Print::canHandle(const std::string &url) {
	return true;
}

Print::Print() {
}