# SageStore - ERP System for Small-to-Medium Enterprises

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](LINK_TO_BUILD)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Contributors](https://img.shields.io/github/contributors/Sage-Cat/SageStore)](https://github.com/Sage-Cat/SageStore/contributors)

---

## Table of Contents

- [Introduction](#introduction)
  - [Project Overview](#project-overview)
  - [Objectives](#objectives)
  - [Features](#features)
- [Technologies Used](#technologies-used)
  - [Tech Stack](#tech-stack)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Acknowledgments](#acknowledgments)

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

### Installation

> If you are using VS Code you could use my [tasks.json](docs/vscode_config/tasks.json) config file

1. Clone the repo

   ```
   git clone git@gitlab.com:sagecat/SageStore.git
   ```

2. Install prerequisites

   > You can use our VS Code config for [tasks.json](docs/vscode_config/tasks.json)

   ```
   conan install . --output-folder=build --build=missing
   ```
 
3. Go to build folder and run cmake with presets

   ```
   cd build
   cmake .. --preset conan-debug -DBUILD_CLIENT=ON -DBUILD_SERVER=ON
   ```

4. Build the project

   ```
   cmake --build .
   ```

5. Run the application

   ```
   # Windows
   .\_server\SageStoreServer.exe
   .\_client\SageStoreClient.exe

   # Linux
   ./_server/SageStoreServer
   ./_client/SageStoreClient
   ```

## Preparing cetificates for network connectivity

```
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes
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

## Contributing

If you would like to contribute to this project, please fork the repository and submit a pull request.

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

## Project Team

| Team Member        | Role                     | Primary Responsibilities                    |
| ------------------ | ------------------------ | ------------------------------------------- |
| Volodymyr Pavlenko | PM/Tech Lead & Architect | Project planning, architecture design, etc. |
| Plotskykh Danylo   | Backend developer        |                                             |
| Max Kozak          | SQL & C++ Developer      |                                             |

---

## Project Objectives, Milestones, Feature Breakdown, User Stories, Tech Stack, Database, Architecture Design

Please refer to the [Project Documentation](docs/Project_Documentation.md) for detailed information.
