#!/usr/local/bin/python3.10
"""
WSGI skript pro nasazení do Apache2 mod_wsgi serveru
Vytvořil: Jakub Hlava (xhlava52)
"""
import logging
import sys
import os
logging.basicConfig(stream=sys.stderr)
sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))
from itu_app import create_app

application = create_app()