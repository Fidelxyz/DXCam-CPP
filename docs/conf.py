# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os
import subprocess
import sys
from pathlib import Path

DIR = Path(__file__).parent.resolve()

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'DXCam-CPP'
copyright = '2025, Fidel Yin'
author = 'Fidel Yin'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['breathe']

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

breathe_projects = {"DXCam-CPP": "_build/xml/"}
breathe_default_project = "DXCam-CPP"

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'furo'
html_static_path = ['_static']


def generate_doxygen_xml(app):
    build_dir = os.path.join(app.confdir, "_build")
    if not os.path.exists(build_dir):
        os.mkdir(build_dir)

    try:
        subprocess.call(["doxygen", "--version"])
        retcode = subprocess.call(["doxygen"], cwd=app.confdir)
        if retcode < 0:
            sys.stderr.write(f"doxygen error code: {-retcode}\n")
    except OSError as e:
        sys.stderr.write(f"doxygen execution failed: {e}\n")


def setup(app):
    # Add hook for building doxygen xml when needed
    app.connect("builder-inited", generate_doxygen_xml)
