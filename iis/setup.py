from setuptools import setup

setup(
    name="iis_app",
    packages=["iis_app"],
    include_package_data=True,
    install_requires=[
        "flask",
        "flask-sqlalchemy",
        "flask-login",
        "psycopg2-binary",
        "flask-bootstrap",
        "flask-wtf"
    ]
)