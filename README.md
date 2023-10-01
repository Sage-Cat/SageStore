# AutoCenter Inventory and Sales Management System

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](LINK_TO_BUILD)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Contributors](https://img.shields.io/github/contributors/Sage-Cat/SageStore)](https://github.com/Sage-Cat/SageStore/contributors)

---

## Table of Contents

- [About the Project](#about-the-project)
  - [Features](#features)
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

## About the Project

AutoCenter Inventory and Sales Management System is a comprehensive platform designed to automate the tasks of sales, inventory, HR, and analytics in an auto center.

### Features

- Sales Module: Management of sales orders and customer data.
- Inventory Module: Real-time inventory tracking.
- HR Module: HR management including employee data and payroll.
- Analytics Module: Comprehensive analytics dashboard.
- User Management: Role-based access control.
- API Support for future scalability.
- Multi-language support.

### Tech Stack

| Component             | Technology    |
| --------------------- | ------------- |
| Programming Language  | C++17         |
| GUI Framework         | Qt5           |
| Backend Libraries     | Boost         |
| Database              | SQLite        |
| JSON Library          | nlohmann/json |
| Testing Framework     | GTest         |
| Logging Library       | spdlog        |
| Dependency Management | Conan         |

---

## Getting Started

### Prerequisites

- [Conan](https://conan.io/)
- [SQLite](https://www.sqlite.org/index.html)
- [CMake](https://cmake.org/)

### Installation

1. Clone the repository
   ```bash
   git clone https://github.com/Sage-Cat/SageStore
   ```
2. Navigate to the project directory and install dependencies using Conan
   ```bash
   conan install .
   ```
3. Run the initialization scripts
   ```bash
   sqlite3 database.sl3 < ./scripts/create_db.sql
   ```

---

## Usage

Provide instructions on how to use the application, or link to a separate `USAGE.md` document.

---

## Contributing

If you would like to contribute to this project, please fork the repository and submit a pull request.

---

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

---

## Contact

Volodymyr Pavlenko - sagecatbox@gmail.com

Project Link: [https://github.com/Sage-Cat/SageStore](https://github.com/Sage-Cat/SageStore)

---

## Acknowledgments

- Open-source libraries: nlohmann_json, Boost, SQLite3
- Generated with PlantUML

---
