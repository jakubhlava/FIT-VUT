"""
    Backend pro práci s uživateli
    Vytvořili: Jakub Hlava (xhlava52), Lukáš Kraus (xkraus13) a Jan Kleisl (xkleis00)
"""
from itu_app import db
from werkzeug.security import generate_password_hash, check_password_hash
from flask_login import login_user, current_user
from itu_app.model.User import User
from bson.objectid import ObjectId

users = db.users
decks = db.decks


def register(username, password):
    """
    Vytvoří účet pro uživatele
    :param username:    jméno
    :param password:    heslo
    :return:            id vytvořeného uživatele nebo -1 při neúspěchu
    """
    if users.find_one({"username": username}) is not None:
        return -1
    user_id = users.insert_one({
        "username": username,
        "password": generate_password_hash(password),
        "decks": {
            "recent": [],
            "fav": [],
            "own": []
        },
        "progress": {},
        "categories": []
    }).inserted_id
    return user_id


def login(username, password):
    """
    Ověří přihalšovací údaje a přihlásí uživatele
    :param username:    jméno
    :param password:    heslo
    :return:
    """
    user = users.find_one({"username": username})
    if user is None:
        return False
    if not check_password_hash(user["password"], password):
        return False
    else:
        u = User(user)
        login_user(u)
        return True


def get_object(username):
    """
    Vrací objekt uživatele vycházející z UserMixin pro potřeby flask_login
    :param username:    jméno
    :return:            objekt uživatele nebo None pokud uživatel neexistuje
    """
    user = users.find_one({"username": username})
    if user is None:
        return None
    else:
        return User(user)


def get_object_by_id(id_):
    """
    Vrací objekt uživatele vycházející z UserMixin pro potřeby flask_login
    :param id_:          id uživatele
    :return:            objekt uživatele nebo None pokud uživatel neexistuje
    """
    user = users.find_one({"_id": ObjectId(id_)})
    if user is None:
        return None
    else:
        return User(user)


def set_latest(username, deck_id):
    """
    Přidá balíček k uživatelovým posledně otevřeným
    :param username:    jméno uživatele
    :param deck_id:     id balíčku
    :return:
    """
    users.update_one({"username": username}, {"$push": {"decks.recent": ObjectId(deck_id)}})
    current_user.flush()


def get_latest(username, count=5):
    """
    Vybere posledních několik balíčků ze seznamu posledních otevřených uživatelem
    :param username:    jméno uživatele
    :param count:       počet balíčků (výchozí 5)
    :return:            seznam balíčků
    """
    deck_arr = users.find_one({"username": username})["decks"]["recent"][::-1]
    return decks.find({"_id": {"$in": deck_arr}})


def get_favs(username):
    """
    Vybere uživatelovy oblíbené balíčky
    :param username:    jméno uživatele
    :param count:       počet balíčků (výchozí 5)
    :return:            seznam balíčků
    """
    deck_arr = users.find_one({"username": username})["decks"]["fav"]
    return decks.find({"_id": {"$in": deck_arr}})


def add_fav(username, deck_id):
    """
    Přidá balíček k uživatelovým oblíbeným
    :param username:    jméno uživatele
    :param deck_id:     id balíčku
    :return:
    """
    users.update_one({"username": username}, {"$push": {"decks.fav": ObjectId(deck_id)}})
    current_user.flush()


def delete_fav(username, deck_id):
    """
    Odebere balíček z uživatelových oblíbených
    :param username:    jméno uživatele
    :param deck_id:     id balíčku
    :return:
    """
    users.update_one({"username": username}, {"$pull": {"decks.fav": ObjectId(deck_id)}})
    current_user.flush()


def set_deck_progress(username, deck_id, progress):
    """
    Upraví postup uživatele balíčkem
    :param username:    jméno uživatele
    :param deck_id:     id balíčku
    :param progress:    id otázky
    :return:
    """
    users.update_one({"username": username}, {"$set": {f"progress.{deck_id}.last": progress}})
    current_user.flush()


def init_deck_progress(username, deck_id):
    """
    Inicializuje pole pro uživatelův postup balíčkem
    :param username:    jméno uživatele
    :param deck_id:     id balíčku
    :return:
    """
    users.update_one({"username": username}, {"$set": {
        f"progress.{deck_id}.last": 0,
        f"progress.{deck_id}.ratings": {}
    }})
    current_user.flush()


