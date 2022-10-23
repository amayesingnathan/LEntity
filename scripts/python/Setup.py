import sys
import os
import subprocess
import platform

from PremakeSetup import PremakeConfiguration as PremakeRequirements

os.chdir(f"{sys.path[0]}/../..") # Change working dir to repo root
premakeInstalled = PremakeRequirements.Validate()

if (premakeInstalled):
    print("\nRunning premake...")
    if platform.system() == "Windows":
        subprocess.call([os.path.abspath("./scripts/gen-projects/msvc.bat"), "nopause"])
    elif platform.system() == "Linux":
        subprocess.call([os.path.abspath("./scripts/gen-projects/gcc.sh"), "nopause"])

    print("\nSetup completed!")
else:
    print("Labyrinth requires Premake to generate project files.")