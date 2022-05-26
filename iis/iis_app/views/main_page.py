from os import name
from flask import Blueprint, render_template, Flask, redirect, url_for, request, flash
from sqlalchemy.sql.expression import false, true
from flask_bootstrap import Bootstrap
from flask_wtf import FlaskForm
from iis_app import db
from iis_app.model import Stop
from iis_app.logic import auth, search, user, reserve
from iis_app.model import Connection, Stop, StopTimed, Vehicle
from sqlalchemy import or_
import datetime


bp = Blueprint("main_page", __name__, url_prefix="/")


@bp.route("/", methods=['GET', 'POST'])
def main_page():
    land_names = list()
    air_names = list()

    for stop in Stop.query.filter(Stop.type != 'plane').all():
        land_names.append(stop.name)

    for stop in Stop.query.filter(Stop.type == 'plane').all():
        air_names.append(stop.name)

    if request.method == "POST":

        if request.form.get("search-btn"):
            frm = request.form.get("from-bar", default="", type=str)
            to = request.form.get("to-bar", default="", type=str)
            date = request.form.get("date-bar", default=datetime.datetime.now().date(), type=str)
            time = request.form.get("time-bar", default=datetime.datetime.now().time(), type=str)
            quantity = request.form.get("quantity", default="", type=str)
        elif request.form.get("search-btn2"):
            frm = request.form.get("from-bar2", default="", type=str)
            to = request.form.get("to-bar2", default="", type=str)
            date = request.form.get("date-bar2", default=datetime.datetime.now().date(), type=str)
            time = request.form.get("time-bar2", default=datetime.datetime.now().time(), type=str)
            quantity = request.form.get("quantity2", default="", type=str)

        #TODO add support for searching valid connections based on ammount of free spaces left
        connection_list, frm_stop, to_stop, conn_date = search.get_connection_list(frm, to, date, time)
        return render_template("index.html", land_names=land_names, air_names=air_names,
                               connection_list=connection_list, frm_stop=frm_stop, to_stop=to_stop,
                               dateusr=conn_date.strftime("%a %d %B %Y"), quantity=quantity, datereq=conn_date.strftime("%Y-%m-%d"),find=True, datetime=conn_date)
    else:
        return render_template("index.html", land_names=land_names, air_names=air_names)


@bp.route("/login", methods=['GET', 'POST'])
def login_page():
    if request.method == "POST":
        username = request.form.get("username", None)
        pwd = request.form.get("password", None)
        if username is None or pwd is None:
            flash("There was an error while logging in, check your username and password and then please try again!", "danger")
            return redirect(url_for("main_page.login_page"))
        result = auth.login(username, pwd, True)
        if result["status"] == "ok":
            flash(f"Welcome back, {username}!", "success")
            return redirect(url_for("main_page.main_page"))
        else:
            flash(f"Error occurred while logging in: {result['reason']}. Please, try again.", "danger")
            return redirect(url_for("main_page.login_page"))
    else:
        return render_template("login.html")


@bp.route("/signup", methods=['GET', 'POST'])
def signup_page():
    if request.method == "POST":
        username = request.form.get("username", None)
        pwd = request.form.get("password", None)
        email = request.form.get("email", None)
        full_name = request.form.get("fullname", None)
        phone = request.form.get("phone_number", None)
        if username is None or pwd is None or email is None:
            flash("Fatal error during registration, please try again later!", "danger")
            return redirect(url_for("main_page.signup_page"))
        result = auth.register(username, pwd, email, "user", full_name=full_name, phone=phone)
        if result["status"] == "ok":
            flash("Registration was successful, you can now log in!", "success")
            return redirect(url_for("main_page.login_page"))
        else:
            flash(f"Error occurred during registration: {result['reason']}", "danger")
            return redirect(url_for("main_page.signup_page"))
    else:
        return render_template("signup.html")


@bp.route("/logout")
def logout():
    result = auth.logout()
    if result["status"] == "ok":
        flash("You were successfully logged out!", "success")
        return redirect(url_for("main_page.main_page"))
    else:
        flash("You cannot log out when you are not logged in!", "danger")
        return redirect(url_for("main_page.main_page"))

    
