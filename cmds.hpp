#include <MOD.hpp>
#include <string>
#include <util/tokeniser.hpp>

namespace cmd {
extern MOD gModFile;
extern std::string gModFileName;
extern util::tokeniser gTokeniser;

namespace mod {
    void loadFile();
    void writeFile();
    void closeFile();

    void importTexture();

    void importIni();

    void exportTextures();
    void exportMaterials();
    void exportIni();
} // namespace mod

} // namespace cmd