from datetime import datetime
from time import strftime, localtime
from flask import Flask, render_template
from flask_sqlalchemy import SQLAlchemy
from flask_login import LoginManager
import uuid
from base64 import b64decode


import configparser
import os
from werkzeug.security import generate_password_hash

db = SQLAlchemy()
login_manager = LoginManager()


def create_app():
    app = Flask(__name__)
    from iis_app.views.main_page import bp as mp_bp
    from iis_app.views.admin import bp as adm_bp
    from iis_app.views.carrier import bp as car_bp
    from iis_app.views.personnel import bp as per_bp
    from iis_app.views.reservation import bp as res_bp


    # config reading from settings.conf file in root directory of module
    config = configparser.ConfigParser()
    config.read(os.path.join(os.path.dirname(__file__), "settings.conf"))

    if "keys" not in config.keys():
        config["keys"] = {
            "app_secret": str(uuid.uuid4().hex)
        }
        with (open(os.path.join(os.path.dirname(__file__), "settings.conf"), "w")) as conffile:
            config.write(conffile)

    # building of database URI from separate credentials in config file
    app.config["SQLALCHEMY_DATABASE_URI"] = f"postgresql://" \
                                            f"{config['database']['user']}" \
                                            f":{b64decode(config['database']['password'])}" \
                                            f"@{config['database']['host']}" \
                                            f":{config['database']['port']}" \
                                            f"/{config['database']['database']}"

    app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False    # suppress deprecation warning
    app.config["SECRET_KEY"] = config["keys"]["app_secret"]

    app.register_blueprint(mp_bp)
    app.register_blueprint(adm_bp)
    app.register_blueprint(car_bp)
    app.register_blueprint(per_bp)
    app.register_blueprint(res_bp)

    # init database ORM
    db.init_app(app)

    # init login manager
    login_manager.init_app(app)

    @app.template_filter("lattostr")
    def lat_to_str(lat):
        if lat is None:
            return None
        dir = "N"
        if lat < 0:
            lat = lat * -1
            dir = "S"
        return f"{lat}°{dir}"

    @app.template_filter("longtostr")
    def long_to_str(long):
        if long is None:
            return None
        dir = "E"
        if long < 0:
            lat = long * -1
            dir = "W"
        return f"{long}°{dir}"

    
    @app.errorhandler(404)
    def page_not_found(e):
        return render_template('404.html'), 404

    
    @app.errorhandler(403)
    def page_not_found(e):
        return render_template('403.html'), 403

    from .model import Connection, Reservation, Stop, StopTimed, StopPending, User, Vehicle, VehicleSection, VehicleSectionSeat
    # helper function, which tells Flask-login how to load users from ORM

    @login_manager.user_loader
    def load_user(id):
        return db.session.get(User, id)

    with app.app_context():
        db.create_all()
        db.session.commit()
        if User.query.filter_by(username="admin").first() is None:
            db.session.add(User(username="admin", password=generate_password_hash("admin"), email="admin@iis.jakubh.cz",
                                phone="+420 777123456", full_name="Administrator", role="admin"))
            db.session.commit()
        carrier = User.query.filter_by(username="carrier").first()
        if carrier is None:
            carrier = User(username="carrier", password=generate_password_hash("carrier"), email="carrier@iis.jakubh.cz",
                           phone="+420 608123456", full_name="IIS Carrier", role="carrier")
            db.session.add(carrier)
            db.session.commit()
        if User.query.filter_by(username="personnel").first() is None:
            db.session.add(User(username="personnel", password=generate_password_hash("personnel"), email="personnel@iis.jakubh.cz",
                           phone="+420 721123456", full_name="IIS Personnel", role="personnel", carrier=carrier.user_id))
            db.session.commit()
        if User.query.filter_by(username="user").first() is None:
            db.session.add(User(username="user", password=generate_password_hash("user"), email="user@iis.jakubh.cz",
                           phone="+420 790123456", full_name="Franta Uživatel", role="user"))
            db.session.commit()

    from iis_app.logic import reserve

    app.jinja_env.globals.update(zip=zip, cdate=datetime.now().date(), price=reserve.get_price, len=len, strftime=strftime, localtime=localtime)
    
    return app
