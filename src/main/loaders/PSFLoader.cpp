/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "PSFLoader.h"

#include <unordered_map>
#include <fmt/compile.h>
#include <fmt/format.h>

#include "LogManager.h"
#include "components/PSFFile2.h"

constexpr int PSF1_VERSION = 0x1;
constexpr int GSF_VERSION = 0x22;
constexpr int SNSF_VERSION = 0x23;
constexpr int NDS2SF_VERSION = 0x24;
constexpr int NCSF_VERSION = 0x25;
const std::unordered_map<int, size_t> data_offset = {{PSF1_VERSION, 0x800},
                                                     {GSF_VERSION, 0x0C},
                                                     {SNSF_VERSION, 0x08},
                                                     {NDS2SF_VERSION, 0x08},
                                                     {NCSF_VERSION, 0x0}};

void PSFLoader::apply(const RawFile *file) {
    if (std::equal(file->begin(), file->begin() + 3, "PSF")) {
        uint8_t version = file->get<u8>(3);
        if (data_offset.find(version) != data_offset.end()) {
            psf_read_exe(file, version);
        }
    }
}

/*
 * This isn't the way the spec advises loading a PSF file;
 * however the following is perfectly fine for our purposes.
 */
void PSFLoader::psf_read_exe(const RawFile *file, int version) {
    try {
        PSFFile2 psf(*file);
        std::filesystem::path basepath(file->path());
        auto libtag = psf.tags().find("_lib");
        if (libtag != psf.tags().end()) {
            auto newpath = basepath.replace_filename(libtag->second).string();
            auto newfile = std::make_shared<DiskFile>(newpath);
            enqueue(newfile);

            /* Look for additional libraries in the same folder */
            auto libstring = fmt::compile("_lib{:d}");
            int i = 1;
            while (true) {
                libtag = psf.tags().find(fmt::format(libstring, i++));
                if (libtag != psf.tags().end()) {
                    newpath = basepath.replace_filename(libtag->second).string();
                    newfile = std::make_shared<DiskFile>(newpath);
                    enqueue(newfile);
                } else {
                    break;
                }
            }
        }

        if (!psf.exe().empty()) {
            auto newfile = std::make_shared<VirtFile>(
                reinterpret_cast<const u8 *>(psf.exe().data()) + data_offset.at(version),
                psf.exe().size() - data_offset.at(version), file->name(), file->path(), file->tag);
            enqueue(newfile);
        }
    } catch (std::exception e) {
        L_ERROR(e.what());
        return;
    }
}
