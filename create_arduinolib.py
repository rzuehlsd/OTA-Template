# package_arduino.py
import shutil
import os
import tempfile

def package_arduino(lib_name="OTA_Template"):
    """
    Prepares an Arduino IDE compatible zip:
    - Creates a temp directory with the correct folder structure
    - Copies config.h and OTA_Test.cpp to examples/OTA_Test, renaming OTA_Test.cpp to OTA_Test.ino
    - Copies all .cpp and .h files from src to tempdir/src
    - Copies library.properties and README.md if they exist
    - Zips the tempdir as OTA_Template.zip
    """
    src_dir = "src"
    examples_src = "src"
    examples_dst = "OTA_Test"
    examples_dir = "examples"
    output_zip = lib_name + "Lib" + ".zip"

    # Create temp directory and required structure
    temp_dir = tempfile.mkdtemp()
    lib_dir = os.path.join(temp_dir, lib_name)
    os.makedirs(os.path.join(lib_dir, "src"))
    example_target_dir = os.path.join(lib_dir, "examples", examples_dst)
    os.makedirs(example_target_dir)

    # Copy and rename OTA_Test.cpp to OTA_Test.ino, and copy config.h
    for file in os.listdir(examples_src):
        if file == "OTA_Test.cpp":
            shutil.copy(os.path.join(examples_src, file), os.path.join(example_target_dir, "OTA_Test.ino"))
        elif file == "config.h":
            shutil.copy(os.path.join(examples_src, file), os.path.join(example_target_dir, file))

    # Copy all .cpp and .h files from src to tempdir/src
    for file in os.listdir(src_dir):
        if file.endswith(".cpp") or file.endswith(".h"):
            if file not in ["OTA_Test.cpp", "config.h"]:
                shutil.copy(os.path.join(src_dir, file), os.path.join(lib_dir, "src", file))

    # Copy library.properties and README.md if they exist
    for extra in ["library.properties", "README.md"]:
        if os.path.exists(extra):
            shutil.copy(extra, lib_dir)

    # Remove old zip if exists
    if os.path.exists(output_zip):
        os.remove(output_zip)

    # Zip the library folder (so the zip contains OTA_Template/...)
    temp_zip = shutil.make_archive(lib_name, 'zip', temp_dir, lib_name)

    # Rename the zip to match output_zip if needed
    if temp_zip != output_zip:
        os.replace(temp_zip, output_zip)

    # Clean up temp dir
    shutil.rmtree(temp_dir)

    print(f"Created {output_zip} for Arduino IDE (with correct folder structure).")

if __name__ == "__main__":
    package_arduino()