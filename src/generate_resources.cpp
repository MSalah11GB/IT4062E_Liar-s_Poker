#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

int main()
{
    namespace fs = filesystem;

    const fs::path imagesDir = "src/shared/resources/images";
    const fs::path qrcFile = "./src/shared/resources/resources.qrc";

    vector<fs::path> files;
    for (auto &p : fs::recursive_directory_iterator(imagesDir))
    {
        if (p.is_regular_file() && p.path().extension() == ".png")
        {
            files.push_back(p.path());
        }
    }

    ofstream out(qrcFile);
    out << "<RCC>\n    <qresource prefix=\"/\">\n";

    for (auto &f : files)
    {
        out << "        <file>" << imagesDir.filename().string() << "/" << f.filename().string() << "</file>\n";
    }

    out << "    </qresource>\n</RCC>\n";

    cout << "Generated: " << qrcFile << "\n";
    return 0;
}