def add_question_rating(username, deck_id, qid, rating):
    """
    Přidá sebehodnocení k otázce
    :param username:    uživatelské jméno
    :param deck_id:     id balíčku
    :param qid:         id otázky
    :param rating:      hodnocení (0 - nezvládl, 1 - zvládl)
    :return:
    """
    users.update_one({"username": username}, {"$set": {f"progress.{deck_id}.ratings.{qid}": rating}})
    current_user.flush()


def get_wrong_questions(username, deck_id):
    user = users.find_one({"username": username})
    if user is None:
        return None
    questions = [q for q in user["progress"][deck_id]["ratings"].keys() if user["progress"][deck_id]["ratings"][q] == 0]
    return questions


def clear_deck_progress(username, deck_id):
    """
    Vymaže postup uživatele balíčkem
    :param username:    jméno uživatele
    :param deck_id:     id balíčku
    :return:
    """
    users.update_one({"username": username}, {"$unset": {f"progress.{deck_id}": {}}})


def create_category(username, category_name):
    """
    Vytvoří uživateli kategorii balíčků
    :param username:        jméno uživatele
    :param category_name:   jméno kategorie
    :return:                id kategorie
    """
    user = users.find_one({"username": username}, {"categories": {"$slice": -1}})
    if not user["categories"]:
        last_id = 0
    else:
        last_id = user["categories"][0]["_id"]
    users.update_one({"username": username}, {"$push": {f"categories": {
        "_id": last_id + 1,
        "name": category_name,
        "decks": []
    }}})
    current_user.flush()
    return last_id + 1


def delete_category(username, category_id):
    """
    Odstraní kategorii balíčků
    :param username:        jméno uživatele
    :param category_id:     id kategorie
    :return:
    """
    users.update_one({"username": username}, {"$pull": {f"categories": {"_id": int(category_id)}}})
    current_user.flush()


def add_category_deck(username, category_id, deck_id):
    """
    Přidá do kategorie balíček
    :param username:            jméno uživatele
    :param category_id:         id kategorie
    :param deck_id:             id balíčku
    :return:
    """
    category_pos = list(users.aggregate([
        {
            "$match": {"username": username}
        },
        {
            "$project":
                {
                    "index": {"$indexOfArray": ["$categories._id", int(category_id)]},
                }
        }
    ]))[0]["index"]
    users.update_one({"username": username}, {"$push": {f"categories.{category_pos}.decks": ObjectId(deck_id)}})
    current_user.flush()


def remove_category_deck(username, category_id, deck_id):
    """
    Odebere z kategorie balíček
    :param username:            jméno uživatele
    :param category_id:         id kategorie
    :param deck_id:             id balíčku
    :return:
    """
    category_pos = list(users.aggregate([
        {
            "$match": {"username": username}
        },
        {
            "$project":
                {
                    "index": {"$indexOfArray": ["$categories._id", int(category_id)]},
                }
        }
    ]))[0]["index"]
    users.update_one({"username": username}, {"$pull": {f"categories.{category_pos}.decks": ObjectId(deck_id)}})
    current_user.flush()


def get_categories(username):
    first = users.find_one({"username": username})["categories"]  # todo efektivnější dotaz
    for i in first:
        test = i["decks"]
        i["decks"] = list(decks.find({"_id": {"$in": test}}))
    return first


def get_categories_without_deck(username, find_code):
    deck = decks.find_one({"find_code": find_code})
    allcats = users.find_one({"username": username})["categories"]
    cats = [x for x in allcats if deck["_id"] not in x["decks"]]
    for c in cats:
        if "decks" in c.keys():
            del c["decks"]
    return cats


def get_deck_categories(find_code):
    deck = decks.find_one({"find_code": find_code})
    user_categories = current_user.user_data["categories"]
    deck["categories"] = [
        {
            "_id": cat["_id"],
            "name": cat["name"],
            "presence": True if deck["_id"] in cat["decks"] else False
        } for cat in user_categories
    ]
    return deck


def get_own(username):
    """
    Vybere uživatelovy oblíbené balíčky
    :param username:    jméno uživatele
    :param count:       počet balíčků (výchozí 5)
    :return:            seznam balíčků
    """
    deck_arr = users.find_one({"username": username})["decks"]["own"]  # todo efektivnější dotaz
    return decks.find({"_id": {"$in": deck_arr}})
