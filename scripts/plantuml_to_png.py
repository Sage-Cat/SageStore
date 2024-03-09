import os
import argparse
from plantuml import deflate_and_encode
import requests


def find_puml_files(directory, depth=1):
    """Find all .puml files within the specified directory up to a given depth."""
    puml_files = []
    for root, dirs, files in os.walk(directory):
        if root[len(directory):].count(os.sep) < depth:
            for file in files:
                if file.endswith(".puml"):
                    puml_files.append(os.path.join(root, file))
        else:
            del dirs[:]
    return puml_files


def generate_png(file_path, server_url, output_dir):
    """Generate a PNG file from a PlantUML file and save it to the specified output directory."""
    with open(file_path, 'rb') as f:
        encoded = deflate_and_encode(f.read())
    response = requests.get(f"{server_url}/png/{encoded}")
    if response.status_code == 200:
        # Generate the output path by replacing the source directory with the output directory
        output_path = os.path.join(output_dir, os.path.basename(
            file_path).rsplit('.', 1)[0] + '.png')
        # Ensure the output directory exists
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        with open(output_path, 'wb') as f:
            f.write(response.content)
        print(f"Generated PNG for {file_path} at {output_path}")
    else:
        print(f"Failed to generate PNG for {file_path}")


def main():
    parser = argparse.ArgumentParser(
        description='Generate PNG files from PlantUML files using a PlantUML server.')
    parser.add_argument('-d', '--directory', required=True,
                        help='Directory to search for PlantUML files')
    parser.add_argument('-o', '--output', required=True,
                        help='Output directory for PNG files')
    parser.add_argument('-p', '--port', default='5018',
                        help='Port on which the PlantUML server is running')
    args = parser.parse_args()

    server_url = f"http://localhost:{args.port}"
    puml_files = find_puml_files(args.directory)

    for file_path in puml_files:
        generate_png(file_path, server_url, args.output)


if __name__ == "__main__":
    main()
