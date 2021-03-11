#pragma once

#include "opfor/core/base.hpp"
#include "stb_image.h"

namespace opfor
{

class ImageLoader
{
  public:
    struct Image
    {
        int width = 0;
        int height = 0;
        int nchannel = 0;
        UniquePtr<uint8_t, std::function<void(uint8_t *)>> data = nullptr;
    };

  public:
    static Image Load(std::string const &path);
    static Image LoadHDR(std::string const &path);
};

} // namespace opfor
