# SageStore - ERP System for Small-to-Medium Enterprises

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](LINK_TO_BUILD)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Contributors](https://img.shields.io/github/contributors/Sage-Cat/SageStore)](https://github.com/Sage-Cat/SageStore/contributors)

---

## Table of Contents

- [SageStore - ERP System for Small-to-Medium Enterprises](#sagestore---erp-system-for-small-to-medium-enterprises)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
    - [Project Overview](#project-overview)
    - [Objectives](#objectives)
    - [Features](#features)
  - [Technologies Used](#technologies-used)
    - [Tech Stack](#tech-stack)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
  - [Building](#building)
  - [Testing](#testing)
  - [Manual build process](#manual-build-process)
  - [Contributing](#contributing)
  - [License](#license)
  - [Contact](#contact)
  - [Acknowledgments](#acknowledgments)
  - [Detailed project documentation](#detailed-project-documentation)

---

## Introduction

### Project Overview

The project aims to build an ERP system to manage core business processes for SMEs, including inventory, sales, purchase, HR, and accounting. It will be a client-server desktop application developed using C++20 and Qt 6.5.

### Objectives

- Efficient Sales Management
- Inventory Control
- Scalable Analytics
- User-Friendly UI

### Features

- Sales Module: Management of sales orders and customer data.
- Inventory Module: Real-time inventory tracking.
- HR Module: HR management including employee data and payroll.
- Analytics Module: Comprehensive analytics dashboard.
- User Management: Role-based access control.
- API Support for future scalability.
- Multi-language support.

## Technologies Used

### Tech Stack

| Layer        | Technology   |
| ------------ | ------------ |
| Frontend     | Qt 6.5       |
| Backend      | C++20        |
| Database     | SQLite       |
| Build Tool   | CMake, Conan |
| Version Ctrl | Git          |
| CI/CD        | Jenkins      |

---

## Getting Started

### Prerequisites

- [Qt 6.5](https://wiki.qt.io/Qt_6.5_Release)
- [Conan](https://conan.io/)
- [SQLite](https://www.sqlite.org/index.html)
- [CMake](https://cmake.org/)
- [Boost](https://www.boost.org/)
- [GTest](http://google.github.io/googletest/)

_Check conanfile.py if you need versions of specific libs, that were used_

## Building

> If you are using VS Code you could use my [tasks.json](tools/vscode_config/tasks.json) config file

1. Clone repo:

   ```
   git clone git@gitlab.com:sagecat/SageStore.git
   ```

2. Automated build:

   ```
   python3 build.py
   ```
   You can also do [manual build process](#manual-build-process)

3. Run the application

   ```
   # Windows
   .\_server\SageStoreServer.exe
   .\_client\SageStoreClient.exe

   # Linux
   ./_server/SageStoreServer
   ./_client/SageStoreClient
   ```

## Testing

Run tests

```
# Inside build folder
ctest

# More detailed output
ctest --verbose

# Run specific test
.\_client\unit\test_SageStoreClient.exe
# or
./_client/unit/test_SageStoreClient
```

## Manual build process
1. Install conan debs

   ```
   conan install . --output-folder=build --build=missing
   ```

   Or if your conan have sudo rights:
   ```
   conan install . --output-folder=build --build=missing -c tools.system.package_manager:mode=install
   ```

   If you have already installed qt manualy, add 
   ```
   QT_DIR='your_path_to_qt_with_gcc_64'
   export QT_DIR
   ```
   into your .bashrc file.
   INPORTANT: version of qt is 6.6.3
 
2. Go to build folder and run cmake with presets

   ```
   cd build
   cmake .. --preset conan-debug -DBUILD_CLIENT=ON -DBUILD_SERVER=ON -DBUILD_TESTS=ON
   ```

3. Build the project

   ```
   cmake --build .
   ```

## Contributing

All contributing rules you can find in [CONTRIBUTING.md](CONTRIBUTING.md)

---

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

---

## Contact

sagecatbox@gmail.com

Project Link: [https://github.com/Sage-Cat/SageStore](https://github.com/Sage-Cat/SageStore)

---

## Acknowledgments

- Open-source libraries: nlohmann_json, Boost, SQLite3, GTest
- Generated with PlantUML

---

## Detailed project documentation

Please refer to the [Project Documentation](docs/Project_Documentation.md) for detailed information.
