"""
Controller pro vytváření balíčků
Vytvořil: Jakub Hlava (xhlava52)
"""
from flask import Blueprint, render_template, request, redirect, url_for, flash, jsonify
from flask_login import login_required, current_user

from itu_app.backend import deck

bp = Blueprint("builder", __name__, url_prefix="/builder")


@bp.route("/create", methods=["GET", "POST"])
@login_required
def create():
    if request.method == "POST":
        name = request.form.get("pkgName")
        desc = request.form.get("pkgDesc", None)
        _, find_code = deck.create(name, desc, current_user.user_data["username"])
        return redirect(url_for("builder.edit", find_code=find_code))
    return render_template("create_deck.html")


@bp.route("/edit/<find_code>")
@login_required
def edit(find_code):
    d = deck.get_by_find_code(find_code)
    if d is None:
        flash("Tento balíček neexistuje!", "danger")
        return redirect(url_for("index.my_decks"))
    return render_template("edit_deck.html", deck=d, last=d["questions"][-1]["_id"]+1 if d["questions"] else 0)


@bp.route("/edit/save/<find_code>", methods=["POST"])
@login_required
def save(find_code):
    q = request.get_json()
    deck.update_questions(find_code, q["questions"])
    return jsonify({"status": "ok"})


@bp.route("/edit/delete/<find_code>")
@login_required
def delete(find_code):
    deck.destroy(find_code)
    flash(f"Balíček {find_code} byl odstraněn!", "success")
    return redirect(url_for("index.my_decks"))


@bp.route("/edit/saveinfo/<find_code>", methods=["POST"])
@login_required
def edit_info(find_code):
    name = request.form.get("name")
    desc = request.form.get("desc")
    deck.update_info(find_code, name, desc)
    return jsonify({"status": "ok"})