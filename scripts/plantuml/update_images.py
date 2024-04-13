import os
import subprocess
import argparse


def run_plantuml_to_png(source_directory, output_directory):
    current_script_dir = os.path.dirname(os.path.abspath(__file__))
    script_path = os.path.join(current_script_dir, 'plantuml_to_png.py')
    command = ['python', script_path, '-d', source_directory,
               '-o', output_directory, '-p', '5018']
    subprocess.run(command, check=True)


def update_folder_images(source_directory):
    """Search for .puml files and convert them to PNG."""
    run_plantuml_to_png(source_directory + "/plantuml/", source_directory)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Generate png for all puml in folder')
    parser.add_argument('-d', '--directory', required=True,
                        help='Directory to search for PlantUML files')
    args = parser.parse_args()

    update_folder_images(args.directory)
