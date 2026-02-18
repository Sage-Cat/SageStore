import os
import subprocess
import argparse


def run_plantuml_to_png(source_directory):
    current_script_dir = os.path.dirname(os.path.abspath(__file__))
    script_path = os.path.join(current_script_dir, 'render_puml_docker.sh')
    command = [script_path, source_directory, 'png']
    subprocess.run(command, check=True)


def update_folder_images(source_directory):
    """Search for .puml files and convert them to PNG via Dockerized PlantUML."""
    run_plantuml_to_png(source_directory)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Generate png for all puml in folder')
    parser.add_argument('-d', '--directory', required=True,
                        help='Directory to search for PlantUML files')
    args = parser.parse_args()

    update_folder_images(args.directory)
