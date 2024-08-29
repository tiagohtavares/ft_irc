#include <iostream>
#include <stack>
#include <vector>
#include <string>

int main() {
    // Sample stack
    std::stack<std::string> params;
    params.push("one");
    params.push("two");
    params.push("three");

    // Temporary stack to reverse the order
    std::stack<std::string> tempStack;

    // Transfer elements from the original stack to the temporary stack
    while (!params.empty())
	{
        tempStack.push(params.top());
        params.pop();
    }

    // Vector to hold the elements in original order
    std::vector<std::string> ordered;

    // Transfer elements from the temporary stack to the vector
    while (!tempStack.empty())
	{
        ordered.push_back(tempStack.top());
        tempStack.pop();
    }

    // Optional: Print the vector to verify the result
    for (const auto& elem : ordered)
	{
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    return 0;
}
