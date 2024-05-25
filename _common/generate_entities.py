import argparse
from pathlib import Path
import re
import shutil
import subprocess

def camel_to_snake(name):
    s1 = re.sub(r'(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', s1).upper()

def snake_to_camel(name):
    return ''.join(word.capitalize() for word in name.split('_'))

def parse_sql_file(file_path):
    table_pattern = re.compile(r'CREATE TABLE (\w+)\s*\((.*?)\);', re.S)
    field_pattern = re.compile(r'^\s*(\w+)\b.*?(?:,|$)', re.M)

    content = file_path.read_text()

    tables = {}
    for table, fields in table_pattern.findall(content):
        field_lines = [line for line in fields.splitlines() if not line.strip().startswith('FOREIGN KEY')]
        field_names = [field_pattern.match(line).group(1) for line in field_lines if field_pattern.match(line)]
        tables[table] = field_names
    return tables

def clean_directory(directory):
    if directory.exists():
        shutil.rmtree(directory)
    directory.mkdir(parents=True, exist_ok=True)

def generate_hpp_files(tables, output_dir):
    template = '''#pragma once

#include <string>

namespace Common {{
namespace Entities {{

struct {struct_name} {{
    static constexpr const char *TABLE_NAME  = "{table_name}";
{key_definitions}

{field_definitions}
}};

}}  // namespace Entities
}}  // namespace Common
'''
    filenames = []
    hpp_dir = output_dir / 'include' / 'common' / 'Entities'

    for table, fields in tables.items():
        struct_name = table  # Struct name should be the same as table name
        table_name = table
        key_definitions = "\n".join([f'    static constexpr const char *{camel_to_snake(field)}_KEY = "{field}";' for field in fields])
        field_definitions = "\n".join([f'    std::string {field}{{}};' for field in fields])

        hpp_content = template.format(
            struct_name=struct_name, 
            table_name=table_name, 
            key_definitions=key_definitions, 
            field_definitions=field_definitions
        )

        hpp_filename = hpp_dir / f"{table}.hpp"
        hpp_filename.parent.mkdir(parents=True, exist_ok=True)  # Ensure the directory exists
        hpp_filename.write_text(hpp_content, encoding='utf-8')
        filenames.append(hpp_filename)

    return filenames

def generate_json_cpp_file(tables, output_dir):
    template_header = '''{headers}
    
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using namespace std;

namespace Common {{
namespace Entities {{

'''
    template_to_json = '''// --- {struct_name} ---
void to_json(json &j, const {struct_name} &u)
{{
    j = json{{{json_serialization}}};
}}

'''
    template_from_json = '''void from_json(const json &j, {struct_name} &u)
{{
    {json_deserialization}
}}

'''

    template_footer = '''
}}  // namespace Entities
}}  // namespace Common
'''

    cpp_filename = output_dir / 'src' / "EntitiesJson.cpp"
    cpp_filename.parent.mkdir(parents=True, exist_ok=True)  # Ensure the directory exists
    cpp_content = ""

    headers = set()

    for table, fields in tables.items():
        struct_name = table  
        header = f"common/Entities/{table}.hpp"
        headers.add(header)
        json_serialization = ", ".join([f'{{{struct_name}::{camel_to_snake(field)}_KEY, u.{field}}}' for field in fields])
        json_deserialization = "\n    ".join([f'j.at({struct_name}::{camel_to_snake(field)}_KEY).get_to(u.{field});' for field in fields])

        cpp_content += template_to_json.format(struct_name=struct_name, json_serialization=json_serialization)
        cpp_content += template_from_json.format(struct_name=struct_name, json_deserialization=json_deserialization)

    header_includes = '\n'.join([f'#include "{header}"' for header in headers])
    cpp_content = template_header.format(headers=header_includes) + cpp_content + template_footer

    cpp_filename.write_text(cpp_content, encoding='utf-8')
    return [cpp_filename]

def format_generated_files(file_paths):
    command = ['clang-format', '-i'] + [str(file_path) for file_path in file_paths]
    subprocess.run(command, check=True)

def main():
    parser = argparse.ArgumentParser(
        description='Generate C++ header files from SQL definitions.',
        epilog='Example: python3 generate_entities.py ../scripts/create_db.sql AutoGenerated'
    )
    parser.add_argument('sqlfile', type=Path, help='The SQL file to process. This file should contain CREATE TABLE statements.')
    parser.add_argument('outputdir', type=Path, help='The directory to output the HPP files and the JSON CPP file. The directory will be cleaned if it exists.')

    args = parser.parse_args()

    clean_directory(args.outputdir)
    tables = parse_sql_file(args.sqlfile)
    generated_hpp_files = generate_hpp_files(tables, args.outputdir)
    generated_cpp_files = generate_json_cpp_file(tables, args.outputdir)

    format_generated_files(generated_hpp_files + generated_cpp_files)

    print("Generated and formatted files:")
    for filename in generated_hpp_files + generated_cpp_files:
        print("+ " + str(filename.relative_to(args.outputdir)))

if __name__ == "__main__":
    main()
