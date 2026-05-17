#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "triangle/hello_triangle_application.h"


int main() {
	try {

		HelloTriangleApplication app;
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

