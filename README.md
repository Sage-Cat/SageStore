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
- [Architecture Schema](#database-schema)
- [Database Schema](#database-schema)
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
- Data Integrity

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

## Architecture Schema

Will be soon

## Database Schema

Will be soon

---

## Getting Started

### Prerequisites

- [Conan](https://conan.io/)
- [SQLite](https://www.sqlite.org/index.html)
- [CMake](https://cmake.org/)

### Installation

1. Clone the repo

   ```
   git clone https://github.com/Sage-Cat/SageStore.git
   ```

2. Create and go to build folder

   ```
   mkdir build
   cd build
   ```

3. Install prerequisites

   ```
   conan install ..
   ```

4. Run CMake

   ```
   cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:\Qt\6.5.3\mingw_64" .. # or your different generator and Qt path
   ```

5. Build the project

   ```
   cmake --build .
   ```

6. Run the application
   will

## Usage

Usage instructions will be provided at a later stage or link to a separate `USAGE.md` document.

---

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

- Open-source libraries: nlohmann_json, Boost, SQLite3
- Generated with PlantUML
