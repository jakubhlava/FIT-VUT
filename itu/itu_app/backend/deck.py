"""
    Backend pro práci s balíčky
    Vytvořili: Jakub Hlava (xhlava52), Lukáš Kraus (xkraus13) a Jan Kleisl (xkleis00)
"""
from itu_app import db
from bson import ObjectId
import re
from pymongo import ASCENDING
from flask_login import current_user

decks = db.decks
system = db.system
users = db.users


def gen_find_code():
    """
    Vygeneruje nový kód pro sdíálení balíčků s přáteli
    :return:
    """
    chars = "0123456789ABCDEFGHIJKLMNOPQRSTVUWXYZ"
    lastfc = list(system.find_one({"name": "kv_store"})["last_code"])
    incdone = False
    index = -1
    while not incdone:
        lastfc[index] = chars[(chars.index(lastfc[index])+1) % (len(chars))]
        if lastfc[index] == "0" and index > -6:
            index -= 1
        else:
            incdone = True
    code = str("".join(lastfc))
    system.update_one({"name": "kv_store"}, {"$set": {"last_code": code}})
    return code


def create(deck_name, deck_description, username):
    """
    Vytvoří nový balíček
    :param public:              udává, zda má být balíček zveřejněn
    :param deck_name:           jméno balíčku
    :param deck_description:    popisek balíčku
    :param username:            jméno uživatele, který balíček vytvořil
    :return:                    id balíčku
    """
    find_code = gen_find_code()
    deck_id = decks.insert_one({
        "name": deck_name,
        "description": deck_description,
        "find_code": find_code,
        "maintainer": username,
        "top_id": 0,
        "questions": []
    }).inserted_id
    users.update_one({"username": username}, {"$push": {"decks.own": ObjectId(deck_id)}})
    current_user.flush()
    return deck_id, find_code


def destroy(find_code):
    """
    Smaže balíček a odstraní zmínky o něm z dotčených uživatelských účtů
    :param deck_id:         id balíčku
    :return:
    """
    deck = decks.find_one({"find_code": find_code})
    deck_id = ObjectId(deck["_id"])
    users.update_one({"username": deck["maintainer"]}, {"$pull": {"decks.own": deck_id}})
    users.update_many({}, {"$pull": {"decks.fav": deck_id}})
    users.update_many({}, {"$pull": {"decks.recent": deck_id}})
    users.update_many({}, {"$unset": {f"progress.{deck_id}": {}}})
    decks.delete_one({"_id": deck_id})
    current_user.flush()


def add_question(deck_id, question, answer):
    """
    Přidá do balíčku otázku
    :param deck_id:         id balíčku
    :param question:        otázka
    :param answer:          odpověď
    :return:
    """
    deck = decks.find_one({"_id": ObjectId(deck_id)}, {"questions": {"$slice": -1}})
    if not deck["questions"]:
        last_id = 0
    else:
        last_id = deck["questions"][0]["_id"]
    decks.update_one({"_id": ObjectId(deck_id)}, {"$push": {"questions": {"_id": last_id+1, "question": question, "answer": answer}}, "$set": {"top_id": last_id}})


def del_question(deck_id, qid):
    """
    Odebere z balíčku otázku
    :param deck_id:         id balíčku
    :param qid:             pořadové číslo otázky
    :return:
    """
    decks.update_one({"_id": ObjectId(deck_id)}, {"$pull": {"questions": {"_id": qid}}})


def get(deck_id):
    """
    Najde balíček podle ID
    :param deck_id:         id balíkču
    :return:                balík nebo None, pokud takový neexistuje
    """
    return decks.find_one({"_id": ObjectId(deck_id)})


def get_by_find_code(find_code):
    """
    Najde balíček podle kódu pro sdílení
    :param find_code:       kód pro sdílení
    :return:                balíček s odpovídajícím kódem nebo None
    """
    return decks.find_one({"find_code": find_code})


def search(query, page, page_size=10):
    """
    Vyhledává balíčky podle části názvu
    :param query:           část jméno balíku
    :return:                seznam balíků odpovídající hledanému dotazu
    """
    fc = get_by_find_code(query)
    if fc:
        results = [fc]
    else:
        results = []
    q = {"name": {"$regex": query, "$options": "i"}}
    results.extend([x for x in decks.find(q).sort("name", ASCENDING).skip((page-1)*page_size).limit(page_size)])
    for r in results:
        r["is_fav"] = True if r["_id"] in current_user.user_data["decks"]["fav"] else False
    for r in results:
        del r["_id"]
    return {
        "count": decks.count_documents(q),
        "results": results,
    }


def update_questions(find_code, new_questions):
    """
    Najde balíček a vymění mu sadu otázek za novou
    :param find_code:        kód pro sdílení
    :param new_questions:    list nových otázek a odpovědí
    :return:
    """
    maxid = max([x["_id"] for x in new_questions]) if new_questions else 0
    top = get_by_find_code(find_code)["top_id"]
    new = max([maxid, top])
    decks.update_one({"find_code": find_code}, {"$set": {"questions": new_questions, "top_id": new}})


def update_info(find_code, name, desc):
    decks.update_one({"find_code": find_code}, {"$set": {"name": name, "description": desc}})