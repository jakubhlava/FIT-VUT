"""
Controller pro vyhledávání balíčků
Vytvořil: Jakub Hlava (xhlava52)
"""
from flask import Blueprint, render_template, request, redirect, url_for, flash, jsonify
from math import ceil
from itu_app.backend import deck, user
from flask_login import current_user
from flask_login import login_required

bp = Blueprint("packages", __name__, url_prefix="/packages")

@bp.route("/")
@login_required
def main():
    return render_template("discover.html")


@bp.route("/search")
@login_required
def search():
    query = request.args.get("query", "")
    page = int(request.args.get("page", 1))
    results = deck.search(query, page)
    total_pages = ceil(results["count"] / 10)
    result = {
        "query": query,
        "page": page,
        "results": results["results"],
        "total": total_pages,
    }
    return jsonify(result)


@bp.route("/get_categories")
@login_required
def get_categories():
    find_code = request.args.get("find_code")
    return jsonify(user.get_categories_without_deck(current_user.user_data["username"], find_code))


@bp.route("/add_to_category", methods=["POST"])
@login_required
def add_to_category():
    find_code = request.form.get("find_code")
    category = int(request.form.get("category"))
    deck_id = deck.get_by_find_code(find_code)["_id"]
    user.add_category_deck(current_user.user_data["username"], category, deck_id)
    return jsonify({"status": "ok"})


@bp.route("/add_fav", methods=["POST"])
@login_required
def add_fav():
    find_code = request.form.get("find_code")
    user.add_fav(current_user.user_data["username"], deck.get_by_find_code(find_code)["_id"])
    return jsonify({"status": "ok"})


@bp.route("/del_fav", methods=["POST"])
@login_required
def del_fav():
    find_code = request.form.get("find_code")
    user.delete_fav(current_user.user_data["username"], deck.get_by_find_code(find_code)["_id"])
    return jsonify({"status": "ok"})
