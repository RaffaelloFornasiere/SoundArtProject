#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
    std::ifstream inFile (argv[1], std::ios_base::in);
    std::ofstream outFile ("fileConverted.mp3", std::ofstream::binary);

    std::string s;
    while(inFile >> s)
    {
        char c = std::stoi(s, 0, 16);

        outFile.write(&c, 1);
    }

}


class a
{
    int b;
    double c;

    a(int b, int c): b(b), c(c){}
    //invece che
    // a(int b, int c) {this->b = b; this->c = c;}
};
