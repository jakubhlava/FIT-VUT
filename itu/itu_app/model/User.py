"""
    User mixin helper pro práci s Flask-login
    Vytvořil: Jakub Hlava (xhlava52)
"""
from flask_login import UserMixin
from itu_app import db


class User(UserMixin):

    def __init__(self, user_data):
        self.user_data = user_data

    def get_id(self):
        return str(self.user_data["_id"])

    def flush(self):
        self.user_data = db.users.find_one({"username": self.user_data['username']})