"""
Controller pro register, login, úvodní stránku
Vytvořil: Lukáš Kraus (xkraus13)
"""
from flask.helpers import url_for
from flask import Blueprint, render_template, request, redirect, flash
from itu_app.backend import deck, user
from werkzeug.utils import redirect
from flask_login import current_user, AnonymousUserMixin, logout_user, login_required
from itu_app import db

bp = Blueprint("index", __name__, url_prefix="/")

decks = db.decks

def zpracovani_decku():
    if len(list(user.get_favs(current_user.user_data['username']))) == 0:
        fav_arr = None
    else:
        fav_arr = list(user.get_favs(current_user.user_data['username']))
        

    if len(list(user.get_categories(current_user.user_data['username']))) == 0:
        cat_arr = None
    else:
        cat_arr = list(user.get_categories(current_user.user_data['username']))

    if len(list(user.get_own(current_user.user_data['username']))) == 0:
        own_arr = None
    else:
        own_arr = list(user.get_own(current_user.user_data['username']))

    all_own = []
    if own_arr:
        for own in own_arr:
            all_own.append({"_id" : own["_id"]})

    all_cat2 = []
    if cat_arr:
        for cat in cat_arr:
            hihi = {"name" : cat["name"], "_id" : cat["_id"], "my_cat" : "no", "decks" : cat["decks"]}
            all_cat2.append(hihi)

    all_fav = []
    if fav_arr:
        for fav in fav_arr:
            all_fav.append({"_id" : fav["_id"]})

    ##################################
    #   Zpracování oblíbených decků
    ##################################
    fav_arr2 = None
    if fav_arr:
        fav_arr2 = []
        for fav in fav_arr:            
            fav = user.get_deck_categories(fav["find_code"]) 
            
            ##################################
            #   kontrola, jestli je deck můj
            #   dávám to do "my_deck"
            ##################################

            huh = "no"
            if own_arr:
                for own in all_own:
                    if own["_id"] == fav["_id"]:
                        huh = "yes"
                        break
            fav["my_deck"] = huh
            
            ##################################
            #   kontrola, jestlise deck nachází
            #     v daných kategoriích 
            ##################################

            del fav["description"]
            del fav["questions"]
            del fav["maintainer"]
            del fav["top_id"]
            fav_arr2.append(fav)


    ##################################
    #   Zpracování mých decků
    ##################################
    own_arr2 = None
    if own_arr:
        own_arr2 = []
        for own in own_arr:
                own = user.get_deck_categories(own["find_code"])

            ##################################
            #   kontrola, jestli je deck oblíbený
            #   dávám to do "is_fav"
            ##################################
                
                is_fav = "no"
                for fav in all_fav:
                    if fav["_id"] == own["_id"]:
                        is_fav = "yes"
                        break 
                own["is_fav"] = is_fav

                del own["description"]
                del own["questions"]
                del own["maintainer"]
                del own["top_id"]
                own_arr2.append(own)

    
    ##################################
    #   Zpracování kategorií
    ##################################
    cat_arr2 = []  
    if cat_arr:
        for cat in cat_arr:
            cat_arr2 = []
            for one in cat["decks"]:
                one = user.get_deck_categories(one["find_code"])
                
                ##################################
                #   kontrola, jestli je deck můj
                #   dávám to do "my_deck"
                ##################################

                huh = "no"
                for own in all_own:
                    if own["_id"] == one["_id"]:
                        huh = "yes"
                        break
                one["my_deck"] = huh
                
                ##################################
                #   kontrola, jestli je deck oblíbený
                #   dávám to do "is_fav"
                ##################################
                
                is_fav = "no"
                for fav in all_fav:
                    if fav["_id"] == one["_id"]:
                        is_fav = "yes"
                        break 
                one["is_fav"] = is_fav

                del one["description"]
                del one["questions"]
                del one["maintainer"]
                del one["top_id"]
                cat_arr2.append(one)
            cat["decks"] = cat_arr2
    return fav_arr2, own_arr2, cat_arr

