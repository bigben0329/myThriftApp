#include "json.h"
#include <iostream>

using namespace std;
using namespace Json;

int main()
{
	Value root;

	int i = 0;
	root["world"][i] = 2;
	root["world"][1] = "\"<root>yes</root>\"";

	XMLWriter writer;
	cout << writer.write(root);
}
