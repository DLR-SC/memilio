import os
import sys

from setuptools import Command, find_packages, setup


class PylintCommand(Command):
    """
    Custom command to run pylint and get a report as html.
    """
    description = "Runs pylint and outputs the report as html."
    user_options = []

    def initialize_options(self):
        from pylint.reporters.json_reporter import JSONReporter
        from pylint.reporters.text import ParseableTextReporter, TextReporter
        from pylint_json2html import JsonExtendedReporter

        self.lint_modules = ["memilio/"]
        self.out_format = "extendedjson"

        self.REPORTERS = {
            "parseable": (ParseableTextReporter, "build_pylint/pylint_parseable.txt"),
            "text": (TextReporter, "build_pylint/pylint.txt"),
            "json": (JSONReporter, "build_pylint/pylint.json"),
            "extendedjson": (JsonExtendedReporter, "build_pylint/pylint_extended.json")
        }

    def finalize_options(self):
        self.reporter, self.out_file = self.REPORTERS.get(
            self.out_format)  # , self.REPORTERS.get("parseable"))

    def run(self):
        os.makedirs("build_pylint", exist_ok=True)

        # Run pylint
        from pylint import lint
        with open(self.out_file, "w", encoding="utf-8") as report_file:
            options = ["--rcfile=../pylintrc", *self.lint_modules]

            lint.Run(options, reporter=self.reporter(
                report_file), do_exit=False)


# Python-magic needs DLLs for libmagic. They have to be installed only on windows.
def get_pymagic():
    if sys.platform == 'win32':
        return 'python-magic-bin'
    else:
        return 'python-magic'

setup(
    packages=find_packages(where=os.path.dirname(os.path.abspath(__file__))),
    test_suite='memilio.epidata_test',
    dependencies=[
        # pandas 2.0 is minimum for CoW
        'pandas>=2.0.0',
        # FutureWarning of pandas that pyarrow will be required in a future release
        'pyarrow',
        'matplotlib',
        'tables',
        # smaller numpy versions cause a security issue, 1.25 breaks testing with pyfakefs
        'numpy>=1.22,<1.25',
        'openpyxl',
        'xlrd',
        'xlsxwriter',
        'requests',
        'pyxlsb',
        'wget',
        'twill==3.1',
        'PyQt6',
        'python-calamine',
        get_pymagic()
    ],
    cmdclass={
        'pylint': PylintCommand
    },
)
