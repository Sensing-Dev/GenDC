import io

from setuptools import setup, find_packages
import platform
import sysconfig
import os

from setuptools._distutils.util import convert_path

def get_plat():
    if platform.system() == 'Linux':
        plat_form = "manylinux1_x86_64"
    else:
        plat_form = sysconfig.get_platform()
    return plat_form


def get_version():
    if os.environ.get("GENDC_VERSION") is not None:
        tag = os.environ.get("GENDC_VERSION")
    else:
        main_ns = {}
        ver_path = convert_path('./gendc_python/version.py')
        with open(ver_path) as ver_file:
            exec(ver_file.read(), main_ns)
        tag = main_ns["__version__"]
    return tag


def main():
    long_description = io.open("README.md", encoding="utf-8").read()
    setup(
        name="gendc-python",
        author="Momoko Kono",
        author_email="momoko.kono@fixstars.com",
        packages=find_packages(),
        long_description=long_description,
        long_description_content_type="text/markdown",
        url="https://github.com/fixstars/ion-kit",
        version=get_version(),
        python_requires=">=3.9.0",
        license="MIT License",
        license_files=('LICENSE.txt',),
        classifiers=[
            "Development Status :: 3 - Alpha",
            "Programming Language :: Python :: 3",
            "Programming Language :: Python :: 3 :: Only",
            "Programming Language :: Python :: 3.9",
            "Programming Language :: Python :: 3.10",
            "Programming Language :: Python :: 3.11",
            "Programming Language :: Python :: 3.12",
            "License :: OSI Approved :: MIT License",
        ],
        description="GenDC Python package",
        # ext_modules=EmptyListWithLength(),
        options={
            "bdist_wheel": {
                "plat_name": get_plat(),
                "python_tag": "py3",
            },
        },
    )

# This creates a list which is empty but returns a length of 1.
# Should make the wheel a binary distribution and platlib compliant.
class EmptyListWithLength(list):
    def __len__(self):
        return 1

if __name__ == "__main__":
    main()
