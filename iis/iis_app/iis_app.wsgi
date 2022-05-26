#!/usr/local/bin/python3.10
import configparser
import logging
import sys
import os
logging.basicConfig(stream=sys.stderr)
sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))
from iis_app import create_app

application = create_app()


