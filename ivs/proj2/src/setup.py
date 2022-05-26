from setuptools import setup, find_packages

setup(
    name="AlphaCalc",
    version="1.0",
    description="Kalkulačka s převodníkem měn",
    author="Jakub Hlava, Thi Bao Ngoc Vu, Lukáš Kraus",
    author_email="xhlava52@vutbr.cz, xvuthi00@vutbr.cz, xkraus13@vutbr.cz",
    url="https://github.com/jakubhlava/alphacalc",
    license="GNU GPL v3.0",
    packages=['alphacalc','alphacalc.GUI','alphacalc.GUI.resources','alphacalc.alphalib','alphacalc.alphalib.utils'],
    package_data={
        "alphacalc.GUI.resources": ["*"],
    },
    install_requires=[
        'requests',
        'PySide2',
    ],
    entry_points={
    'console_scripts': [
        'alphacalc = alphacalc.__main__:main',
    ],
    },
    python_requires='>=3.5, !=3.8.0, !=3.6.0, !=3.6.1'
)