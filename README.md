## 声明：
- 本项目基于天马前端pegasus-frontend项目（作者：mmatyas）修改。
- 如使用本项目修改过的代码，请注明出处（作者：蜜仔，QQ363647869）。
- 特别感谢跳坑者联盟的大光圈先森、Alex的指导与技术支持。

## 项目说明
- 加入各种优化机制，显著提高天马加载速度。
- 由于使用缓存机制加速，如需修改media目录的素材，请手动删除内置存储pegasus-frontend目录下的缓存文件media_cache.json。

![screenshot](etc/promo/screenshot_alpha10.jpg)


# Pegasus Frontend

Pegasus is a graphical frontend for browsing your game library and launching all kinds of emulators from the same place. It's focusing on customizability, cross platform support (including embedded) and high performance.

[![CircleCI status](https://circleci.com/gh/mmatyas/pegasus-frontend.svg?style=shield)](https://circleci.com/gh/mmatyas/pegasus-frontend)
[![AppVeyor status](https://ci.appveyor.com/api/projects/status/github/mmatyas/pegasus-frontend?svg=true&branch=master)](https://ci.appveyor.com/project/mmatyas/pegasus-frontend)
[![GPLv3 license](https://img.shields.io/badge/license-GPLv3-blue.svg)](LICENSE.md)

- [**Main website**](http://pegasus-frontend.org)
- [Documentation](https://pegasus-frontend.org/docs/)
- [Latest releases](https://github.com/mmatyas/pegasus-frontend/releases/)
- [Development thread](https://retropie.org.uk/forum/topic/9598/announcing-pegasus-frontend)
- [Discord](https://discord.gg/KTtzP6y)


## Features

**Open source and cross platform:** runs on Windows, Linux, Mac, Android, all Raspberries, Odroids and possibly on other devices too. Hardware accelerated and works with or without X11.

**Full control over the UI:** themes can completely change everything that is on the screen. Add or remove UI elements, menu screens, animations, whatever. Want to make it look like Kodi? Steam? Any other launcher? No problem. You can even [run your custom shader code](etc/promo/shader_demo.jpg).

**ES2 backward compatibility:** Pegasus can use EmulationStation's gamelist files, no need to invent a new set of tools.

**Features you'd expect:** Gamepad support and config. Multiple aspect ratio support. Portable mode support. Live theme reload. A clock in the corner.


## Installation and setup

You can find downloads and platform-specific notes on the [project's website](http://pegasus-frontend.org), under *Downloads*. Documentation and configuration guides can be found [HERE](http://pegasus-frontend.org/docs/), in particular you might want to take a look on the [Getting started](http://pegasus-frontend.org/docs/user-guide/getting-started/) page.


## Building from source

**Build dependencies**

- C++11 compatible compiler
- Qt 5.15.0 or later, with the following modules:
    - QML and QtQuick2
    - Multimedia
    - SVG
    - SQL (SQLite v3)
- Either SDL (2.0.4 or later) or Qt Gamepad

For more information, please see the detailed [build guide](http://pegasus-frontend.org/docs/dev/build).

**Downloading the source**

The project uses Git submodules. If you've cloned the repository recursively, they are automatically cloned as well. In case you forgot to do that, you can do it manually, like this:

```sh
# call this after every repo update
git submodule update --init
```

**Building**

You can use any Qt-compatible IDEs, eg. Qt Creator to build the project, or you can manually invoke:

```sh
mkdir build && cd build
qmake ..  # qmake [params] [project path], see below
make
make check    # optional, to run tests
make install  # optional
```

**For more details**, including possible build parameters, see the [build documentation](http://pegasus-frontend.org/docs/dev/build).



## License

Pegasus Frontend is available under GPLv3 license. Some included assets, such as product logos and symbols may not be available for commercial usage and/or may require additional permissions from their respective owners for certain legal uses. Furthermore, trademark usage may be limited as per §7 of the GPLv3 license. You can find the details in the [LICENSE](LICENSE.md) file.

All trademarks, service marks, trade names, trade dress, product names and logos are property of their respective owners. All company, product and service names used in this product are for identification purposes only. Use of these names, logos, and brands does not imply endorsement.


## Donate

Pegasus is a free and open source project, and will remain as one. If you would like to provide financial support, you can use the following links. This is completely optional, but every contribution is much appreciated!

[![PayPal](https://www.paypalobjects.com/webstatic/en_US/i/buttons/PP_logo_h_100x26.png)](https://paypal.me/MatyasMustoha)
[![Liberapay](https://liberapay.com/assets/widgets/donate.svg)](https://liberapay.com/mmatyas/donate)
[![Patreon](etc/promo/donate-patreon.png)](https://www.patreon.com/user?u=14424256)
[![Buy Me a Coffee](etc/promo/donate-coffee.png)](https://www.buymeacoffee.com/mmatyas)
