"""
Inicializace aplikace
Vytvořil: Jakub Hlava (xhlava52)
"""
from flask import Flask, render_template
from flask_login import LoginManager
import os
import configparser
from pymongo import MongoClient

# config reading from settings.conf file in root directory of module
config = configparser.ConfigParser()
lm = LoginManager()
config.read(os.path.join(os.path.dirname(__file__), "settings.conf"))
client = MongoClient(config["db"]["uri"])
db = client[config["db"]["db"]]

def create_app():
    app = Flask(__name__)
    app.secret_key = config["db"]["secret_key"]

    lm.init_app(app)

    from itu_app.views.builder import bp as builder_bp
    from itu_app.views.index import bp as index_bp
    from itu_app.views.question import bp as question_bp
    from itu_app.views.packages import bp as packages_bp

    app.register_blueprint(builder_bp)
    app.register_blueprint(index_bp)
    app.register_blueprint(question_bp)
    app.register_blueprint(packages_bp)

    from itu_app.backend import user, deck

    @lm.user_loader
    def load_user(id_):
        return user.get_object_by_id(id_)

    from flask import redirect, url_for, flash

    @app.errorhandler(AttributeError)
    def handle_attr_error(e):
        if "AnonymousUserMixin" in str(e):
            with app.app_context():
                flash("Pro tuto akci se prosím přihlaš!", "danger")
                return redirect(url_for("index.main"))

    @app.errorhandler(401)
    def handle_401(e):
        with app.app_context():
            flash("Pro tuto akci se prosím přihlaš!", "danger")
            return redirect(url_for("index.main"))

    if db.system.count_documents({"name": "kv_store"}) == 0:
        db.system.insert_one({
            "name": "kv_store",
            "last_code": "000000"
        })

    return app

