"""
Controller pro prostředí prohlížení otázek
Vytvořil: Jan Kleisl (xkleis00)
"""
import random
from flask import request
from os import name, pardir
from flask.templating import render_template
from flask import Blueprint
from itu_app.backend import deck
from itu_app.backend import user
from flask_login import current_user, login_required

bp = Blueprint("question", __name__, url_prefix="/question")


@bp.route("/<find_code>")
@login_required
def main(find_code, select=-1):    
    arr_questions = []
    d = deck.get_by_find_code(find_code)
    username = current_user.user_data['username']
    zeroing_flag = -1
    last_flag = -1
    
    progress = current_user.user_data['progress']
    deck_id = str(d["_id"])

    user.set_latest(username, deck_id)


    if d["questions"] == []:
        return render_template("no_cards.html")

    if request.args.get('continue') == 'no':
        if deck_id not in progress.keys():
            pass
        else:
            for qid in progress[deck_id]["ratings"].keys():
                user.add_question_rating(username, deck_id, qid, 0)
                zeroing_flag = 1
                last_flag = 0
                
    prog = 0
    tmp1 = 0
    _not_learned = 0
         
    for i in d["questions"]:
        if deck_id not in progress:
            pass
        else:
            if (str(i["_id"]) in progress[deck_id]["ratings"].keys() and progress[deck_id]["ratings"][str(i["_id"])] == 1):   
                tmp1 += 1
            elif (str(i["_id"]) in progress[deck_id]["ratings"].keys() and progress[deck_id]["ratings"][str(i["_id"])] == 0):
                _not_learned += 1
        
    if _not_learned == 0:
        if deck_id not in progress:
            pass
        else:
            for qid in progress[deck_id]["ratings"].keys():
                user.add_question_rating(username, deck_id, qid, 0)
            return render_template("package_done.html", fc=find_code)
        
    if _not_learned == 1:
        if last_flag == 0:
            if len(d["questions"]) == 1:
                last_flag = 1
        else:
            last_flag = 1

    if deck_id not in progress.keys():
        user.init_deck_progress(current_user.user_data['username'], d["_id"])
        for i in d["questions"]:
            i["question"] = i["question"].replace("\n", "<br>")
            arr_questions.append(i)
        pass
    else:
        for i in d["questions"]:
            if ((str(i["_id"]) in progress[deck_id]["ratings"].keys() and progress[deck_id]["ratings"][str(i["_id"])] == 0) or str(i["_id"]) not in progress[deck_id]["ratings"].keys()):
                i["question"] = i["question"].replace("\n", "<br>")
                arr_questions.append(i)            

    if zeroing_flag == 1:
        prog = 0
    else:
        prog = tmp1

    data_progress = round((prog / len(d["questions"])*100),1)
    rand = random.choice(arr_questions)
    rand["question"] = rand["question"].replace("\n", "<br>")

    deck_id2 = d["_id"]
    if deck_id not in progress.keys():
        user.init_deck_progress(current_user.user_data['username'], d["_id"])
        for i in d["questions"]:
            user.add_question_rating(username, deck_id2, i["_id"], 0)

    if select != -1:
        for q in d["questions"]:
            if str(q["_id"]) == str(select):
                selected_q_a = q
                break

        return render_template("question.html", q_a=selected_q_a, fc=find_code, dp=data_progress, lf=last_flag, deck=d)
    else:
        if deck_id not in progress.keys():
            return render_template("question.html", q_a=rand, fc=find_code, dp=data_progress, lf=last_flag, deck=d)
        else:
            return render_template("question.html", q_a=rand, fc=find_code, dp=data_progress, lf=last_flag, deck=d)

@bp.route("/umim/<find_code>/<qid>")
@login_required
def umim(find_code, qid):
    username = current_user.user_data['username']
    d = deck.get_by_find_code(find_code)
    deck_id = d["_id"]
    rating = 1
    
    user.add_question_rating(username, deck_id, qid, rating)
    return main(find_code)
    
@bp.route("/neumim/<find_code>/<qid>")
@login_required
def neumim(find_code, qid):
    username = current_user.user_data['username']
    d = deck.get_by_find_code(find_code)
    deck_id = d["_id"]
    rating = 0;
    
    user.add_question_rating(username, deck_id, qid, rating)
    return main(find_code)

@bp.route("/select/<find_code>/<qid>/<selected_q>")
@login_required
def select(find_code, qid, selected_q):
    username = current_user.user_data['username']
    d = deck.get_by_find_code(find_code)
    deck_id = d["_id"]
    rating = 0;
    
    
    user.add_question_rating(username, deck_id, qid, rating)
    user.add_question_rating(username, deck_id, selected_q, rating)
    
    return main(find_code, select=selected_q)  

    