@bp.route("/my_decks", methods=["GET","POST"])
@login_required
def my_decks():

    deck_id = request.args.get('find_code')
    category_id = request.args.get('category_id')
    code = request.args.get('code')
    add_cat_name = request.form.get("add_cat_name")
    if code:
        if int(code) == 1:  #odebere balíček z oblíbených
            user.delete_fav(current_user.user_data['username'], deck_id)
            return redirect(url_for("index.my_decks"))
        if int(code) == 2:  #přidá balíček z oblíbených
            user.add_fav(current_user.user_data['username'], deck_id)
            return redirect(url_for("index.my_decks"))
        if int(code) == 3:  #odebere balíček do dané kategorie
            user.remove_category_deck(current_user.user_data['username'], category_id, deck_id)
            return redirect(url_for("index.my_decks"))
        if int(code) == 4:  #přidat balíček do dané kategorie
            user.add_category_deck(current_user.user_data['username'], category_id, deck_id)
            return redirect(url_for("index.my_decks"))
        if int(code) == 5:  #odebrání kategorie
            user.delete_category(current_user.user_data['username'], category_id)
            return redirect(url_for("index.my_decks"))
        if int(code) == 6:  #přidání nové kategorie
            user.create_category(current_user.user_data['username'], add_cat_name)
            return redirect(url_for("index.my_decks"))

    else:
        fav_arr2, own_arr2, cat_arr = zpracovani_decku()

        return render_template("my_decks.html",favs=fav_arr2, cats=cat_arr, owns=own_arr2)


@bp.route("/main_page")
@login_required
def main_page():
    if type(current_user) == AnonymousUserMixin:
        flash("Nejsi přihlášený!", "danger")
        return redirect(url_for("index.main"))
    code = request.args.get('code')
    deck_id = request.args.get('find_code')
    category_id = request.args.get('category_id')
    add_cat_name = request.form.get("add_cat_name")
    if code:
        if int(code) == 1:  #odebere balíček z oblíbených
            user.delete_fav(current_user.user_data['username'], deck_id)
            return redirect(url_for("index.main_page"))
        if int(code) == 2:  #přidá balíček z oblíbených
            user.add_fav(current_user.user_data['username'], deck_id)
            return redirect(url_for("index.main_page"))
        if int(code) == 3:  #odebere balíček do dané kategorie
            user.remove_category_deck(current_user.user_data['username'], category_id, deck_id)
            return redirect(url_for("index.main_page"))
        if int(code) == 4:  #přidat balíček do dané kategorie
            user.add_category_deck(current_user.user_data['username'], category_id, deck_id)
            return redirect(url_for("index.main_page"))
        if int(code) == 5:  #odebrání kategorie
            user.delete_category(current_user.user_data['username'], category_id)
            return redirect(url_for("index.main_page"))
        if int(code) == 6:  #přidání nové kategorie
            user.create_category(current_user.user_data['username'], add_cat_name)
            return redirect(url_for("index.main_page"))
    else:
        if len(list(user.get_favs(current_user.user_data['username']))) == 0:
            fav_arr = None
        else:
            fav_arr = user.get_favs(current_user.user_data['username'])
        if len(list(user.get_latest(current_user.user_data['username']))) == 0:
            recent_arr = None
        else:   
            recent_arr = user.get_latest(current_user.user_data['username']) 
        return render_template("main_page.html", favs=fav_arr, recent=recent_arr)   


@bp.route("/", methods=["GET", "POST"])
def main():
    if request.method == "POST":
        name = request.form.get("login")
        pw = request.form.get("password")
        if not user.login(name, pw):
            flash("Špatně zadané jméno nebo heslo", "danger")
            return redirect(url_for("index.main"))
        return redirect(url_for("index.main_page")) #změnit
    else:
        if current_user.is_authenticated:
            return redirect(url_for("index.main_page"))
        return render_template("index.html")
    

@bp.route("/register", methods=["GET","POST"])
def register():
    if request.method == "POST":
        name = request.form.get("jmeno")
        pw1 = request.form.get("heslo1")
        pw2 = request.form.get("heslo2")
        if pw1 != pw2:
            flash("Heslo musi byt stejne", "danger")
            return redirect(url_for("index.register"))
        
        if user.register(name, pw1) == -1:
            flash("Přihlašovací jméno již existuje", "danger")
            return redirect(url_for("index.register"))

        
        return render_template("index.html")
    else:
        return render_template("register.html")


@bp.route("/logout")
def logout():
    logout_user()
    return redirect(url_for("index.main"))
