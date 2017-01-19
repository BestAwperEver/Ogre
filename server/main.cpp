#include "tcp_server.h"

int main(int argc, char* argv[]) {

	std::srand(time(0));
	setlocale(0, "Rus");

	try {
		tcp_server::getInstance().start();
	}
	catch (boost::system::error_code& e) {
		std::cerr << "Some execption was thrown: " << e.message() << std::endl;
	}

	system("pause");
}