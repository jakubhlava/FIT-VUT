from setuptools import setup

setup(
    name="itu_app",
    packages=["itu_app"],
    include_package_data=True,
    install_requires=[
        "flask",
        "flask_login",
        "pymongo"
    ]
)